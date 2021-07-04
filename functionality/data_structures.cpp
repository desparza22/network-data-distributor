#include "data_structures.hpp"

void init_AddressList(struct AddressList** set_to_AddressList,
		      int address_capacity) {
  *set_to_AddressList = malloc(sizeof(struct AddressList));

  (*set_to_AddressList)->addresses =
    malloc(address_capacity * sizeof(struct Address*));
  (*set_to_AddressList)->address_capacity = address_capacity;
  (*set_to_AddressList)->num_addresses = 0;
}

void packet_from_AddressList(struct AddressList* addresses, void** buffer,
			     int* set_to_buffer_len) {

  int packet_size = calculate_packet_size(addresses);
  *set_to_buffer_len = packet_size;
  char* packet = malloc(packet_size * sizeof(char));

  fill_packet_from_AddressList(packet, addresses);
  *buffer = (void*)packet;
}


void AddressList_from_packet(void* packet_void, int packet_len,
			     struct AddressList** set_to_addresses) {
  char* packet = (char*) packet_void;
  int read_idx = 0;
  int num_addresses = (int) packet[read_idx++];

  int initial_capacity = std::min(5, num_addresses + num_addresses/2);
  init_AddressList(set_to_addresses, initial_capacity);

  for(int i = 0; i < num_addresses; i++) {
    struct Address* address;
    packet_read_Address_from(packet, &address, &read_idx);
  }
}

void add_Address_to_AddressList(struct AddressList* addresses,
				struct Address* address) {

}

int calculate_packet_size(struct AddressList* addresses) {
  //ips and ports
  int ips_and_ports_size = 0;
  for(int i = 0; i < addresses->num_addresses; i++) {
    struct Address* address = addresses->addresses[i];
    ips_and_ports_size += address->ip_len + address->port_len;
  }

  //headers
  int header_size = 1;
  int headers_per_address_list = 1;
  int headers_per_address = 2;
  
  int address_headers = headers_per_address * addresses->num_addresses;
  int total_headers = headers_per_address_list + address_headers;

  return total_headers * header_size + ips_and_ports_size;
}

void fill_packet_from_AddressList(char* packet,
				  struct AddressList* addresses) {
  int fill_idx = 0;
  packet[fill_idx++] = (char) addresses->num_addresses;

  for(int i = 0; i < addresses->num_addresses; i++) {
    struct Address* address = addresses->addresses[i];
    packet_insert_Address_at(packet, address, &fill_idx);
  }
}

void packet_insert_Address_at(char* packet, struct Address* address,
			      int* fill_idx_ptr) {
  int fill_idx = *fill_idx_ptr;

  packet[fill_idx++] = address->ip_len;
  for(int i = 0; i < address->ip_len; i++) {
    packet[fill_idx++] = address->ip[i];
  }

  packet[fill_idx++] = address->port_len;
  for(int i = 0; i < address->port_len; i++) {
    packet[fill_idx++] = address->port[i];
  }

  *fill_idx_ptr = fill_idx;
}
