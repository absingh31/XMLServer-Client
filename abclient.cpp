#include <cassert>
#include "abclient.hpp"

/*
Probable Data types are: (With their example representation)
1) integer          ==>  12133
2) float            ==>  213.12313
3) string           ==>  afjanga ego21321123123 1k2j 12 12k4hb124 12
4) interger_array   ==>  [12 1212 12 414 12 12]
5) float_array      ==>  [12.11 1212.1212 1212 1212.43]
6) string_array     ==>  ["abhishek" "singh" "himanshu"]



Valid Queries example::
1). GET("planes_for_sale.ad.year");
2). GET("planes_for_sale.ad.previous.0");
3). SET("planes_for_sale.ad.year", 1900, integer);
4). SET("planes_for_sale.ad.previous.0", 4, integer);
5). SET("planes_for_sale.ad.prev_string.0", "vedant", string);
*/

/*
Scheme here:
User enters: 
1). GET("planes_for_sale.model.year");
2). SET("planes_for_sale.model.year", 1901, int);

We would modify it and send it to server like this
1). GETplanes_for_sale.model.year
2). SETplanes_for_sale.model.year;int;1901
*/

[[nodiscard]] std::string take_and_process_input() 
{
    std::string input;
    std::cout << "Use \"quit\" to terminate the process=========================== \nEnter your Query: \n";
    getline(std::cin, input);

    // To exist the process
    if (input == std::string("quit")) {
        return input;
    }
    else if(input.size() < 5) 
    { 
        std::cout << "ERROR!!!!!! Query entered is not right...Please enter a valid query..." << std::endl;
        return take_and_process_input();
    }
    
    std::string query = input.substr(0, 3);

    // Basic sanity check
    if(!(query == "GET" || (query == "SET") || (query[query.size()-1] == ';' && query[query.size()-2] == ')' && query[query.size()-3] == '\"'))) 
    {
        std::cout << "ERROR!!!!!! Query entered is not right...Please enter a valid query..." << std::endl;
        return take_and_process_input();
    }

    // process GET /////////////
    if (query == std::string("GET"))
    {
        query.push_back(';');
        size_t pos = 5;
        while(input[pos] != '\"') 
        {
            query.push_back(input[pos++]);
        }
        return query;
    }
    
    // process SET ////////////////
    try
    {
        size_t idx1 = input.find_first_of(",");
        size_t idx2 = input.find_last_of(",");
        size_t idx_term = input.find_last_of(')');

       // This is all emperical below......Hardcoded for now
       query = query + input.substr(5, (idx1-6)) + ";" + input.substr(idx2+1, (idx_term-idx2-1)) + 
                    ";" + input.substr(idx1+1, (idx2-idx1-1));
    }
    catch(...)
    {
        std::cout << "ERROR!!!!!! Query entered is not right...Please enter a valid query..." << std::endl;
        return take_and_process_input();
    }

    // Debug line below 
    //std::cout << "After parsing user's input : " << query << "\n";
    return query;
}

void print_to_console(const std::string& send_message, const std::string& recv_message) 
{
    /*
    Received message would be of form :
    1). ERROR;error_message  ==> This is when XML server throws any exception
    OR
    2). datatype;data  ==> This is when XML server returns some result
    Therefore, we would have to process it here
    */
    if (recv_message.size() > 5 && recv_message.substr(0,5) == std::string("ERROR")) 
    {
        // debug print below
        //std::cout << recv_message << std::endl;
        std::cout << "Invalid operation: " << recv_message.substr(7) << "\n\n\n";
    }
    else if (send_message.substr(0, 3) == std::string("SET")) 
    {
        std::cout << "\n" << recv_message << "\n\n";
    }   
    else if (send_message.substr(0, 3) == std::string("GET"))
    {
        std::cout << "\nResults from server: \n";
        size_t datatype_end_idx = recv_message.find_first_of(';');
        
        std::string datatype;
        
        size_t counter = 0;
        while (counter != datatype_end_idx) 
        {
            datatype.push_back(recv_message[counter++]);
        }
        
        std::string data;
        datatype_end_idx++;
        while (datatype_end_idx < recv_message.size()) 
        {
            data.push_back(recv_message[datatype_end_idx++]);
        }

        std::cout << "XML node's data is: " << data << "\nXML Node's data type is: " << datatype << "\n\n\n\n";
    } 
}

void queryXMLServer(int sock) 
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////// Will be displayed on terminal on the client side////////////////////
    std::cout << "Example Queries: \n1). GET(\"planes_for_sale.model.year\");\n"
                        "2). SET(\"planes_for_sale.model.year\", 1901, integer); //Here \"1901\" is Data for the XML tag and \"integer\" is the DataType\n\n\n";
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    while (true) 
    {
        std::string send_message = take_and_process_input();
        // Debug line below
        //std::cout << "Query being send is: " << send_message << std::endl;
        int n = send(sock, send_message.c_str(), send_message.size() + 1, 0);

        if (send_message == std::string("quit"))
        {
            break; //exit process
        }
        
        if (n < 0) error("ERROR writing to socket");
        if (send_message.size() == 0) break;

        std::string recv_message;
        while (true) 
        {
            char buffer[BUFF_LEN + 1];
            bzero(buffer, BUFF_LEN);
            n = recv(sock, buffer, BUFF_LEN, 0);
            
            if (n < 0) error("ERROR while reading from socket");
            
            recv_message += buffer;

            if (buffer[n - 1] == 0) break;
        }

        print_to_console(send_message, recv_message);
    }
}

[[nodiscard]] abSOCKET verify_args_and_get_server_socket(char **command_line_inputs, abClient &client) {
    assert(command_line_inputs[1] == std::string("-portnumber") && "Please use right input qualifiers");
    assert(command_line_inputs[3] == std::string("-hostservername") && "Please use right input qualifiers");

    char local_hostname[256] = {}; // This is just sanity checking
    gethostname(local_hostname, 256);

    std::string hostname {command_line_inputs[4]};

    if (hostname != local_hostname) 
    {
        std::cout << "Error. Since processes are local to this system, the host name must equal to: '" << local_hostname << "'.\n";
        exit(1);
    }
    std::string port {command_line_inputs[2]};

    int port_number {std::stoi(port)};

    abSOCKET server_socket = client.connect_server(hostname, port_number);

    if (server_socket == abINVALID_SOCKET) 
    {
        std::cout << "Could not connect to the server.\nPlease check the port number.\n";
        exit(1);
    }

    return server_socket;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

////    DRIVER CODE BELOW /////
int main(int argc, char **argv) 
{
    abClient client;

    abSOCKET server_socket = verify_args_and_get_server_socket(argv, client);

    std::cout << "\nConnected to XML server!!!!!!\n";

    queryXMLServer(server_socket); // This would run in loop i.e while(true)

    std::cout << std::endl; // flush stream as we the process is getting terminated
    client.release_self_from_server();

    return 0;
}