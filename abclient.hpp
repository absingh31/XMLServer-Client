/*
@Functionality: Client Side Process
@Author: Abhishek Singh
*/

// socket headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>

#include <cstdio>
#include <cerrno>
#include <iostream>
#include <string.h>
#include <string>

#define abSOCKET int // for better readability
#define abINVALID_SOCKET -1 // anything that is -1 is invalid 
#define abSOCKET_ERROR -1   // most functions that fail return -1 
#define BUFF_LEN 256 // recv buffer length

class abClient 
{
  public:
    // no copies please, only references
    abClient(abClient const &) = delete;
    void operator=(abClient const &) = delete;

    abClient();
    ~abClient();

    abSOCKET& connect_server(std::string host_name, int port_number);
    bool release_self_from_server(int how = SHUT_RDWR);
    bool error_state(int *value = 0);
  private:
    bool connecting_to(std::string host_name, int port_number);
    void free_array();
    void free_socket(abSOCKET &s);

    addrinfo address_hints;
    addrinfo *address_array = 0;
    abSOCKET socket_server = abINVALID_SOCKET;
    bool connected = false;
    bool error = false;
    std::string hostname;
    int port {0};
};


///////// Implementations /////////////
abClient::abClient() 
{
    // right now, this only needs to happen once
    memset(&address_hints, 0, sizeof(address_hints));
    address_hints.ai_family = AF_INET;
    address_hints.ai_socktype = SOCK_STREAM;
    address_hints.ai_protocol = IPPROTO_TCP;
}
abClient::~abClient() 
{
    free_array();
    free_socket(socket_server);
}

abSOCKET& abClient::connect_server(std::string host_name, int port_number) 
{
    if (!connecting_to(host_name, port_number)) 
    {
        socket_server = abINVALID_SOCKET;
    }
    return socket_server;
}

bool abClient::release_self_from_server(int how) 
{
    int result = 0;
    result = ::shutdown(socket_server, how);
    if (result == abSOCKET_ERROR) 
    {
        error = true;
        std::cout << "send shutdown failed: " << errno << std::endl;
        return false;
    }
    connected = false;
    return true;
}

bool abClient::error_state(int *value) 
{
    if (error) 
    {
        if (value) 
        {
            *value = errno;
        }
        error = false;
        return true;
    }
    return false;
}

bool abClient::connecting_to(std::string host_name, int port_number) 
{
    if (connected && hostname == host_name && port == port_number) 
    {
        return true;
    }
    hostname = host_name;
    port = port_number;
    connected = false;

    int result = 0;

    // getaddrinfo
    free_array();
    result = ::getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &address_hints, &address_array);
    if (result != 0) 
    {
        error = true;
        std::cout << "getaddrinfo failed: " << gai_strerror(result) << std::endl;
        return false;
    }
    addrinfo *addr = address_array;

    while (addr != 0) 
    {
        // socket
        free_socket(socket_server);
        
        socket_server = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        
        if (socket_server == abINVALID_SOCKET) 
        {
            error = true;
            std::cout << "socket failed: " << errno << std::endl;
            return false;
        }

        result = ::connect(socket_server, addr->ai_addr, (int)addr->ai_addrlen);

        if (result == abSOCKET_ERROR) 
        {
            addr = addr->ai_next;
            continue;
        }

        break;
    }
    if (addr == 0) 
    {
        return false;
    }

    connected = true;
    return true;
}

void abClient::free_array() 
{
    if (address_array) 
    {
        ::freeaddrinfo(address_array);
    }
    address_array = 0;
}

void abClient::free_socket(abSOCKET &s) 
{
    if (s != abINVALID_SOCKET) 
    {
        if (::close(s) != 0) 
        {
            error = true;
            std::cout << "closesocket failed: " << errno << std::endl;
        }
        s = abINVALID_SOCKET;
    }
}

void error(const char * msg) 
{
    perror(msg);
    exit(1);
}