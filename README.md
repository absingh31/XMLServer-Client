# XMLServer-Client

## Tasks details:
1. Implement an xml parser which satisfies all conditions and restrictions as described below. This code should be all yours and not downloaded from the internet.
2. Task is to query this xml tag from another process( write code for this process too) in the following way: 

GET("planes_for_sale.ad.year"); // this would return 1977 along with a probable data type. Note: XML does not provide any information about data type, but you would have to accomodate somewhere in your request.
SET("planes_for_sale.ad.year", 1900 ); // some way to specify data type while setting the value. It's up to you on how you want to specify.
GET("planes_for_sale.ad.previous.0", 20); // 0 is the index of the array "previous"

Sample XML file could be of type: (see data.xml)

Following things are to be noted:
1. XML tag would not contain any attribute. e.g. <test type="A">1</test> // type="A" would not be there. This is for simplification. Do not parse attributes.
2. Every XML tag can contain another xml tag or value. Value could be a number( of any standard data type ), string or an array enclosed within []. An array can contain a string or a number but not another xml tag. [] is not a part of standard xml.
3. Maximum nesting levels of XML tags can be unlimited.
4. There would not be any repeated tags within a tag. e.g. another <ad>..</ad> would not appear within <planes_for_sale>..</...>.
5. You should return an error if a particular tag does not exist or if a particular array index is out of bound or invalid. This error should be provided to another program.
6. Once the data is updated, the xml file should be saved to the disk by the xml server.
7. Do not use STL libraries like vector, queue and list. Manage all data structure within the code yourself. 
8. For interprocess communication choice is yours for mechanism.
9. All code should be in C/C++.
10. Do not use any third party libraries.
11. XML file to be used is to be provided as a command line argument.
12. Second process which can communicate with the XML process should have IPC connection information as the command line argument itself.
13. Provide an interactive interface in the second process to query these tags and print the outcome.

## Steps to launch: (Remember to compile the files with C++17 standard)
1) Launch 2 terminals : One for server and one for client
2) On the server side terminal, execute these commands:
    a) clang++ abserver.cpp -o abserver -std=c++17
    b) ./abserver -portnumber 9898 -filename data.xml
   These commands are for Mac so use g++ instead of clang++ if you are using linux

3). On the client side terminal, execute these commands:
    a) clang++ abclient.cpp -o abclient -std=c++17
    b) ./abclient -portnumber 9898 -hostservername Abhisheks-MacBook-Air.local
   These commands are for Mac so use g++ instead of clang++ if you are using linux.
   hostservername is localhost here, to know your localhost, use "hostname" on your terminal and use that. Here "Abhisheks-MacBook-Air.local" is my hostname

4). You can now query from the client process. Use "quit" as a query to terminate the program.


## Explanation:
No STL library used, therefore I implemented my own vector class
There is no third party library used.
Choice of **IPC** for here is connection through **sockets**
Everything is in C++17
**2 types of queries are allowed : GET and SET**

To see the list of data types supported, please check "abclient.cpp" or "abXMLParser.hpp". 
I have written the list of datatypes supported as comments
To see the example queries, please check "abclient.cpp". I have written the examples as comments
Since this the communication is between 2 process in the same computer I have used localhost as XML server
Basic **error handling is incorporated**
