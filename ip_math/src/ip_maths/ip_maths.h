#ifndef IP_MATHS_H
#define IP_MATHS_H

#define PREFIX_LEN (16U)

void get_broadcast_address(char *ip_addr, char mask, char *output_buffer);
unsigned int get_ip_integral_equivalent(char *ip_addr);
void get_abcd_ip_format(unsigned int ip_addr, char *output_buffer);
void get_network_id(char *ip_addr, char mask, char *output_buffer);
unsigned int get_subnet_cardinality(char mask);
int check_ip_subnet_membership(char *network_id, char mask, char *check_ip);

#endif /* IP_MATHS_H */
