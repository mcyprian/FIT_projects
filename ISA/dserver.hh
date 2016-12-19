/*
 * @file dserver.hh
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * subject ISA, FIT BUT
 * Implementation of simple DHCP server for IPv4. Messages DISCOVER, OFFER,
 * REQUEST, ACK, NACK and
 * RELEASE are supported.
 */

#ifndef DSERVER_H
#define DSERVER_H

#include <netinet/in.h>
#include <sys/socket.h>

#include <vector>

#include "address_pool.hh"
#include "message_codes.hh"
#include "options.hh"

using namespace std;

#define BUFFER_LENGTH 380

#define REQUEST 0x03
#define ACK 0x05

/* UDP port numbers, server and client. */
#define IPPORT_BOOTPS 67
#define IPPORT_BOOTPC 68

/* BOOT protocol structure */
struct bootp {
  unsigned char bp_op;         /* packet opcode type */
  unsigned char bp_htype;      /* hardware addr type */
  unsigned char bp_hlen;       /* hardware addr length */
  unsigned char bp_hops;       /* gateway hops */
  unsigned int bp_xid;         /* transaction ID */
  unsigned short bp_secs;      /* seconds since boot began */
  unsigned short bp_flags;     /* RFC1532 broadcast, etc. */
  struct in_addr bp_ciaddr;    /* client IP address */
  struct in_addr bp_yiaddr;    /* 'your' IP address */
  struct in_addr bp_siaddr;    /* server IP address */
  struct in_addr bp_giaddr;    /* gateway IP address */
  unsigned char bp_chaddr[16]; /* client hardware address */
  unsigned char bp_sname[64];  /* server host name */
  unsigned char bp_file[128];  /* boot file name */
  unsigned char bp_vend[64];   /* vendor-specific area */
};

class ArgsHolder {
 public:
  ArgsHolder(){};
  ~ArgsHolder();
  struct ip_data *get_data() {
    return &this->data;
  }

 private:
  struct ip_data data;
};

const char *msg_types[9] = {"",        "DISCOVER", "OFFER", "REQUEST",
                            "DECLINE", "ACK",      "NAK",   "RELEASE"};

/* Visualization of DHCP message */
void print_bootp_msg(struct bootp *bootp, option_list_t *op_list);
/* Construct DHCP message to be send as an response to received message. */
int create_response(struct bootp *received, struct bootp *response,
                    char *my_address, struct msg_data *data, AddressPool *pool);
/* Send data stored in buffer to broadcast address. */
void send_to_broadcast(char *buffer, int number_of_bytes,
                       sockaddr_in *client_address, int server_socket,
                       int unicast, in_addr broadcast);
/* Fill properly formated time into buf. */
char *format_time(time_t time, char *buf, bool end);
/* Free all elements in vector v. */
void delete_vector(vector<struct in_addr *> *v);
#endif // ! DSERVER_H
