/*
@Functionality: Server Side Process
@Author: Abhishek Singh
*/

#include "abclient.hpp"

class abServer 
{
  public:
    abServer(abServer const &) = delete;
    void operator=(abServer const &) = delete;
    abSOCKET& connect_client_process(int port_number);
    bool release_client_process(int how = SHUT_RDWR);
    static bool check_if_port_available(int port);

    abServer() 
    {
        // This would happen only once
        memset(&address_hints, 0, sizeof(address_hints));
        address_hints.ai_family = AF_INET;
        address_hints.ai_socktype = SOCK_STREAM;
        address_hints.ai_protocol = IPPROTO_TCP;
        address_hints.ai_flags = AI_PASSIVE;
    }
    ~abServer() 
    {
        free_array();
        free_socket(socket_listen);
        free_socket(socket_client);
    }

  private:

    bool listening_to(int port_number);
    bool connecting_to();
    void free_array();
    void free_socket(abSOCKET &s);

    
    addrinfo address_hints;
    addrinfo *address_array = 0;
    abSOCKET socket_listen = abINVALID_SOCKET;
    abSOCKET socket_client = abINVALID_SOCKET;
    bool listening = false;
    bool connected = false;
    int port {0};
};



///// Implementation below ////////////
bool abServer::listening_to(int port_number) 
{
    if (listening && port == port_number) 
    {
        return true;
    }
    port = port_number;
    listening = false;

    int result = 0;

    // getaddrinfo
    free_array();
    result = ::getaddrinfo(0, std::to_string(port).c_str(), &address_hints, &address_array);
    if (result != 0) 
    {
        std::cout << "getaddrinfo failed: " << result << std::endl;
        return false;
    }
    addrinfo *addr = address_array;

    free_socket(socket_listen);

    socket_listen = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (socket_listen == abINVALID_SOCKET) 
    {
        std::cout << "Invalid Socket, operation failed: " << errno << std::endl;
        return false;
    }

    result = ::bind(socket_listen, addr->ai_addr, (int)addr->ai_addrlen);

    if (result == abSOCKET_ERROR) 
    {
        std::cout << "Binding to socket failed: " << errno << std::endl;
        return false;
    }

    result = ::listen(socket_listen, SOMAXCONN);

    if (result == abSOCKET_ERROR) 
    {
        std::cout << "Listening to socket failed: " << errno << std::endl;
        return false;
    }

    listening = true;
    return true;
}

bool abServer::connecting_to() 
{
    connected = false;

    free_socket(socket_client);

    socket_client = ::accept(socket_listen, 0, 0);

    if (socket_client == abINVALID_SOCKET) 
    {
        std::cout << "Connection failed: " << errno << std::endl;
        return false;
    }

    connected = true;
    return true;
}

void abServer::free_array() 
{
    if (address_array) 
    {
        ::freeaddrinfo(address_array);
    }
    address_array = 0;
}

void abServer::free_socket(abSOCKET &s) 
{
    if (s != abINVALID_SOCKET) 
    {
        if (::close(s) == abSOCKET_ERROR) 
        {
            std::cout << "Closing socket failed: " << errno << std::endl;
        }
        s = abINVALID_SOCKET;
    }
}

bool abServer::release_client_process(int how) 
{
    int result = 0;

    // shutdown the send half of the connection since no more data will be sent
    result = ::shutdown(socket_client, how);
    if (result == abSOCKET_ERROR) 
    {
        std::cout << "send shutdown failed: " << errno << std::endl;
        return false;
    }
    connected = false;
    return true;
}

abSOCKET & abServer::connect_client_process(int port_number) 
{
    if (listening_to(port_number)) 
    {
        if (!connecting_to()) 
        {
            socket_client = abINVALID_SOCKET;
        }
    }
    return socket_client;
}

bool abServer::check_if_port_available(int port)
{
    abClient client;
    abSOCKET server_socket = client.connect_server("localhost", port);
    while (server_socket != abINVALID_SOCKET)
    {
        ++port;
        server_socket = client.connect_server("localhost", port);
    }
    return client.error_state() == false;
}