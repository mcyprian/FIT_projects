/* @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @file client.hh
 * @section DESCRIPTION
 *
 * Second project for subject IPK on FIT, BUT
 * Header file of module client.cc
 */

#ifndef CLIENT_HH
#define CLIENT_HH

#include <netdb.h>
#include <netinet/in.h>

#include "cypro.hh"

using namespace std;

enum operations {NOTSET, DOWNLOAD, UPLOAD};

/** Struct to store command line options of the client */
struct args {
    string hostname;
    string file_name;
    int port;
    int operation;
};

/** Class containg client's metods to connect and communicate with server */
class CyProClient: public CyPro {

public:
    CyProClient();
    CyProClient(int port_number, string hostname);
    ~CyProClient();
    void create_socket();
    struct hostent* get_server() const { return this->server; }
    void set_server(string hostname);
    void open_connection();
    void upload_file(string file_name);
    void download_file(string file_name);

private:
    struct hostent *server;
    struct sockaddr_in server_address;
};

#endif
