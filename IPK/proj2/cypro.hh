/* @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @file cypro.hh
 * @section DESCRIPTION
 *
 * Second project for subject IPK on FIT, BUT
 * Header file of module cypro.cc
 */

#ifndef CYPRO_HH
#define CYPRO_HH

#include <string>

using namespace std;

/* An abstract class providing common interface of client and server. */
class CyPro {

public:
    CyPro();
    ~CyPro();
    int get_port() const { return this->port_number; }
    void set_port(int port) {this->port_number = port; }
    static void send_msg(int comm_socket, string message);
    static string receive_msg(int comm_socket);
    static string get_msg_header(int comm_socket);
    static  void parse_header(string request, string& service, string& file_name, size_t& file_siz);
    void file_to_cache(string file_name);
    void send_file(int comm_socket, string file_name, string message);
    void receive_file(int comm_socket, string file_name, size_t file_size);
    virtual void create_socket() = 0;

protected:
    static const int BUFFER_LENGTH = 1024;
    static const int DEFAULT_PORT = 5050;
    int port_number;
    int comm_socket;
    string file_cache;
    static const string MS; // message separator
    static const string DS; // data separator
    static const string PROTOCOL_HEADER;
};

/** Converts string to int
 * @param data data to be converted
 * @return integer result of conversion
 */
int str_to_number(const string &data);

/** Converts number to string */
string num_to_str(int num);
#endif
