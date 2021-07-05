#include "data_structures.hpp"

void init_Address(struct Address** set_to_Address) {
  *set_to_Address = (struct Address*) malloc(sizeof(struct Address));
}

void init_AddressList(struct AddressList** set_to_AddressList,
		      int address_capacity) {
  *set_to_AddressList = (struct AddressList*) malloc(sizeof(struct AddressList));

  (*set_to_AddressList)->addresses =
    (struct Address**) malloc(address_capacity * sizeof(struct Address*));
  (*set_to_AddressList)->address_capacity = address_capacity;
  (*set_to_AddressList)->num_addresses = 0;
}

void packet_from_AddressList(struct AddressList* addresses, void** buffer,
			     int* set_to_buffer_len) {

  int packet_size = calculate_packet_size(addresses);
  *set_to_buffer_len = packet_size;
  char* packet = (char*) malloc(packet_size * sizeof(char));

  fill_packet_from_AddressList(packet, addresses);
  *buffer = (void*)packet;
}


void AddressList_from_packet(void* packet_void, struct AddressList** set_to_addresses) {
  char* packet = (char*) packet_void;
  int read_idx = 0;
  int num_addresses = (int) packet[read_idx++];

  int initial_capacity = std::max(5, num_addresses + num_addresses/2);
  init_AddressList(set_to_addresses, initial_capacity);

  for(int i = 0; i < num_addresses; i++) {
    struct Address* address;
    packet_read_Address_from(packet, &address, &read_idx);
    add_Address_to_AddressList(*set_to_addresses, address);
  }
}

void add_Address_to_AddressList(struct AddressList* addresses,
				struct Address* address) {
  if(addresses->address_capacity <= addresses->num_addresses) {
    int new_size = addresses->num_addresses + addresses->num_addresses/2 + 2;
    addresses->addresses = (struct Address**) realloc(addresses->addresses, new_size);
  }

  addresses->addresses[(addresses->num_addresses)++] = address;
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

  packet[fill_idx++] = (char) address->ip_len;
  for(int i = 0; i < address->ip_len; i++) {
    packet[fill_idx++] = address->IP[i];
  }

  packet[fill_idx++] = (char) address->port_len;
  for(int i = 0; i < address->port_len; i++) {
    packet[fill_idx++] = address->port[i];
  }

  *fill_idx_ptr = fill_idx;
}

void packet_read_Address_from(char* packet, struct Address** set_to_Address,
			      int* read_from_ptr) {
  int read_from = *read_from_ptr;

  int ip_len = (int) packet[read_from++];
  char* ip = (char*) malloc(ip_len * sizeof(char));
  for(int i = 0; i < ip_len; i++) {
    ip[i] = packet[read_from++];
  }

  int port_len = (int) packet[read_from++];
  char* port = (char*) malloc(port_len * sizeof(char));
  for(int i = 0; i < port_len; i++) {
    port[i] = packet[read_from++];
  }

  init_Address(set_to_Address);
  (*set_to_Address)->ip_len = ip_len;
  (*set_to_Address)->IP = ip;
  (*set_to_Address)->port_len = port_len;
  (*set_to_Address)->port = port;
  
  *read_from_ptr = read_from;
}


int main(int argc, char* argv[]) {
  struct AddressList* address_list;
  init_AddressList(&address_list, 20);

  for(int i = 0; i < 20; i++) {
    struct Address* address;
    init_Address(&address);
    address->ip_len = 7 + i;
    address->IP = (char*) malloc((7+i) * sizeof(char));
    for(int j = 0; j < 6 + i; j++) {
      address->IP[j] = rand() % 26 + 'a';
    }
    address->IP[6+i] = '\0';

    address->port_len = 5 + i;
    address->port = (char*) malloc((5+i) * sizeof(char));
    for(int j = 0; j < 4 + i; j++) {
      address->port[j] = rand() % 26 + 'a';
    }
    address->port[4+i] = '\0';

    add_Address_to_AddressList(address_list, address);
    printf("Added address with ip %s and port %s\n", address->IP, address->port);
  }

  printf("\n\n");

  void* hold_packet;
  int packet_len;
  packet_from_AddressList(address_list, &hold_packet, &packet_len);

  struct AddressList* address_list2;
  AddressList_from_packet(hold_packet, &address_list2);

  for(int i = 0; i < address_list2->num_addresses; i++) {
    struct Address* address = address_list2->addresses[i];
    printf("Reading address with ip %s and port %s\n", address->IP, address->port);
  }
  printf("\n\n");

  void* hold_packet2;
  int packet_len2;
  packet_from_AddressList(address_list2, &hold_packet2, &packet_len2);
  
  struct AddressList* address_list3;
  AddressList_from_packet(hold_packet2, &address_list3);

  for(int i = 0; i < address_list3->num_addresses; i++) {
    struct Address* address = address_list3->addresses[i];
    printf("Reading address with ip %s and port %s\n", address->IP, address->port);
  }
  
  return 0;
}
