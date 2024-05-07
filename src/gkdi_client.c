/***********************************************************************************************************************
**
** Copyright (C) 2024 BaseALT Ltd. <org@basealt.ru>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
***********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dcerpc.h>
#include <ndr.h>
#include <param.h>
#include <credentials.h>
#include <tevent.h>

#include <locale.h>
#include <errno.h>

#include <openssl/md5.h>
#include <openssl/hmac.h>

#include <krb5.h>
#include <gssapi/gssapi.h>
#include <gssapi/gssapi_ext.h>
#include <gssapi/gssapi_generic.h>
#include <gssapi/gssapi_krb5.h>

#include "gkdi.h"
#include "ndr_gkdi_c.h"

/* Defines related to GSS authentication */

#ifndef GSSAPI_MECH_SPNEGO
/*
 * SPNEGO MECH OID: 1.3.6.1.5.5.2
 * http://www.oid-info.com/get/1.3.6.1.5.5.2
 */
#define GSSAPI_MECH_SPNEGO "\x2b\x06\x01\x05\x05\x02"
#define GSSAPI_MECH_SPNEGO_LEN 6
#endif

int cli_credentials_set_client_gss_creds(
    struct cli_credentials *cred,
    struct loadparm_context *lp_ctx,
    gss_cred_id_t gssapi_cred,
    enum credentials_obtained obtained,
    const char **error_string);

static uint32_t
rpc_create_gss_auth_identity(
    const char *user,
    const char *domain,
    gss_cred_id_t *rpc_identity_h)
{
    OM_uint32 minor = 0;
    OM_uint32 major = 0;
    gss_OID_desc spnego_mech_oid = {GSSAPI_MECH_SPNEGO_LEN, (void *) GSSAPI_MECH_SPNEGO};
    gss_buffer_desc name_buf = {0};
    gss_name_t gss_name_buf = NULL;
    size_t upn_len = 0;
    char *upn = NULL;
    gss_cred_id_t cred_handle = NULL;
    gss_OID_desc mech_oid_array[1];
    gss_OID_set_desc desired_mechs = {0};

    if (domain)
    {    
        upn_len = strlen(user) + 1 + strlen(domain) + 1;
        upn = calloc(upn_len, sizeof(char));
        if (!upn)
        {
            major = GSS_S_FAILURE;
            minor = 0;
        }
        snprintf(upn, upn_len, "%s@%s", user, domain);
    }
    else
    {
        upn = (char *) user;
    }
    name_buf.value = upn;
    name_buf.length = strlen(name_buf.value);
    major = gss_import_name(
              &minor,
              &name_buf,
              GSS_C_NT_USER_NAME,
              &gss_name_buf);
    if (major)
    {
        goto error;
    }

    desired_mechs.count = 1;
    desired_mechs.elements = mech_oid_array;
    desired_mechs.elements[0] = spnego_mech_oid;
    major = gss_acquire_cred(
              &minor,
              gss_name_buf,
              0,
              &desired_mechs,
              GSS_C_INITIATE,
              &cred_handle,
              NULL,
              NULL);
    if (major)
    {
        goto error;
    }

    *rpc_identity_h = cred_handle;

error:
    if (major)
    {
        major = minor ? minor : major;
    }

    if (upn != user)
    {
        free(upn);
    }
    if (gss_name_buf)
    {
        gss_release_name(&minor, &gss_name_buf);
    }

    return major;
}

NTSTATUS
get_client_rpc_binding(
    TALLOC_CTX *mem_ctx,
    struct dcerpc_pipe **pipe,
    char * hostname,
    char * domain,
    char * username
)
{
    struct dcerpc_binding *binding;
    struct cli_credentials *credentials;
    NTSTATUS status;

    status = dcerpc_init();
    if (!NT_STATUS_IS_OK(status))
    {
        printf("Failed to initialize dcerpc: %d\n", status.v);
        return status;
    }

    char* binding_string = talloc_asprintf(mem_ctx, "ncacn_ip_tcp:%s[seal]", hostname);

    // Parse the binding string
    status = dcerpc_parse_binding(mem_ctx, binding_string, &binding);
    if (!NT_STATUS_IS_OK(status))
    {
        printf("Failed to parse binding string: %d\n", status.v);
        return status;
    }

    gss_cred_id_t id;

    int32_t operation_result = 0;

    operation_result = rpc_create_gss_auth_identity(username, domain, &id);
    if (operation_result != 0)
    {
        printf("Failed to create gss auth identity!\n");
        return NT_STATUS_RPC_BINDING_HAS_NO_AUTH;
    }

    // Set up the credentials
    struct loadparm_context *lp_ctx = loadparm_init(mem_ctx);
    credentials = cli_credentials_init(mem_ctx);
    cli_credentials_set_conf(credentials, lp_ctx);

    const char* error_string = NULL;

    operation_result = cli_credentials_set_client_gss_creds(credentials,
                                                            lp_ctx,
                                                            id,
                                                            CRED_SPECIFIED,
                                                            &error_string);
    if (operation_result != 0)
    {
        printf("Failed to set gss credentials: %s\n", error_string);
        return NT_STATUS_ACCESS_DENIED;
    }

    bool ok = cli_credentials_set_kerberos_state(credentials, CRED_USE_KERBEROS_REQUIRED, CRED_SPECIFIED);
    if (!ok)
    {
        printf("Failed to set credentials kerberos state!\n");
        return NT_STATUS_UNSUCCESSFUL;
    }

    struct tevent_context *ev = tevent_context_init(mem_ctx);

    status = dcerpc_pipe_connect_b(mem_ctx, pipe, binding, &ndr_table_ISDKey, credentials, ev, lp_ctx);

    if (!NT_STATUS_IS_OK(status))
    {
        printf("Failed to establish RPC connection: %u\n", status.v);
        return status;
    }

    // OK STATUS
    return NT_STATUS(0);
}
