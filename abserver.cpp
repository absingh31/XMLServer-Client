#include <vector>
#include <fstream>
#include <cassert>
#include "abserver.hpp"
#include "abXMLParser.hpp"
/*
Probable Data types are: (With their example representation)
1) integer          ==>  12133
2) float            ==>  213.12313
3) string           ==>  afjanga ego21321123123 1k2j 12 12k4hb124 12
4) interger_array   ==>  [12 1212 12 414 12 12]
5) float_array      ==>  [12.11 1212.1212 1212 1212.43]
6) string_array     ==>  ["abhishek" "singh" "himanshu"]
*/

[[nodiscard]] std::string prepare_results_for_get_query(const std::string& recv_message, abXMLParser& parserObj) {
    // Get data format: GETplanes_for_sale.model.year
    std::string send_message;
    try
    {
        std::string query_string {recv_message.substr(4)}; // query string is from 4th position to end
        // Debug line below
        //std::cout << "Query string for parser is: " << query_string << std::endl;
        abvector<std::string> results = parserObj.get_data(query_string);
        send_message = send_message + results[1] + ";" + results[0];
        // Debug line below
        //std::cout << "Message to be sent is : " << send_message << std::endl;
    } 
    catch(const std::exception& ex) 
    {
        std::string err_mssg = ex.what();
        send_message = std::string("ERROR;") + err_mssg;
    }
    catch(const std::string& ex)
    {
        send_message = std::string("ERROR;") + ex;
    }
    catch(...)
    {
        send_message = std::string("ERROR;") + std::string("Unknown Error Occured");
    }

    return send_message;
}

[[nodiscard]] std::string prepare_results_for_set_query(const std::string& recv_message, abXMLParser& parserObj) {
    // Set data format: SETplanes_for_sale.model.year;int;1901

    std::string send_message;
    try
    {
        std::string query_string {recv_message.substr(3)}; // query string is from 4th position to end
        // Debug line below
        //std::cout << "Query string for parser is: " << query_string << std::endl;

        // Parse the query_string and prepare data to for set_data function of parser
        size_t first_idx = query_string.find_first_of(';');
        size_t second_idx = query_string.find_last_of(';');
        std::string query_tag {query_string.substr(0, first_idx)};
        // Parse of query_string is complete


        send_message = parserObj.set_data(query_tag, query_string.substr(second_idx+1), 
                                                    query_string.substr(first_idx+1, (second_idx-first_idx)));
    } 
    catch(const std::exception& ex) 
    {
        std::string err_mssg = ex.what();
        send_message = std::string("ERROR;") + err_mssg;
    }
    catch(const std::string& ex)
    {
        send_message = std::string("ERROR;") + ex;
    }
    catch(...)
    {
        send_message = std::string("ERROR;") + std::string("Unknown Error Occured");
    }

    return send_message;
    // Debug line below
    //std::cout << "Message to be sent is : " << send_message << std::endl;
}

[[nodiscard]] std::string prepare_data_to_send(const std::string& recv_message, abXMLParser& parserObj) {
    // Debug line below
    //std::cout << "Query received is: " << recv_message << std::endl;
    // This is how server will get request from client process
    //GETplanes_for_sale.model.year
    //SETplanes_for_sale.model.year;int;1901
    if (recv_message.substr(0,3) == std::string("GET")) 
    {
        return prepare_results_for_get_query(recv_message, parserObj);
    }
    else
    {
        return prepare_results_for_set_query(recv_message, parserObj);
    }
}

[[nodiscard]] bool process_client_request(const abSOCKET client_socket, abXMLParser& parserObj) {
    while (true)
    {
        // Recieve and send
        int n;
        std::string recv_message;

        while (true)
        {
            char buffer[BUFF_LEN + 1];
            bzero(buffer, BUFF_LEN);
            n = recv(client_socket, buffer, BUFF_LEN, 0);
            if (n < 0) error("ERROR while reading from socket");
            recv_message += buffer;
            if (buffer[n - 1] == 0) break;
        }
        if (recv_message.size() == 0 || recv_message == std::string("quit"))
        {
            return true; // Client is disconnected or terminated
        }

        std::string send_message = prepare_data_to_send(recv_message, parserObj);

        n = send(client_socket, send_message.c_str(), send_message.size() + 1, 0);

        if (n < 0) error("ERROR writing to socket");
    }
}

[[nodiscard]] int verify_args_and_get_port_number(char **command_line_inputs) {
    assert(command_line_inputs[1] == std::string("-portnumber") && "Please provide right input qualifier/s.");
    assert(command_line_inputs[3] == std::string("-filename") && "Please provide right input qualifier/s.");

    std::string port {command_line_inputs[2]};
    int port_number {std::stoi(port)};

    if (!abServer::check_if_port_available(port_number))
    {
        std::cout << "Could not connect to port number: " << port_number << "." << "\nMaybe try using a different port number above 1024.\n";
        exit(1);
    }

    return port_number;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/////    DRIVER CODE for server //////
int main(int argc, char **argv)
{
    const int port_number = verify_args_and_get_port_number(argv);
    
    std::cout << "Activating XML server ....\nActivated\n\n";
    
    std::string filename {argv[4]};
    abXMLParser parserObj(filename);

    std::cout << "Parsed XML file...\n\n" << "XML Server is listening on port number: " << port_number << "\n\n";

    abServer server;
    while (true)
    {
        const abSOCKET client_socket = server.connect_client_process(port_number);
        
        if (client_socket == abINVALID_SOCKET)
        {
            break;
        }

        std::cout << "Connected to client process\n";

        const bool isClientDisconnected = process_client_request(client_socket, parserObj);

        if (isClientDisconnected)
        {
            std::cout << "\nShutting Down XML server.....\n" << std::endl;
            break;
        }
        
        std::cout << std::endl;
        server.release_client_process();
    }
    return 0;
}