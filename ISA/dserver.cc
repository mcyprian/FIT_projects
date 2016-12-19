/*
 * @file dserver.cc
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * subject ISA, FIT BUT
 * Implementation of simple DHCP server for IPv4. Messages DISCOVER, OFFER,
 * REQUEST, ACK, NACK and
 * RELEASE are supported.
 */

#include <ctype.h>
#include <netinet/ether.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/types.h>

#include <atomic>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "dserver.hh"

using namespace std;

ArgsHolder::~ArgsHolder() { delete_vector(&this->data.excluded); }

// global flag to stop server loop and SIGINT handler
sig_atomic_t quit = 0;

void catch_signal(int) { quit = true; }

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

char *format_time(time_t time, char *buf, bool end) {
  struct tm *timeinfo;
  timeinfo = localtime(&time);
  strftime(buf, 64, "%F_%R", timeinfo);
  if (end) timeinfo->tm_hour += 1;
  strftime(buf, 64, "%F_%R", timeinfo);
  return buf;
}

void print_bootp_msg(struct bootp *bootp, option_list_t *op_list) {
  cout << "server_address: " << inet_ntoa(bootp->bp_siaddr) << endl;
  cout << "Msg type: " << msg_types[bootp->bp_vend[6]] << endl;
  cout << "op: " << bootp->bp_op << endl
       << "hops: " << bootp->bp_hops << endl
       << "xid: " << bootp->bp_xid << endl;

  cout << "ciaddr: " << inet_ntoa(bootp->bp_ciaddr) << endl;
  cout << "yiaddr: " << inet_ntoa(bootp->bp_yiaddr) << endl;
  cout << "siaddr: " << inet_ntoa(bootp->bp_siaddr) << endl;
  cout << "giaddr: " << inet_ntoa(bootp->bp_giaddr) << endl;

  if (op_list != nullptr) print_options(op_list);
  printf("mac: ");
  int i;
  for (i = 0; i < 6; i++) {
    printf("%2x", bootp->bp_chaddr[i]);
    if (i != 5) printf(":");
  }
  printf("\n");
}

int create_response(struct bootp *received, struct bootp *response,
                    char *my_address, struct msg_data *data,
                    AddressPool *pool) {
  option_list_t op_read_list;
  option_list_t op_write_list;
  int len;
  time_t time;
  char mac[16];
  char time_buf[64];
  char end_time_buf[64];

  option_list_init(&op_read_list);
  option_list_init(&op_write_list);
  buffer_to_option_list((char *)received->bp_vend + 4, &op_read_list);
  if ((set_options(&op_write_list, &op_read_list, my_address, data)) < 0)
    return -1;

  if (data->type == DHCP_RELEASE) {
    pool->release(received->bp_ciaddr);
    return -1;
  }

  try {
    memcpy(mac, received->bp_chaddr, sizeof(received->bp_chaddr));
    switch (data->type) {
      case DHCP_OFFER:
        if (data->address.s_addr == 0 || !pool->is_free(data->address))
          data->address = pool->next_free_address();
        pool->reserve(data->address, string(mac));
        break;
      case DHCP_ACK:
        if (received->bp_ciaddr.s_addr != 0) {
          data->address = received->bp_ciaddr;
          data->unicast = 1;
        }
        if (!pool->is_free(data->address) &&
            !pool->is_reserved(data->address, string(mac)) &&
            !pool->is_assigned(data->address, string(mac)))
          data->type = DHCP_NAK;
        else {
          time = pool->assign(data->address, string(mac));
          printf("%s %s %.64s %.64s\n", ether_ntoa((ether_addr *)mac),
                 inet_ntoa(data->address), format_time(time, time_buf, false),
                 format_time(time, end_time_buf, true));
        }
        break;
    }
  } catch (logic_error err) {
    cerr << "No more free IP addresses";
    return -1;
  }

  response->bp_op = 0x02, response->bp_htype = 0x01, response->bp_hlen = 0x06,
  response->bp_hops = 0x00, response->bp_secs = 0x0000,
  response->bp_xid = received->bp_xid;

  response->bp_ciaddr.s_addr =
      (data->type == DHCP_ACK ? received->bp_ciaddr.s_addr : 0x00000000);
  if (data->type == DHCP_NAK) {
    response->bp_yiaddr.s_addr = 0x00000000;
    response->bp_siaddr.s_addr = 0x00000000;
  } else {
    response->bp_yiaddr = data->address;           // Set IP for client
    response->bp_siaddr = *(in_addr *)my_address;  // Set IP of resever
  }

  response->bp_flags = received->bp_flags;
  response->bp_giaddr.s_addr = received->bp_giaddr.s_addr;
  memcpy(response->bp_chaddr, received->bp_chaddr, sizeof(received->bp_chaddr));
  memcpy(response->bp_file, received->bp_file, sizeof(received->bp_file));

  if (data->type != DHCP_NAK) {
    add_option(&op_write_list, 0x33, 0x4,
               (char *)&data->lease_time);                       // lease_time
    add_option(&op_write_list, 0x01, 0x4, (char *)&data->mask);  // Mask
  }
  add_option(&op_write_list, 0xff, 0x00, nullptr);  // End

  len =
      option_list_to_buffer((char *)response->bp_vend, nullptr, &op_write_list);
  delete_option_list(&op_read_list);
  delete_option_list(&op_write_list);
  return len;
}

