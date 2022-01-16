/*
@Functionality: XML Parser
@Author: Abhishek Singh

There is no limit to the nest level of tags, even though exercise mentioned 5 as limits

@Assumption: I am assuming there is only one root node and there will not be any other node at root node level
Predicates:
    1). We would read from XML files once

Probable Data types are: (With their example representation)
1) integer          ==>  12133
2) float            ==>  213.12313
3) string           ==>  afjanga ego21321123123 1k2j 12 12k4hb124 12
4) interger_array   ==>  [12 1212 12 414 12 12]
5) float_array      ==>  [12.11 1212.1212 1212 1212.43]
6) string_array     ==>  ["abhishek" "singh" "himanshu"]
*/

#include "abvector.hpp" // Our own vector class, implemented for our custom use case
#include <string>
#include <fstream>
#include <stdexcept>

// Debug Includes
#include <iostream>

using XMLNode = struct XMLNode_; // for ease of use

struct XMLNode_
{
    std::string content_;      // data in the tag
    bool isContentArray_{false};
    std::string data_type_;   // probable datatype
    std::string tag_;        // tag name

    abvector<XMLNode_*> childs_;

    XMLNode_() = default;

    ~XMLNode_()
    {
        for (size_t idx = 0; idx < childs_.size(); idx++) 
        {
            delete childs_[idx];
            childs_[idx] = nullptr;
        }
        childs_.clear();
        
        // Uncomment line below to check if deallocation is perfect or not
        //std::cout << "I am gettting destroyed" << std::endl;
    }

    // getter methods
    [[nodiscard]] std::string get_datatype() const    { return data_type_; }
    [[nodiscard]] std::string get_tag() const         { return tag_; }
    [[nodiscard]] std::string get_description() const { return content_; }
    [[nodiscard]] std::string get_indexed_value(int) const;
    [[nodiscard]] std::string get_element_from_string_array(int) const;
    [[nodiscard]] std::string get_element_from_int_or_float_array(int) const;
                  void        populate_string_array(abvector<std::string>&) const;
                  void        populate_int_or_float_array(abvector<std::string>&) const;

    // setter methods
    void set_tag(std::string&& tag)                   { tag_ = std::move(tag); }
    void set_datatype(std::string&& data_type)        { data_type_ = std::move(data_type); }
    void set_content(std::string&& description)       { content_ = std::move(description); }
    void set_if_array(const std::string& description) { if ('[' == description[0]) isContentArray_ = true; }
    void set_indexed_content(int, const std::string&, const std::string&);
};

void XMLNode_::populate_int_or_float_array(abvector<std::string>& elements) const
{
    size_t idx {1};
    std::string element;

    while ( idx < content_.size())
    {
        if (']' == content_[idx]) // we reached the end of array
        {
            if (!element.empty())
            {
                //element has one of the element in the array
                //check the push that element now
                elements.push_back(element);
                element.clear();
            }
            break;
        }

        else if (('\n' == content_[idx]) || ('\t' == content_[idx]) || ('\r' == content_[idx]) || ('\b' == content_[idx]) || (' ' == content_[idx])) 
        {
            if (element.empty())
            {
                continue;
            }
            else 
            {
                //element has one of the element in the array
                //check the push that element now
                elements.push_back(element);
                element.clear();
            }
            idx++;
        }
        else
        {
            element.push_back(content_[idx++]);
        }
    }
}

std::string XMLNode_::get_element_from_int_or_float_array(int index) const
{
    abvector<std::string> elements;

    this->populate_int_or_float_array(elements);

    if (index >= elements.size()) 
    {
        throw std::invalid_argument( " Invalid Indexing " );
    }
    return elements[index];
}

