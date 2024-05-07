#include <gkdi/gkdi_client.h>
#include <gkdi/ndr_gkdi_c.h>

int main(int argc, char ** argv)
{
    (void)argc;
    (void)argv;

    TALLOC_CTX *mem_ctx = talloc_named(NULL, 0, "create_rpc_client");
    struct dcerpc_pipe* pipe = NULL;
    NTSTATUS status;

    status = get_client_rpc_binding(
                                    mem_ctx,
                                    &pipe,
                                    "dc0.domain.alt",
                                    "DOMAIN.ALT",
                                    "administrator");

    if (!NT_STATUS_IS_OK(status)) {
        printf("Failed to establish RPC connection: %u\n", status.v);
        exit(EXIT_FAILURE);
    }

    // Cleanup
    TALLOC_FREE(pipe);
    TALLOC_FREE(mem_ctx);

    return 0;
}