void send_to_broadcast(char *buffer, int number_of_bytes,
                       sockaddr_in *client_address, int server_socket,
                       int unicast, in_addr broadcast) {
  if (!unicast) {  // Everything except renew is send as a broadcast
    client_address->sin_family = AF_INET;
    client_address->sin_port = htons(IPPORT_BOOTPC);
    client_address->sin_addr = broadcast;
  }
  /* Send OFFER to client */
  if (sendto(server_socket, buffer, number_of_bytes, 0,
             (struct sockaddr *)client_address, sizeof(*client_address)) < 0) {
    perror("Send");
    throw runtime_error("Failed to send msg to client");
  }
}

void delete_vector(vector<struct in_addr *> *v) {
  while (!v->empty()) {
    delete v->back();
    v->pop_back();
  }
}

void parse_args(int argc, char **argv, struct ip_data *args) {
  if (argc != 3 && argc != 5)
    throw runtime_error(
        "Invalid arguments, usage: ./dserver -p <ip_addresses/mask> [-e "
        "<reserved_ip_addresses>].");

  int option_char;
  size_t pos;
  string net_mask;
  string net;
  string excluded_list;

  while ((option_char = getopt(argc, argv, "p:e:")) != -1) {
    switch (option_char) {
      case 'p':
        net_mask = string(optarg);
        if ((pos = net_mask.find("/")) <= net_mask.length()) {
          net = net_mask.substr(0, pos);
          args->prefix = str_to_number(net_mask.substr(pos + 1));
          if (args->prefix == 0 || args->prefix == 31 || args->prefix == 32)
            throw runtime_error("Invalid prefix.");
          if (inet_aton(net.c_str(), &args->net_address) == 0)
            throw runtime_error(
                "Invalid arguments, usage: ./dserver -p <ip_addresses/mask> "
                "[-e <reserved_ip_addresses>].");
          args->mask.s_addr = swap_endian((-1) << (32 - args->prefix));
          args->broadcast.s_addr = args->net_address.s_addr |
                                   swap_endian(~((-1) << (32 - args->prefix)));
        }
        break;
      case 'e':
        excluded_list = string(optarg);
        while ((pos = excluded_list.find(",")) <= excluded_list.length()) {
          args->excluded.push_back(new in_addr);
          if (inet_aton(excluded_list.substr(0, pos).c_str(),
                        args->excluded.back()) == 0)
            throw runtime_error(
                "Invalid arguments, usage: ./dserver -p <ip_addresses/mask> "
                "[-e <reserved_ip_addresses>].");
          excluded_list = excluded_list.substr(pos + 1);
        }
        args->excluded.push_back(new in_addr);
        if (inet_aton(excluded_list.c_str(), args->excluded.back()) == 0)
          throw runtime_error(
              "Invalid arguments, usage: ./dserver -p <ip_addresses/mask> [-e "
              "<reserved_ip_addresses>].");
        break;
      default:
        throw runtime_error(
            "Invalid arguments, usage: ./dserver -p <ip_addresses/mask> [-e "
            "<reserved_ip_addresses>].");
    }
  }
}

int main(int argc, char **argv) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = catch_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, nullptr);
  ArgsHolder args_holder;
  struct ip_data *args = args_holder.get_data();
  try {
    parse_args(argc, argv, args);
    AddressPool pool(args);
    in_addr my_address = pool.get_my_address();

    struct msg_data msg_data;
    msg_data.mask = args->mask;
    char buffer[BUFFER_LENGTH] = {
        0,
    };
    int server_socket = 0;
    int msg_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    bzero((char *)&client_address, sizeof(client_address));
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;  // IPv4 addressing
    server_address.sin_addr.s_addr =
        htonl(INADDR_ANY);                           // listens to any interface
    server_address.sin_port = htons(IPPORT_BOOTPS);  // listens on this port
    /* Opening UDP socket */
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
      throw runtime_error("Failed to open socket");

    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,
               sizeof(int));
    setsockopt(server_socket, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));

    /* Binding to the port */
    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
      throw runtime_error("Failed to bind to the port");
    /*
     * Main server loop
     */
    for (;;) {
      if ((msg_len =
               recvfrom(server_socket, buffer, BUFFER_LENGTH, 0,
                        (struct sockaddr *)&client_address, &client_len)) < 0) {
        if (quit) break;
        perror("Recv");
        throw runtime_error("Failed to receive message from client");
      }

      struct bootp *received = (struct bootp *)&buffer;
      struct bootp to_send = {
          0,
      };
      int option_len = create_response(
          received, &to_send, (char *)&my_address.s_addr, &msg_data, &pool);
      if (option_len > 0)
        send_to_broadcast((char *)&to_send, sizeof(struct bootp) + option_len,
                          &client_address, server_socket, msg_data.unicast,
                          args->broadcast);

      if (quit) break;
    }
  } catch (exception &e) {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
