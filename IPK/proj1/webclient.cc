/**
 * @file webclient.cc
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * First project for subject IPK on FIT, BUT
 * Simple client using sockets API interface, downloads object specified in URL
 * from WWW server.
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <stdexcept>
#include <algorithm>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <stack>

using namespace std;

// structure to store all important parts of URL.
struct Url_data {
    string domain;
    int port_number;
    string path;
    string file_name;
};

/** Opens socket connection to server
 * @param server_hostname name of server
 * @param port_number communication port
 * @returns socket of opened connection
 */
int open_connection(string server_hostname, int port_number) {
    struct sockaddr_in server_address;
    struct hostent *server;
    int client_socket;

    if ((server = gethostbyname(server_hostname.c_str())) == nullptr)
        throw runtime_error("Failed to get host.");

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        throw runtime_error("Failed to create socket.");

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
        throw runtime_error("Failed to connect.");

    return client_socket;
}

/** Replaces all occurances of from substring with to string
 * @param str original string
 * @param from substring to be replaced
 * @param to replacement string
 * @return converted string
 */
string replace_all(string str, const string & from, const string & to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) < str.length()) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

/** Sends GET request to server and reads response header.
 * @param client_socket communication socket 
 * @param url_data data parsed from URL
 * @return response header
 */
string get_response_header(int & client_socket, Url_data & url_data, string http_version) {
    char *buffer = nullptr;
    char *tmp = nullptr;
    int index = 0;
    int length = 4;
    int buffer_length = 1024;
    string http = " HTTP/1." + http_version;
    string response_header;
    // converts problematic characters
    string converted_path = replace_all(url_data.path, " ", "%20");
    converted_path = replace_all(converted_path, "~", "%7E");

    string get_request = "GET " + converted_path + http + "\r\nHost:" + url_data.domain + "\r\n" + "Connection: close \r\n\r\n";

    if ((send(client_socket, get_request.c_str(), get_request.length(), 0)) <= 0)
        throw runtime_error("Failed to send request.");

    if ((buffer = (char *)calloc(buffer_length, CHAR_BIT)) == nullptr)
        throw runtime_error("Failed to alloc memory.");

    do {
        length = (index == 0) ? 4 : 1;
        if ((recv(client_socket, (buffer + index), length, 0)) < 0) {
                free(buffer);
                throw runtime_error("Reading of response failed.");
        }

        index += length;
        if (index >= buffer_length) {
            // buffer is reallocated to double size.
            if ((tmp = (char *)realloc(buffer, buffer_length * 2)) == nullptr) {
                free(buffer);
                throw runtime_error("Failed to realloc memory.");
            }
            buffer_length *= 2;
            buffer = tmp;
        }

    } while (strstr(buffer, "\r\n\r\n") == nullptr);

    response_header = string(buffer);
    free(buffer);
    buffer = nullptr;
    return response_header;
}

/** Checks if byte_queue ends with \r\n characters
 * @param byte_queue queue to be checked
 * @return true if \r\n is at the end of queue, otherwise false
 */
bool ends_with_CRLF(queue<char> byte_queue) {
    if (byte_queue.size() < 2)
        return false;

    stack<char> temp_stack;
    while (!byte_queue.empty()) {
        temp_stack.push(byte_queue.front());
        byte_queue.pop();
    }
    char last = temp_stack.top(); temp_stack.pop();
    char previous = temp_stack.top();
    return (previous == '\r' && last == '\n') ? true : false;
}

/** Reads bytes from opened socket and writes them to file.
 * @param client_socket communication socket
 * @param file_name name of file where data will be writen
 */
void get_received_content(int & client_socket, string file_name, bool chunk) {
    ofstream fout;
    const string DEFAULT_FILE_NAME = "index.html";
    bool in_chunk = false;
    int iRet;
    char byte;
    queue<char> byte_queue;
    if (file_name.empty())
        file_name = DEFAULT_FILE_NAME;
    fout.open(file_name, ios::binary);

    while ((iRet = recv(client_socket, &byte, 1, 0)) == 1) {
        if (iRet < 0)
            throw runtime_error("Failed to read from socket.");
        if (chunk) {
            if (ends_with_CRLF(byte_queue)) {
                if (!in_chunk) {
                    // beggining of chunk, get rid of five bytes.
                    in_chunk = true;
                    while (!byte_queue.empty()) 
                        byte_queue.pop();
                } else {
                    // end of chunk, remove two bytes from queue
                    in_chunk = false;
                    while (!byte_queue.empty() && byte_queue.front() != '\r') {
                        fout.write((char *) &byte_queue.front(), sizeof(byte));
                        byte_queue.pop();
                    }
                    // Remove \r\n
                    byte_queue.pop();
                    byte_queue.pop();
                }
            }
            if (byte_queue.size() < 5) {
                // Queue is not full
                byte_queue.push(byte);
            } else {
                // write first byte to file
                fout.write((char *) &byte_queue.front(), sizeof(byte));
                byte_queue.pop();
                byte_queue.push(byte);
            }
        } else
            fout.write((char *) &byte, sizeof(byte));
    }
    fout.close();
}