void XMLNode_::populate_string_array(abvector<std::string>& elements) const
{
    size_t idx {1};
    std::string element;

    bool first_colon {false};
    
    while ( idx < content_.size())
    {
        if (']' == content_[idx] && !first_colon) // we reached the end of array
        {
            if (!element.empty())
            {
                element.push_back(content_[idx]);
                elements.push_back(element);
                
                // These 2 lines below are not that necessary but a regular clean up
                first_colon = false;
                element.clear();
            }
            break;
        }

        if ((('\n' == content_[idx]) || ('\t' == content_[idx]) || ('\r' == content_[idx]) || 
            ('\b' == content_[idx]) || (' ' == content_[idx])) && !first_colon)
        {
            idx++;
            continue;   
        }
        else if ('\"' == content_[idx] && !first_colon)
        {
            first_colon = true;
            element.push_back(content_[idx]);
            idx++;
        }
        else if ('\"' == content_[idx] && first_colon) // we completed a string element
        {
            element.push_back(content_[idx]);
            elements.push_back(element);
            first_colon = false;
            element.clear();
            idx++;
        }
        else
        {
            element.push_back(content_[idx]);
            idx++;
        }
    }
}

std::string XMLNode_::get_element_from_string_array(int index) const
{
    abvector<std::string> elements;

    this->populate_string_array(elements);
    
    if (index >= elements.size()) 
    {
        throw std::invalid_argument( " Invalid Indexing " );
    }

    return elements[index];
}

std::string XMLNode_::get_indexed_value(int index) const
{
    abvector<std::string> list_of_elements;

    if (std::string("string_array") == data_type_)
    {
        return this->get_element_from_string_array(index);
    }
    else
    {
        return this->get_element_from_int_or_float_array(index);
    }
}

void XMLNode_::set_indexed_content(int index, const std::string& data, const std::string& datatype)
{
    if (datatype != data_type_.substr(0, data_type_.find('_')))
    {
        throw std::invalid_argument( " Data type mismatch!!!! " );
    }

    abvector<std::string> elements;
    if (std::string("string") == datatype)
    {
        this->populate_string_array(elements);
    }
    else
    {
        this->populate_int_or_float_array(elements);
    }

    elements[index] = data;
    content_.clear(); // Clear the old content

    // Now set the content again
    content_.push_back('[');
    size_t idx = 0;
    while (idx < elements.size())
    {
        content_ = content_ + ' ' + elements[idx];
        idx++;
    }
    content_.push_back(']');
}

//////////// XMLNode_ implementation ends here //////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////// abXMLParser class starts here ///////////
#pragma once

class abXMLParser 
{
private:
    XMLNode* root{nullptr};
    std::string version_;
    std::string encoding_;

    ////APIs for parsing the XML/////
    [[nodiscard]] XMLNode*    parse(std::ifstream&);
                  void        ignore_comment(std::ifstream&);
                  void        skip_special_chars(std::ifstream&);
                  bool        check_if_leaf_node(std::ifstream&);
    [[nodiscard]] std::string parse_description(std::ifstream&);
                  void        parse_version_and_encoding(std::ifstream&);
    [[nodiscard]] std::string get_probable_datatype(const std::string&);
    void                      verify_self_end_tag(std::ifstream&, const std::string&);
    [[nodiscard]] bool        check_if_array(const std::string&);
    [[nodiscard]] std::string get_array_data_type(const std::string&);
    [[nodiscard]] std::string get_data_type(const std::string&);
    [[nodiscard]] bool        is_token_integer(const std::string&);

    // Not using rvalue because length of version's and encoding's length would be less than 15 
    // i.e string would not be heap allocated because of small string optimization
    void                      set_version(const std::string& version) { version_ = version; }
    void                      set_encoding(const std::string& encoding) { encoding_ = encoding; }


    ////APIs for querying parsed XML/////
    [[nodiscard]] abvector<std::string> split_string_tokens(const std::string&);
    [[nodiscard]] const XMLNode*        get_matching_node(const abvector<std::string>& query_tokens, const XMLNode* node,
                                                             int parsed_position);
    [[nodiscard]] bool                  set_data_and_type(const abvector<std::string>& query_tags, XMLNode* node, std::string&& data, 
                                                            std::string&& data_type, int parsed_position);
public:
    abXMLParser(const std::string& filename);
    ~abXMLParser();

