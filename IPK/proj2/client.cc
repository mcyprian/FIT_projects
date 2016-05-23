/* @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @file client.cc
 * @section DESCRIPTION
 *
 * Second project for subject IPK on FIT, BUT
 * CyPro (Cybernetic Yummy Protocol) client using sockets API interface.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "client.hh"

CyProClient::CyProClient() {
    this->server = nullptr;
    this->create_socket();
}

CyProClient::CyProClient(int port_number, string hostname) {
    this->port_number = port_number;
    this->set_server(hostname);
    this->create_socket();
}

CyProClient::~CyProClient() {}

/** Creating communication socket */
void CyProClient::create_socket() {
    if ((this->comm_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        throw runtime_error("Failed to create socket");
}

/** Setup server info before connection */
void CyProClient::set_server(string server_hostname) {
    if ((this->server = gethostbyname(server_hostname.c_str())) == nullptr)
        throw runtime_error("Failed to get host server.");

    bzero((char *) &this->server_address, sizeof(this->server_address));
    this->server_address.sin_family = AF_INET;
    bcopy((char *)this->server->h_addr, (char *)&this->server_address.sin_addr.s_addr, this->server->h_length);
    this->server_address.sin_port = htons(port_number);
}

/** Opens connection to server */
void CyProClient::open_connection() {
    if (connect(this->comm_socket, (const struct sockaddr *)&this->server_address, sizeof(this->server_address)) != 0)
        throw runtime_error("Failed to open connection.");
}

/** Send request and uploads binary data of given file to server, using send_file method */
void CyProClient::upload_file(string file_name) {
    this->send_file(this->comm_socket, file_name, this->PROTOCOL_HEADER + this->DS + "SAVE"
            + this->DS + file_name + this->DS + num_to_str(this->file_cache.size()) + this->MS);
    cerr << "INFO: File " << file_name <<  " sent to server.\n";
}

/** Sends download request to server, saves the file if it is available */
void CyProClient::download_file(string file_name) {
    string header;
    string status;
    size_t file_size;

    this->send_msg(this->comm_socket, this->PROTOCOL_HEADER + this->DS + "LOAD"
            + this->DS + file_name + this->DS + "0" + this->MS);
    header = this->get_msg_header(this->comm_socket);
    this->parse_header(header, status, file_name, file_size);
    if (status == "OK") {
        this->receive_file(this->comm_socket, file_name, file_size);
        cerr << "INFO: File " << file_name << " received from server.\n";
    } else if (status == "NOTFOUND") {
        throw runtime_error("File " + file_name + " not found in server's directory");
    }
}

/** Parses command line arguments */
void parse_args(int argc, char **argv, struct args & args) {
    int option_char;

    if (argc != 7)
        throw runtime_error("Invalid commad line arguments, "
                            "usage: ./client -h hostname -p port [-d|u] file_name.");
    while ((option_char = getopt(argc, argv, "h:p:d:u:")) != -1)
        switch (option_char) {
            case 'h':
                args.hostname = string(optarg);
                break;
            case 'p':
                args.port = str_to_number(optarg);
                break;
            case 'd':
                args.file_name = string(optarg);
                args.operation = DOWNLOAD;
                break;
            case 'u':
                args.file_name = string(optarg);
                args.operation = UPLOAD;
                break;
            default:
                throw runtime_error("Invalid commad line arguments, "
                                    "usage: ./client -h hostname -p port [-d|u] file_name.");
        }
    if (args.file_name.find("/") <= args.file_name.length())
        // Get base file name from path
        args.file_name = args.file_name.substr(args.file_name.find_last_of("/\\") + 1);
    
}


int main(int argc, char **argv) {
    struct args args;
    try {
        parse_args(argc, argv, args);
        CyProClient client = CyProClient(args.port, args.hostname);
        if (args.operation == DOWNLOAD) {
            client.open_connection();
            client.download_file(args.file_name);
        } else if (args.operation == UPLOAD) {
            try {
                client.file_to_cache(args.file_name);
            } catch(exception& e) {
                throw runtime_error("Failed to open file " + args.file_name + ".");
            }
            client.open_connection();
            client.upload_file(args.file_name);
        }
    } catch (exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
