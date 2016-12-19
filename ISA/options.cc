/*
 * @file options.cc
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * subject ISA, FIT BUT
 * Implementation of structures to create bootp options.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "constants.hh"
#include "message_codes.hh"
#include "options.hh"

using namespace std;

void option_list_init(option_list_t *l) { l->head = nullptr; }

void add_option(option_list_t *l, unsigned char code, unsigned char length,
                char *content) {
  option_t *new_node;
  new_node = new option_t;
  new_node->code = code;
  new_node->length = length;
  new_node->content = content;
  new_node->next = nullptr;
  if (l->head == nullptr) {
    l->head = new_node;
    l->tail = new_node;
  } else {
    l->tail->next = new_node;
    l->tail = new_node;
  }
}

void delete_option_list(option_list_t *l) {
  option_t *tmp;
  for (option_t *first = l->head; first != nullptr;) {
    tmp = first;
    first = first->next;
    delete tmp;
  }
  l->head = nullptr;
  l->tail = nullptr;
}

int option_list_to_buffer(char *buf, char *index, option_list_t *l) {
  if (index == nullptr) index = buf;
  if (buf == nullptr || l == nullptr) throw runtime_error("nullptr argument");
  option_t *tmp;
  for (tmp = l->head; tmp != nullptr; tmp = tmp->next) {
    *index++ = tmp->code;
    if (tmp->length > 0 && tmp->content != nullptr) {
      *index++ = tmp->length;
      for (int i = 0; i < tmp->length; i++) *index++ = tmp->content[i];
    }
  }
  return index - buf;  // length of options
}

int buffer_to_option_list(char *buf, option_list_t *l) {
  if (buf == nullptr || l == nullptr) throw runtime_error("nullptr argument");
  char *index = buf;
  char option_type;
  char option_len;
  int n = 0;
  while (*index != (signed)END_OPTION && ++n < 10) {
    option_type = *index++;
    option_len = *index++;
    add_option(l, option_type, option_len, index);
    index += option_len;
  }
  return index - buf;  // length of options
}

void print_options(option_list_t *l) {
  if (l == nullptr) throw runtime_error("nullptr argument");
  cout << "*** OPTIONS: ***\n";
  option_t *tmp;
  for (tmp = l->head; tmp != nullptr; tmp = tmp->next) {
    cout << hex << tmp->code << ": " << option_names[(int)tmp->code]
         << " len: " << +tmp->length << endl;
    if (tmp->length > 0 && tmp->content != nullptr) {
      cout << "content: ";
      for (int i = 0; i < tmp->length; i++) cout << +tmp->content[i];
    }
    cout << endl;
  }
}

int set_options(option_list_t *dest, option_list_t *src, char *server_address,
                struct msg_data *data) {
  if (src == nullptr || dest == nullptr)
    throw runtime_error("nullptr argument");
  int retval = 0;
  data->lease_time = 0x100e0000;  // 3600s
  data->address.s_addr = 0;
  data->unicast = 0;

  option_t *tmp;
  add_option(dest, 0x63, 0x00, nullptr);  // Magic cookie
  add_option(dest, 0x82, 0x00, nullptr);  // Magic cookie
  add_option(dest, 0x53, 0x00, nullptr);  // Magic cookie
  add_option(dest, 0x63, 0x00, nullptr);  // Magic cookie

  for (tmp = src->head; tmp != nullptr; tmp = tmp->next) {
    if (tmp->length <= 0 || tmp->content == nullptr) continue;
    switch (tmp->code) {
      case DHCP_MESSAGE_TYPE:
        if (*tmp->content == DHCP_DISCOVER) {
          data->type = DHCP_OFFER;
        } else if (*tmp->content == DHCP_REQUEST) {
          data->type = DHCP_ACK;
        } else  // Unexpected type of message
          throw runtime_error("Unexpected DHCP message type");
        add_option(dest, DHCP_MESSAGE_TYPE, 0x1,
                   &data->type);  // DHCP Message Type
        break;
      case DHCP_REQUESTED_ADDRESS:
        data->address.s_addr = (tmp->content[3] << 24) |
                               (tmp->content[2] << 16) |
                               (tmp->content[1] << 8) | (tmp->content[0]);
        break;
      case DHCP_SERVER_IDENTIFIER:
        if (*(in_addr_t *)tmp->content != *(in_addr_t *)server_address) {
          retval = -1;
        }
        break;
    }
  }
  add_option(dest, 0x36, 0x4, server_address);  // Server Identifier
  return retval;
}