    // This class is non-copyeable non-movable
    abXMLParser  ()                                  = delete;
    abXMLParser  (const abXMLParser &other)          = delete;
    abXMLParser& operator=(const abXMLParser &other) = delete;
    abXMLParser  (abXMLParser&& other)               = delete;
    abXMLParser& operator=(abXMLParser&& other)      = delete;


    //// Public APIs to get and set data in parsed tree /////
    [[nodiscard]] abvector<std::string> get_data(std::string& query);
    [[nodiscard]] std::string           set_data(std::string& query, std::string&& data, std::string&& data_type = std::string("Not provided"));
};




////////////////////Implementations below//////////////////////
abXMLParser::abXMLParser(const std::string& filename)
{
    std::ifstream xml_file_stream(filename);

    if (!xml_file_stream.is_open()) 
    {
        std::cerr << "Could not open the file - '" << filename << "'" << std::endl;
        exit(1);
    }

    parse_version_and_encoding(xml_file_stream);

    root = parse(xml_file_stream);
}

abXMLParser::~abXMLParser()
{
    // deleting the root would delete its child, 
    // and its child will delete their child, 
    // this will go on recursively untill everything is deleted
    delete root;
    root = nullptr;
}

abvector<std::string> abXMLParser::get_data(std::string& query) 
{
    abvector<std::string> query_tokens = split_string_tokens(query);

    // Debug lines below
    // std::cout << "Printing the split strings below: \n" << "size of query token is: " << query_tokens.size() << "\n";
    // for(size_t idx = 0; idx < query_tokens.size(); idx++) {
    //     std::cout << query_tokens[idx] << "\n";
    // }

    const XMLNode* matched_node = get_matching_node(query_tokens, root, 0); // search starts from root

    abvector<std::string> results;

    if (nullptr == matched_node) 
    {
        results.push_back(std::string("Queried Tag does not exists")); // return empty results if the query is invalid
        results.push_back(std::string("Invalid datatype"));
    }
    else
    {
        bool isIndexedQuery = is_token_integer(query_tokens[query_tokens.size()-1]);
        if (isIndexedQuery)
        {
            std::string datatype = matched_node->get_datatype();
            datatype.erase(datatype.find('_')); // data type result is ready

            int idx = std::stoi(query_tokens[query_tokens.size()-1]);

            std::string indexedVal = matched_node->get_indexed_value(idx);
            results.push_back(indexedVal);
            results.push_back(datatype);
        }
        else
        {
            results.push_back(matched_node->get_description());
            results.push_back(matched_node->get_datatype());
        }
    }
    return results;
}


std::string abXMLParser::set_data(std::string& query, std::string&& data, std::string&& data_type)
{
    abvector<std::string> query_tokens = split_string_tokens(query);

    const bool success = set_data_and_type(query_tokens, root, std::move(data), std::move(data_type), 0);

    if (success)
    {
        return std::string("Data updated succesfully!!!!");
    }
    else
    {
        return std::string("ERROR!!!! Requested XML tag does not exists!!!!");
    }
}


//// Private APIs below ///////
bool abXMLParser::check_if_leaf_node(std::ifstream& xml_buffer) 
{
    skip_special_chars(xml_buffer);

    char next = xml_buffer.peek();

    return (next == '<') ? false : true;
}

std::string abXMLParser::parse_description(std::ifstream& xml_buffer) 
{
    skip_special_chars(xml_buffer);

    char curr_char;

    std::string description;
    while(xml_buffer.get(curr_char)) 
    {
        char next = xml_buffer.peek();

        if (('<' == curr_char) && ('/' == next)) 
        {
            // we reached the end of XMLNode, we are encountering end tag
            
            char waste = xml_buffer.get(); // get '/' char as it would not be needed anymore
            return description;
        }

        description.push_back(curr_char);
    }

    return std::string(); // we would never reach here until the xml is invalid
}

