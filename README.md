# XMLServer-Client

## Project details:
1. Implement an xml parser which satisfies all conditions and restrictions as described below:
   - **Sample XML file could be of type: **(see data.xml)
   - XML tag would not contain any attribute. e.g. <test type="A">1</test> // type="A" would not be there. Do not parse attributes.
   - Every XML tag can contain another xml tag or value. Value could be a number( of any standard data type ), string or an array enclosed within []. An array can contain a string or a number but not another xml tag. [] is not a part of standard xml.
   - There would not be any repeated tags within a tag. e.g. another <ad>..</ad> would not appear within <planes_for_sale>..</...>.
     
     
2. Server side is XML Parser, it should meet the following conditions:
    - It should return an error if a particular tag does not exist or if a particular array index is out of bound or invalid. This error should be provided to client on their query.
    - Once the data is updated, the xml file should be saved to the disk by the xml server.
    - XML file to be used is to be provided as a command line argument.
    - XML does not provide any information about data type, but server would have to accomodate somewhere in its request by figuring out the datatype itself
    
    
3. Task is to query this xml tag from another process, i.e from client

4. Important things to note:
    - No use of any library, either STL or any third party
    - Need to choose an IPC mechanism for communication of server and client
    - Second process which can communicate with the XML process should have IPC connection information as the command line argument itself.
    - We need to provide an interactive interface on client side to query these tags and print the outcome.
    
5. **Sample Queries** from client side and their explanation:
    1. `GET("planes_for_sale.ad.year");`
        Explantion: this would return 1977 along with a probable data type.
    2. `SET("planes_for_sale.ad.year", 1900 );`
        Explanation: and add some way to specify data type while setting the value
    3. `GET("planes_for_sale.ad.previous.0", 20);`
        Explanation:  // 0 is the index of the array "previous"


## [Solution] Steps to launch: (Remember to compile the files with C++17 standard)
1. **Launch 2 terminals** : One for server and one for client  
2. On the **server side terminal**, execute these commands:  
    1. `clang++ abserver.cpp -o abserver -std=c++17`   
    2. `./abserver -portnumber 9898 -filename data.xml`    
   Note: These commands are for Mac so use g++ instead of clang++ if you are using linux  
3. On the **client side terminal**, execute these commands:  
    1. `clang++ abclient.cpp -o abclient -std=c++17`   
    2. `./abclient -portnumber 9898 -hostservername Abhisheks-MacBook-Air.local`  
   Note: These commands are for Mac so use g++ instead of clang++ if you are using linux. hostservername is localhost here, to know your localhost, use "hostname" on your terminal and use that. Here "Abhisheks-MacBook-Air.local" is my hostname. 
4. You can now query from the client process (see example queries above). Use "quit" as a query to terminate the program. 
5. To see the example queries, please check "abclient.cpp". I have written the examples as comments


## Explanation:
1. No STL library had to be used, therefore I implemented my own vector class
2. There is no third party library used.
2. Choice of **IPC** for here is connection through **sockets**
3. Everything is in C++17
4. **2 types of queries are allowed :** `GET` and `SET`
5. To see the list of data types supported, please check "abclient.cpp" or "abXMLParser.hpp". 
6. Since this the communication is between 2 process in the same computer I have used localhost as XML server
7. Basic **error handling is incorporated**

