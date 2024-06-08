#include <stdio.h>
#include <string.h>
#include "ip_maths.h"

int main()
{
    char ip_address[PREFIX_LEN];
    char ipadd_buffer[PREFIX_LEN];
    unsigned int int_ip;
    char mask;

    strncpy(ip_address, "192.168.2.10", PREFIX_LEN);
    memset(ipadd_buffer, 0, PREFIX_LEN);
    get_broadcast_address(ip_address, 24, ipadd_buffer);
    printf("Broadcast address = %s\n", ipadd_buffer);

    strncpy(ip_address, "192.168.0.10", PREFIX_LEN);
    int_ip = get_ip_integral_equivalent(ip_address);
    printf("Integer equivalent for %s is %u\n", ip_address, int_ip);

    int_ip = 2058138165; /* 122.172.178.53 */
    memset(ipadd_buffer, 0, PREFIX_LEN);
    get_abcd_ip_format(int_ip, ipadd_buffer);
    printf("IP in A.B.C.D format is = %s\n", ipadd_buffer);

    strncpy(ip_address, "192.168.2.10", PREFIX_LEN);
    memset(ipadd_buffer, 0, PREFIX_LEN);
    get_network_id(ip_address, 20, ipadd_buffer);
    printf("Network Id = %s\n", ipadd_buffer);

    mask = 24;
    printf("Subnet cardinality for Mask = %u is %u\n", mask, get_subnet_cardinality(mask));

    char network_id[] = "192.168.0.0";
    mask = 24;
    strncpy(ip_address, "192.168.0.13", PREFIX_LEN);
    int result = check_ip_subnet_membership(network_id, mask, ip_address);
    if (result == 0)
    {
        printf("Ip address = %s is a member of subnet %s/%u\n", ip_address, network_id, mask);
    }

    else
    {
        printf("Ip address = %s is a not a member of subnet %s/%u\n", ip_address, network_id, mask);
    }

    return 0;
}