bool abXMLParser::check_if_array(const std::string& data)
{
    if ('[' != data[0])
    {
        return false;
    }

    size_t idx = data.size() - 1;

    while (idx > 1)
    {
        if (('\n' == data[idx]) || ('\t' == data[idx]) || ('\r' == data[idx]) || ('\b' == data[idx]) || (' ' == data[idx])) 
        {
            idx--;
            continue;
        }
        else if (']' == data[idx])
        {
            return true;
        }
        else
        {
            return false;
        }
        idx--;
    }

    return false; // we would never reach here but adding it due to compiler warning
}

std::string abXMLParser::get_array_data_type(const std::string& data) 
{
    size_t idx = 1;

    std::string element;
    std::string probable_datatype {"integer_array"}; // keeping the default to integer_array

    while ( idx < data.size())
    {
        if (']' == data[idx]) // we reached the end of array
        {
            if (!element.empty())
            {
                //element has one of the element in the array
                //check the data type of that element now
                std::string elem_datatype = get_data_type(element);

                if (std::string("string") == elem_datatype)
                {
                    return std::string("string_array");
                }
                else if (std::string("float") == elem_datatype)
                {
                    probable_datatype = std::string("float_array");
                }

                element.clear();
            }
            break;
        }

        else if (('\n' == data[idx]) || ('\t' == data[idx]) || ('\r' == data[idx]) || ('\b' == data[idx]) || (' ' == data[idx])) 
        {
            idx++;
            if (element.empty())
            {
                continue;
            }
            else 
            {
                //element has one of the element in the array
                //check the data type of that element now
                std::string elem_datatype = get_data_type(element);

                if (std::string("string") == elem_datatype)
                {
                    return std::string("string_array");
                }
                else if (std::string("float") == elem_datatype)
                {
                    probable_datatype = std::string("float_array");
                }

                element.clear();
            }
        }
        else
        {
            element.push_back(data[idx++]);
        }
    }

    return probable_datatype;
}

std::string abXMLParser::get_data_type(const std::string& data)
{
    unsigned int dotCount {0}; // This would help us know about floating data type

    size_t idx = 0;

    while(idx < data.size()) 
    {
        if (('\n' == data[idx]) || ('\t' == data[idx]) || ('\r' == data[idx]) || ('\b' == data[idx]) || (' ' == data[idx])) 
        {
            idx++;
            continue;
        }
        else if ('.' == data[idx])
        {
            idx++;
            dotCount++;
        }
        if (0 != isdigit(data[idx])) // This means number is numeric
        {
            idx++;
            continue;
        }
        else
        {
            return std::string("string");
        }
    }

    if (dotCount > 1)
    {
        return std::string("string");
    }
    else if (1 == dotCount)
    {
        return std::string("float");
    }

    return std::string("integer");
}

std::string abXMLParser::get_probable_datatype(const std::string& data) 
{
    bool isArray = check_if_array(data);

    if (isArray)
    {
        return get_array_data_type(data);
    }
    else
    {
        return get_data_type(data);
    }
}

void abXMLParser::verify_self_end_tag(std::ifstream& xml_buffer, const std::string& tag) 
{
    //std::cout << "Verifying tag: " << tag << std::endl;
    std::string end_tag;

    std::getline(xml_buffer, end_tag, '>');

    if (tag != end_tag)
    {
        std::cout << "original tag: " << tag << " end tag: " << end_tag << std::endl;
        throw std::invalid_argument( "XML Tag mismatch. Invalid XML provided" );
    }
}

void abXMLParser::skip_special_chars(std::ifstream& xml_buffer) 
{
    char first_char;
    while (xml_buffer.get(first_char)) 
    {
        if (('\n' == first_char) || ('\t' == first_char) || ('\r' == first_char) || ('\b' == first_char) || (' ' == first_char)) 
        {
            continue;
        }
        else 
        {
            break;
        }
    }
    xml_buffer.unget();
}