/** Converts string to int
 * @param data data to be converted
 * @return integer result of conversion
 */
int str_to_number(const string &data) {
    if (!data.empty()) {
        int data_number;
        istringstream iss(data);
        iss >> dec >> data_number;
        if (iss.fail())
            throw runtime_error("Failed to convert data number to int.");
        return data_number;
    }
    throw runtime_error("Empty string can not be converted to int");
}

/** Parses URL, stores all data to url_data structure
 * @param object_url URL string
 * @return filled url_data structure
 */
Url_data parse_url(string object_url) {    
    const int DEFAULT_PORT = 80;
    string http_prefix = "http://";
    string https_prefix = "https://";
    string port;
    Url_data url_data = { /*domain=*/ "", /*port_number=*/ DEFAULT_PORT, /*path=*/ "/", /*file_name=*/ "" };
    size_t pos;
    size_t path_pos;

    if ((pos = object_url.find(https_prefix)) <= object_url.length()) {
        // https found in url
        object_url = object_url.substr(pos + https_prefix.length());
    } else if ((pos = object_url.find(http_prefix)) <= object_url.length()) {
        // http found in url
        object_url = object_url.substr(pos + http_prefix.length());
    } else
        throw runtime_error("Failed to find http/https, given url was probably invalid.");

    if ((pos = object_url.find(":")) <= object_url.length()) {
        // port is specified
        if ((path_pos = object_url.find("/")) > object_url.length()) {
            port = object_url.substr(pos + 1);
        } else {
            port = object_url.substr(pos + 1 , path_pos - 1 - pos);
            url_data.path += object_url.substr(path_pos + 1);
        }
        url_data.port_number = str_to_number(port);
        url_data.domain = object_url.substr(0, pos);
    } else if ((path_pos = object_url.find("/")) <= object_url.length()) {
        url_data.domain = object_url.substr(0, path_pos);
        url_data.path += object_url.substr(path_pos + 1);
    } else
        url_data.domain = object_url.substr(0, pos);
        if (url_data.path != "/") {
            if ((pos = url_data.path.find_last_of("/")) > object_url.length())
                throw runtime_error("Failed to get file name");
            else {
                url_data.file_name = url_data.path.substr(pos + 1);
                if (!url_data.file_name.empty() && (url_data.file_name[url_data.file_name.length()-1] == '\r' ||
                    url_data.file_name[url_data.file_name.length()-1] == '\n'))
                    url_data.file_name.erase(url_data.file_name.length()-1);
            }
        }
    return url_data;
}

/**
 * Parsing response header, gets response status code
 * @param response_header full header of server response
 * @return status code
 */
string parse_status_code(string response_header) {
    size_t pos;

    if ((pos = response_header.find("HTTP/")) > response_header.length())
        throw runtime_error("Failed to parse response header.");

    string status_code = response_header.substr(pos + string("HTTP/1.X ").length(), 3); 
    // check if parsed status code is in correct format
    str_to_number(status_code);
    return status_code;
}

/** Parses response header, gets new location in case request was redirected
 * @param response_header full header of server response
 * @return new location where request was redirected
 */
string get_location(string response_header) {
    size_t pos;
    string location;

    if ((pos = response_header.find("Location: ")) > response_header.length())
        throw runtime_error("Failed to parse response header.");

    location = response_header.substr(pos + string("Location: ").length());
    pos = location.find("\n");
    location = location.substr(0, pos);
    return location;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Invalid commad line arguments, usage: ./webclient OBJECT_URL." << endl;
        return EXIT_SUCCESS;
    }
    bool success_flag = false;
    int client_socket;
    size_t pos;
    bool chunk;
    int redirections = 0;
    string http_version = "1";
    Url_data url_data;
    string url = argv[1];
    try {
        do {
            url_data = parse_url(url);
            client_socket = open_connection(url_data.domain, url_data.port_number);
            string response_header = get_response_header(client_socket, url_data, http_version);
            string status_code = parse_status_code(response_header);
            chunk = ((pos = response_header.find("chunked")) > response_header.length()) ? false : true;
            switch (status_code.at(0)) {
                case '3':
                    url = get_location(response_header);
                    redirections++;
                    break;
                case '2': 
                    get_received_content(client_socket, url_data.file_name, chunk);
                    success_flag = true;
                    break;
                default:
                    // try http 1.0 
                    if (http_version == "1")
                        http_version = "0";
                    else
                        throw runtime_error(status_code);
            }
        } while (!success_flag && redirections <= 5);
    } catch (exception & e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
