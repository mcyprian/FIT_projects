/*
 * @file options.hh
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * subject ISA, FIT BUT
 * Implementation of structures to create bootp options.
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#define END_OPTION 0xffffffff

typedef struct option {
  char code;
  char length;
  char *content;
  struct option *next;
} option_t;

typedef struct option_list {
  option_t *head;
  option_t *tail;
} option_list_t;

struct msg_data {
  char type;
  int lease_time;
  struct in_addr mask;
  struct in_addr address;
  int unicast;
};

/*Initialization of option list */
void option_list_init(option_list_t *l);
void add_option(option_list_t *l, unsigned char code, unsigned char length,
                char *content);
void delete_option_list(option_list_t *l);
/* Write content of option list to array of bytes. */
int option_list_to_buffer(char *buf, char *index, option_list_t *l);
/* Create buffer containing data from array of bytes. */
int buffer_to_option_list(char *buf, option_list_t *l);
/* Visualise option list. */
void print_options(option_list_t *l);
int set_options(option_list_t *dest, option_list_t *src, char *server_address,
                struct msg_data *data);

#endif  // !OPTIONS_H