XMLNode* abXMLParser::parse(std::ifstream& xml_buffer) 
{
    ignore_comment(xml_buffer);
    skip_special_chars(xml_buffer);

    /// Actual parsing starts now
    char first_char = xml_buffer.get();
    assert('<' == first_char);

    XMLNode* newNode = new XMLNode;

    //get the tag
    std::string tag;
    std::getline(xml_buffer, tag, '>');
    //std::cout << "New xml node: " << tag << std::endl; // for debug
    newNode->set_tag(std::move(tag));

    // check if the current node is a child node
    if(check_if_leaf_node(xml_buffer)) 
    {
        std::string description = parse_description(xml_buffer);
        std::string datatype = get_probable_datatype(description);

        newNode->set_if_array(description);
        newNode->set_content(std::move(description));
        newNode->set_datatype(std::move(datatype));

        verify_self_end_tag(xml_buffer, newNode->get_tag());        

        // there are no child Node of this Node
        return newNode;
    }
    else 
    {
        char end_tag_identifier;

        skip_special_chars(xml_buffer);

        while (xml_buffer.get(end_tag_identifier)) 
        {
            char next = xml_buffer.peek();
            if (('<' == end_tag_identifier) && ('/' == next)) 
            {
                char waste = xml_buffer.get(); // waste '/' character
                break;
            }

            // If you are here then "newNode" has child/s and we are going to parse that
            xml_buffer.unget();  // if it is not 
            XMLNode* childNode = parse(xml_buffer);
            newNode->childs_.push_back(childNode);

            skip_special_chars(xml_buffer);
        }
    }

    verify_self_end_tag(xml_buffer, newNode->get_tag());
    return newNode;
}


void abXMLParser::parse_version_and_encoding(std::ifstream &xml_buffer) 
{
    skip_special_chars(xml_buffer);
    char first_char = xml_buffer.get();
    char next = xml_buffer.peek();

    std::string meta_data;

    // debug print below
    //std::cout << "Entered into parsing version" << std::endl;
    if ('<' == first_char && '?' != next)
    {
        xml_buffer.unget();
        return;
    }
    else if ('<' == first_char && '?' == next)
    {
        next = xml_buffer.get();
        char temp;
        while (xml_buffer.get(temp)) 
        {
            if ('?' == temp) 
            {
                xml_buffer.unget();
                break;
            }
            else if (' ' == temp)
            {
                continue;
            }
            else
            {
                meta_data.push_back(temp);
            }
        }
    }
    else 
    {
        throw std::invalid_argument( "Invalid XML file provided!!!!" );
    }

    char second_last = xml_buffer.get();
    char last = xml_buffer.get();

    if (!('?' == second_last && '>' == last) || meta_data.find("version") == std::string::npos ||
             meta_data.find("encoding") == std::string::npos || meta_data.find("xml") == std::string::npos)
    {
        throw std::invalid_argument( "Invalid XML file provided!!!!" );
    }

    size_t idx1 = meta_data.find("version=\"");
    size_t idx2 = meta_data.find("encoding=\"");
    size_t idx_term = meta_data.find_last_of('\"');
    
    std::string version = meta_data.substr(idx1+9, (idx2-10-idx1));
    std::string encoding = meta_data.substr(idx2+10, (idx_term-10-idx2));

    set_version(version);
    set_encoding(encoding);

    // Debug print below
    //std::cout << "Exiting parsing version" << std::endl;

    return;
}

void abXMLParser::ignore_comment(std::ifstream &xml_buffer) 
{
    skip_special_chars(xml_buffer);

    char first_char = xml_buffer.get();
    char next = xml_buffer.peek();

    std::string comment;

    if ('<' == first_char && '!' != next)
    {
        xml_buffer.unget();
        return;
    }
    else if ('<' == first_char && '!' == next) 
    {
        next = xml_buffer.get();

        char temp;

        while (xml_buffer.get(temp))
        {
            if ('>' == temp) // we have reached end of comment
            {
                comment.push_back(temp);
                break;
            }
            else if (' ' ==  temp)
            {
                continue;
            }

            comment.push_back(temp);
        }
    }
    else
    {
        throw std::invalid_argument( "Invalid XML file provided!!!!" );
    }

    // Lets do the sanity checks on comment now
    if ('-' == comment[0] && '-' == comment[1] && '>' == comment[comment.size()-1] 
            && '-' == comment[comment.size() - 2] && '-' == comment[comment.size() -3])
    {
        return;
    }
    
    throw std::invalid_argument( "Invalid XML file provided!!!!" );
}

