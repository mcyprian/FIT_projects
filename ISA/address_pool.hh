/*
 * @file address_pool.hh
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * subject ISA, FIT BUT
 * Implementation of IP addresses pool
 */

#include <netinet/in.h>

#include <cstdint>
#include <ctime>

#define DEFAULT_LEASE_TIME 3600
#define RESERVED_TIME 1

using namespace std;

enum Status { reserved, assigned, expired };

struct ip_data {
  struct in_addr net_address;
  struct in_addr mask;
  struct in_addr broadcast;
  int prefix;
  vector<struct in_addr *> excluded;
};

struct assign_entry {
  in_addr address;
  string mac;
  time_t time;
  Status status;
};

class AddressPool {
 public:
  AddressPool(){};
  AddressPool(struct ip_data *args);
  ~AddressPool();
  /* Atomic methods to get state of address */
  bool is_free(in_addr address);
  bool is_reserved(in_addr address, string mac);
  bool is_assigned(in_addr address, string mac);
  bool is_excluded(in_addr address);
  void print_pool();
  in_addr next_free_address(void);
  assign_entry *get_entry(in_addr address);
  assign_entry *get_entry(string mac);
  /* Methods to change state of address. */
  time_t assign(in_addr address, string mac);
  void release(in_addr address);
  void reserve(in_addr address, string mac);
  bool current_in_subnet(void);
  bool in_this_subnet(in_addr address);
  void reclaim_expired(void);
  in_addr get_my_address(void) const { return this->my_addr; };

 private:
  struct ip_data *data;
  in_addr current_addr;
  vector<struct assign_entry *> assigned_addresses;
  in_addr my_addr;
};

uint32_t swap_endian(uint32_t input);
in_addr_t addr_increment(in_addr_t address);
bool in_subnet(in_addr address, in_addr net, in_addr mask, in_addr broadcast);
in_addr next_address(void);
