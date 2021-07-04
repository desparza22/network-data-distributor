#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <ares.h>
#include <stdlib.h>



struct Address {
  char* IP;
  int ip_len;
  char* port;
  int port_len;
};

struct AddressList {
  struct Address** addresses;
  int num_addresses;

  int address_capacity;
};

void init_AddressList(struct AddressList** set_to_AddressList,
		      int initial_num_addresses);

void packet_from_AddressList(struct AddressList* addresses, void** buffer,
			     int* set_to_buffer_len);

void AddressList_from_packet(void* packet_void, int packet_len,
			     struct AddressList** set_to_addresses);

void add_Address_to_AddressList(struct AddressList* addresses,
				struct Address* address);

int calculate_packet_size(struct AddressList* addresses);

void fill_packet_from_AddressList(char* packet, struct AddressList* addresses);

void packet_insert_Address_at(char* packet, struct Address* address,
			      int* fill_idx_ptr);



#endif
