/* @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @file server.hh
 * @section DESCRIPTION
 *
 * Second project for subject IPK on FIT, BUT
 * Header file of module server.cc
 */

#ifndef SERVER_HH
#define SERVER_H

#include <netinet/in.h>

#include "cypro.hh"

using namespace std;

/** Class containg methods for accepting clients  and processing of their's requests */
class CyProServer: public CyPro {

public:
    CyProServer();
    CyProServer(int port);
    ~CyProServer();
    virtual void create_socket();
    void client_service(int comm_socket);
    thread sevice_runner(int comm_socket);
    void loop();
private:
    struct sockaddr_in sa;
    struct sockaddr_in sa_client;
    socklen_t sa_client_len;
};

#endif
