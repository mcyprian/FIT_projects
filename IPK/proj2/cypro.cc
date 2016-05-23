/* @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @file cypro.cc
 * @section DESCRIPTION
 *
 * Second project for subject IPK on FIT, BUT
 * CyPro (Cybernetic Yummy Protocol) client and server common interface.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <stdlib.h>

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>

#include "cypro.hh"

const string CyPro::MS = "\a";
const string CyPro::DS = "\b";
const string CyPro::PROTOCOL_HEADER = "CyPRO";

CyPro::CyPro() {
    this->port_number = DEFAULT_PORT;
}

CyPro::~CyPro() {}

/** Sends text message on a connected socket */
void CyPro::send_msg(int comm_socket, string message) {
    if ((send(comm_socket, message.data(), message.size(), 0)) <= 0)
        throw runtime_error("Failed to send message from comm_socket.");
}

/** Receives message on a connected socket */
string CyPro::receive_msg(int comm_socket) {
    char buffer[BUFFER_LENGTH];
    if ((recv(comm_socket, buffer, BUFFER_LENGTH, 0)) < 0)
        throw runtime_error("Failed to read data from comm socket.");
    return string(buffer);
}

/** Reads on a socket till the end of message header */
string CyPro::get_msg_header(int comm_socket) {
    char *buffer = nullptr;
    char *tmp = nullptr;
    int index = 0;
    int buffer_length = 1024;
    int length = 1;

    if ((buffer = (char *)calloc(buffer_length, CHAR_BIT)) == nullptr)
        throw runtime_error("Failed to alloc memory.");

    do {
        if ((recv(comm_socket, (buffer + index), length, 0)) < 0) {
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

    } while (buffer[index - 1] != MS[0]);

    string msg_header = string(buffer);
    free(buffer);
    buffer = nullptr;
    return msg_header;
}

/** Parses all data from message header */
void CyPro::parse_header(string message, string& service, string& file_name, size_t& file_size) {
    string protocol_header;
    string str_size;
    size_t pos = 0;

    pos = message.find(DS);
    protocol_header = message.substr(0, pos);
    message.erase(0, pos + string(DS).length());
    pos = message.find(DS);
    service = message.substr(0, pos);
    message.erase(0, pos + string(DS).length());
    pos = message.find(DS);
    file_name = message.substr(0, pos);
    message.erase(0, pos + string(DS).length());
    str_size = message.substr(0, message.find(DS));
    str_size.pop_back();
    file_size = str_to_number(str_size);
}

/** Reads content of specified file and store it to attribute file_cache */
void CyPro::file_to_cache(string file_name) {
    ifstream fi;
    stringstream str_stream;

    fi.exceptions(ifstream::failbit);
    fi.open(file_name, ios::out);
    str_stream << fi.rdbuf();
    this->file_cache = str_stream.str();
    fi.close();
}

/** Send binary data from file_cache attribute on a socket */
void CyPro::send_file(int comm_socket, string file_name, string message) {
    this->send_msg(comm_socket, message);

    if ((send(comm_socket, this->file_cache.data(), this->file_cache.size(), 0)) <= 0)
        throw runtime_error("Failed to send message from comm_socket.");
}

/** Receives file content on socket and write them to local file */
void CyPro::receive_file(int comm_socket, string file_name, size_t file_size) {
    ofstream fout;
    int iRet;
    char byte;
    size_t readed = 0;
    fout.open(file_name, ios::binary);

    while ((iRet = recv(comm_socket, &byte, 1, 0)) == 1) {
        if (iRet < 0)
            throw runtime_error("Failed to read file data from socket.");
        fout.write((char *) &byte, sizeof(byte));
        readed++;
    }
    if (readed != file_size)
        throw runtime_error("Transmission of file was not competed.");
    fout.close();
}

int str_to_number(const string& data) {
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

string num_to_str(int num) {
    ostringstream ss;
    ss << num;
    return ss.str();
}