// @Brief: Since the getter query to XML would be '.' based, this function would 
//         split that query into multiple strings
abvector<std::string> abXMLParser::split_string_tokens(const std::string& query) 
{
    abvector<std::string> query_tags;

    size_t init_pos{0}, end_pos{0};
    for(size_t idx = 0; idx < query.size(); idx++) 
    {
        if ('.' == query[idx]) 
        {
            end_pos = idx;

            std::string sub_query = query.substr(init_pos, (end_pos-init_pos));
            query_tags.push_back(std::move(sub_query));

            init_pos = idx+1;
        }
        else if (idx == query.size() - 1) 
        {
            end_pos = idx+1;

            std::string sub_query = query.substr(init_pos, (end_pos-init_pos));
            query_tags.push_back(std::move(sub_query));

            break;
        }
    }

    return query_tags;
}

bool abXMLParser::is_token_integer(const std::string& token)
{
    size_t idx = 0;

    while (idx < token.size())
    {
        if (0 == isdigit(token[idx]))
        {
            return false;
        }
        idx++;
    }
    return true;
}

const XMLNode* abXMLParser::get_matching_node(const abvector<std::string>& query_tokens, const XMLNode* node, int parsed_position)
{
    // sanity check
    if (nullptr ==  node || node->get_tag() != query_tokens[parsed_position]) return nullptr;

    parsed_position++;
    for (size_t idx = 0; idx < node->childs_.size(); idx++) 
    {
        const XMLNode* child_node = node->childs_[idx];
        if (child_node->get_tag() == query_tokens[parsed_position])  
        {
            // we found it
            if (parsed_position == (query_tokens.size() - 1)  || 
                (parsed_position == (query_tokens.size() - 2) && is_token_integer(query_tokens[parsed_position+1]))) 
            {
                //std::cout << "Bingo we found a match" << std::endl;
                //std::cout << "Matched tag is: " << child_node->get_tag() << std::endl;
                return child_node;
            }
            else 
            {
                return get_matching_node(query_tokens, child_node, parsed_position);
            }
        }
    }

    return nullptr;
}

bool abXMLParser::set_data_and_type(const abvector<std::string>& query_tokens, XMLNode* node, std::string&& data, std::string&& data_type, int parsed_position)
{
    if (nullptr ==  node || node->get_tag() != query_tokens[parsed_position]) return false;

    parsed_position++;
    for (size_t idx = 0; idx < node->childs_.size(); idx++) 
    {
        XMLNode* child_node = node->childs_[idx];
        if (child_node->get_tag() == query_tokens[parsed_position])  
        {
            // we found it
            if (parsed_position == (query_tokens.size() - 1)) 
            {
                //std::cout << "Bingo we need to update here" << std::endl;
                //std::cout << "Tag to be updated is : " << child_node->get_tag() << std::endl;
                child_node->set_content(std::move(data));
                child_node->set_datatype(std::move(data_type));
                return true;
            }
            else if (parsed_position == (query_tokens.size() - 2) && is_token_integer(query_tokens[parsed_position+1])) 
            {
                int idx = std::stoi(query_tokens[query_tokens.size()-1]);

                std::string datatype {data_type};
                child_node->set_indexed_content(idx, data, datatype);
                return true;
                
            }
            else 
            {
                return set_data_and_type(query_tokens, child_node, std::move(data), std::move(data_type), parsed_position);
            }
        }
    }

    return false;
}