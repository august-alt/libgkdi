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

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

extern "C"
{
#include <gkdi/gkdi_client.h>
#include <gkdi/ndr_gkdi_c.h>
}

class RpcWrapper
{
private:
    TALLOC_CTX* mem_ctx;
    dcerpc_pipe* pipe;

public:
    RpcWrapper(const std::string &host, const std::string& domain, const std::string& username)
        : mem_ctx(nullptr),
          pipe(nullptr)
    {
        mem_ctx = talloc_named(NULL, 0, "create_rpc_client");

        if (!NT_STATUS_IS_OK(get_client_rpc_binding(mem_ctx,
                                   &pipe,
                                   const_cast<char*>(host.c_str()),
                                   const_cast<char*>(domain.c_str()),
                                   const_cast<char*>(username.c_str()))))
        {
            std::cout << "Couldnt obtain RPC server binding. exiting.\n" << std::endl;
            return;
        }
    }

    ~RpcWrapper()
    {
        TALLOC_FREE(pipe);
        TALLOC_FREE(mem_ctx);
    }
};

int main(int argc, char **argv)
{
    std::vector<std::string> args(argv, argv+argc);
    if(args.size() < 4)
    {
        std::cerr << "Usage: " << args[0] << " [host] [domain] [username]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string hostname = args[1];
    std::string domain = args[2];
    std::string username = args[3];

    std::unique_ptr<RpcWrapper> wrapper { new  RpcWrapper(hostname, domain, username) };

    return 0;
}
