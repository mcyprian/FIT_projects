/*
 * @file address_pool.cc
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * subject ISA, FIT BUT
 * Implementation of IP addresses pool
 */

#ifndef ADDRESS_POOL_H
#define ADDRESS_POOL_H

#include <arpa/inet.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "address_pool.hh"

AddressPool::AddressPool(struct ip_data *args) {
  this->data = args;
  try {
    this->my_addr.s_addr = 0;
    this->current_addr = this->data->net_address;
    this->my_addr = this->next_free_address();
    this->current_addr = this->next_free_address();
  } catch (logic_error err) {
    throw runtime_error("Invalid address and mask");
  }
}

AddressPool::~AddressPool() {
  while (!this->assigned_addresses.empty()) {
    delete this->assigned_addresses.back();
    this->assigned_addresses.pop_back();
  }
}

bool AddressPool::is_free(in_addr address) {
  this->reclaim_expired();
  if (!this->in_this_subnet(address) || this->is_excluded(address))
    return false;

  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    if (this->assigned_addresses[i]->address.s_addr == address.s_addr) {
      if (this->assigned_addresses[i]->status != expired) return false;
      return true;
    }
  }
  return true;
}

bool AddressPool::is_reserved(in_addr address, string mac) {
  this->reclaim_expired();
  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    if (this->assigned_addresses[i]->address.s_addr == address.s_addr) {
      if (this->assigned_addresses[i]->status == reserved &&
          this->assigned_addresses[i]->mac == mac)
        return true;
      return false;
    }
  }
  return false;
}

bool AddressPool::is_assigned(in_addr address, string mac) {
  this->reclaim_expired();
  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    if (this->assigned_addresses[i]->address.s_addr == address.s_addr) {
      if (this->assigned_addresses[i]->status == assigned &&
          this->assigned_addresses[i]->mac == mac)
        return true;
      return false;
    }
  }
  return false;
}

bool AddressPool::is_excluded(in_addr address) {
  if (address.s_addr == this->my_addr.s_addr) return true;
  for (unsigned i = 0; i < this->data->excluded.size(); i++) {
    if (this->data->excluded[i]->s_addr == address.s_addr) return true;
  }
  return false;
}

void AddressPool::print_pool() {
  time_t current_time = time(NULL);
  cout << "Current time " << current_time << endl;
  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    cout << inet_ntoa(this->assigned_addresses[i]->address) << ": "
         << this->assigned_addresses[i]->mac
         << " time: " << this->assigned_addresses[i]->time
         << " status: " << this->assigned_addresses[i]->status << endl;
  }
}

in_addr AddressPool::next_free_address(void) {
  int mark = 0;
  while (!this->is_free(this->current_addr) ||
         this->is_excluded(this->current_addr)) {
    this->current_addr.s_addr = addr_increment(this->current_addr.s_addr);
    if (!this->current_in_subnet()) {
      if (mark)  // Second cycle, free address cannont be found
        throw logic_error("No free addresses.");
      mark = 1;
      this->current_addr.s_addr =
          addr_increment(this->data->net_address.s_addr);
    }
  }
  return this->current_addr;
}

assign_entry *AddressPool::get_entry(in_addr address) {
  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    if (this->assigned_addresses[i]->address.s_addr == address.s_addr)
      return this->assigned_addresses[i];
  }
  return nullptr;
}

assign_entry *AddressPool::get_entry(string mac) {
  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    if (this->assigned_addresses[i]->mac == mac)
      return this->assigned_addresses[i];
  }
  return nullptr;
}

void AddressPool::release(in_addr address) {
  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    if (this->assigned_addresses[i]->address.s_addr == address.s_addr)
      assigned_addresses[i]->status = expired;
  }
}

time_t AddressPool::assign(in_addr address, string mac) {
  this->reclaim_expired();
  assign_entry *tmp = get_entry(address);
  time_t current_time = time(NULL);

  if (this->is_reserved(address, mac) || this->is_assigned(address, mac)) {
    if (tmp != nullptr) {
      tmp->time = current_time;
      tmp->status = assigned;
    }
  } else if (this->is_free(address)) {
    if (tmp == nullptr) {
      this->assigned_addresses.push_back(new assign_entry);
      tmp = this->assigned_addresses.back();
    }
    tmp->address = address;
    tmp->mac = mac;
    tmp->time = current_time;
    tmp->status = assigned;
  } else
    throw runtime_error("Requested address already in use.");

  return current_time;
}

void AddressPool::reserve(in_addr address, string mac) {
  this->reclaim_expired();
  assign_entry *tmp = get_entry(address);

  if (tmp == nullptr) {
    this->assigned_addresses.push_back(new assign_entry);
    tmp = this->assigned_addresses.back();
  }
  tmp->address = address;
  tmp->mac = mac;
  tmp->time = time(NULL);
  tmp->status = reserved;
}

void AddressPool::reclaim_expired(void) {
  time_t current_time = time(NULL);
  for (unsigned i = 0; i < this->assigned_addresses.size(); i++) {
    if (this->assigned_addresses[i]->time + DEFAULT_LEASE_TIME < current_time)
      this->assigned_addresses[i]->status = expired;
    if (this->assigned_addresses[i]->status == reserved &&
        this->assigned_addresses[i]->time + RESERVED_TIME < current_time)
      this->assigned_addresses[i]->status = expired;
  }
}

bool AddressPool::in_this_subnet(in_addr address) {
  return in_subnet(address, this->data->net_address, this->data->mask,
                   this->data->broadcast);
}

bool AddressPool::current_in_subnet(void) {
  return in_subnet(this->current_addr, this->data->net_address,
                   this->data->mask, this->data->broadcast);
}

uint32_t swap_endian(uint32_t input) {
  uint32_t b0, b1, b2, b3;
  b0 = (input & 0x000000ff) << 24u;
  b1 = (input & 0x0000ff00) << 8u;
  b2 = (input & 0x00ff0000) >> 8u;
  b3 = (input & 0xff000000) >> 24u;
  return b0 | b1 | b2 | b3;
}

in_addr_t addr_increment(in_addr_t address) {
  in_addr_t tmp = ntohl(address) + 1;
  return htonl(tmp);
}

bool in_subnet(in_addr address, in_addr net, in_addr mask, in_addr broadcast) {
  if (address.s_addr == net.s_addr ||
      address.s_addr == broadcast.s_addr)  // Address of subnet
    return false;
  return ((address.s_addr & mask.s_addr) == net.s_addr);
}

#endif  // !ADDRESS_POOL_H
