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

#ifndef GKDI_CLIENT_H
#define GKDI_CLIENT_H

#include <dcerpc.h>
#include <talloc.h>
#include <ndr.h>

/**
 * @brief get_client_rpc_binding - Creates RPC binding.
 * @param mem_ctx                - TALLOC memory context.
 * @param pipe                   - DCERPC pipe.
 * @param hostname               - FQDN or IP of domain controller to connect to.
 * @param domain                 - domain name.
 * @param username               - user name.
 * @return
 *          - NT_STATUS_OK - on success.
 *          - NT_STATUS_RPC_BINDING_HAS_NO_AUTH - if unable to create gss auth identity.
 *          - NT_STATUS_ACCESS_DENIED - if unable to set client identity.
 *          - NT_STATUS_UNSUCCESSFUL - if unable to set kerberos state.
 *          - NT_STATUS_PRC* - if unable to establish RPC connection.
 */
NTSTATUS
get_client_rpc_binding(
    TALLOC_CTX *mem_ctx,
    struct dcerpc_pipe **pipe,
    char * binding_string,
    char * domain,
    char * username
);

#endif//GKDI_CLIENT_H
