/* @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @file server.cc
 * @section DESCRIPTION
 *
 * Second project for subject IPK on FIT, BUT
 * CyPro (Cybernetic Yummy Protocol) server using sockets API interface.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "server.hh"

CyProServer::CyProServer() {
    this->sa_client_len = sizeof(this->sa_client);
    this->create_socket();
    memset(&this->sa, 0, sizeof(this->sa));
    this->sa.sin_family = AF_INET;
    this->sa.sin_addr.s_addr = INADDR_ANY;
    this->sa.sin_port = htons(this->port_number);
    if ((bind(this->comm_socket, (struct sockaddr*)&this->sa, sizeof(this->sa))) < 0)
        throw runtime_error("Bind error.");
    if ((listen(this->comm_socket, 10)) < 0)
        throw runtime_error("Listen error.");
}

CyProServer::CyProServer(int port) {
    this->sa_client_len = sizeof(this->sa_client);
    this->port_number = port;
    this->create_socket();
    memset(&this->sa, 0, sizeof(this->sa));
    this->sa.sin_family = AF_INET;
    this->sa.sin_addr.s_addr = INADDR_ANY;
    this->sa.sin_port = htons(this->port_number);
    if ((bind(this->comm_socket, (struct sockaddr*)&this->sa, sizeof(this->sa))) < 0)
        throw runtime_error("Bind error.");
    if ((listen(this->comm_socket, 10)) < 0)
        throw runtime_error("Listen error.");
}

CyProServer::~CyProServer() {}

/** Creates connection socket */
void CyProServer::create_socket() {
    if ((this->comm_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        throw runtime_error("Failed to create welcomesocket.");
}

/** Comunication with specific client after connection, running in a new thread
 * for each of the clients
*/
void CyProServer::client_service(int comm_socket) {
    char client_info[INET6_ADDRSTRLEN];
    string request;
    string service;
    string file_name;
    size_t file_size;

    if (comm_socket > 0) {
        if (inet_ntop(AF_INET, &this->sa_client.sin_addr, client_info, sizeof(client_info)))
            cerr << "INFO: New connection: client address is " << string(client_info)
                 << " client port is " << ntohs(sa_client.sin_port) << endl;
        request = this->get_msg_header(comm_socket);
        this->parse_header(request, service, file_name, file_size);
        if (service == "LOAD") {
            try {
                this->file_to_cache(file_name);
                this->send_file(comm_socket, file_name, this->PROTOCOL_HEADER + this->DS + "OK"
                        + this->DS + file_name + this->DS + num_to_str(this->file_cache.size()) + this->MS);
                cerr << "INFO: File " << file_name << " sent to " << string(client_info) << ".\n";
            } catch(exception& e) {
                cerr << "WARNING: File " << file_name << " requested by " << string(client_info) << " not found.\n";
                this->send_msg(comm_socket, this->PROTOCOL_HEADER + this->DS + "NOTFOUND"
                        + this->DS + file_name + this->DS + "0" + this->MS);
            }
        } else if (service == "SAVE") {
            receive_file(comm_socket, file_name, file_size);
            cerr << "INFO: File " << file_name << " received from " << string(client_info) << ".\n";
        }
    }
    cerr <<"INFO: Connection " << string(client_info) << " closed.\n";
    close(comm_socket);
}

/** Lambda function running client_service in a new thread */
thread CyProServer::sevice_runner(int comm_socket) {
    return thread([=] { client_service(comm_socket); });
}

/** Infinite main loop of the server */
void CyProServer::loop() {
    vector<thread> service_threads;
   while (true) {
        int comm_socket = accept(this->comm_socket, (struct sockaddr*)&this->sa_client, &this->sa_client_len);
        service_threads.push_back(this->sevice_runner(comm_socket));
   }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Invalid command line arguments, usage: ./server -p port\n";
        return EXIT_FAILURE;
    }
    try {
        CyProServer server = CyProServer(str_to_number(argv[2]));
        server.loop();
    } catch (exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

