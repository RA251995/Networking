#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ip_maths.h"

void get_broadcast_address(char *ip_addr, char mask, char *output_buffer)
{
    unsigned int ip_int = get_ip_integral_equivalent(ip_addr);
    ip_int |= (1U << (32U - (unsigned int)mask)) - 1U;
    get_abcd_ip_format(ip_int, output_buffer);
}

unsigned int get_ip_integral_equivalent(char *ip_addr)
{
    char tmp[PREFIX_LEN];
    strncpy(tmp, ip_addr, PREFIX_LEN);
    unsigned int ip_int = 0U;
    char *token = strtok(tmp, ".");
    unsigned int shift = 24U;
    while (token != NULL)
    {
        ip_int += ((unsigned int)strtoul(token, NULL, 10) << shift);
        token = strtok(NULL, ".");
        shift -= 8U;
    }

    return ip_int;
}

void get_abcd_ip_format(unsigned int ip_addr, char *output_buffer)
{
    unsigned char octet[4];

    for (unsigned char idx = 0; idx < 4; idx++)
    {
        octet[idx] = (unsigned char) ((ip_addr >> (idx * 8U)) & 0xFFU);
    }
    snprintf(output_buffer, PREFIX_LEN, "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);
}

void get_network_id(char *ip_addr, char mask, char *output_buffer)
{
    unsigned int ip_int = get_ip_integral_equivalent(ip_addr);
    ip_int &= ~((1U << (32U - (unsigned int)mask)) - 1U);
    get_abcd_ip_format(ip_int, output_buffer);
}

unsigned int get_subnet_cardinality(char mask)
{
    return (1U << (32U - (unsigned int)mask)) - 2U;
}

/* Return 0 if true , -1 if false*/
int check_ip_subnet_membership(char *network_id, char mask, char *check_ip)
{
    unsigned int nw_id = get_ip_integral_equivalent(network_id);
    unsigned int ip_int = get_ip_integral_equivalent(check_ip);
    ip_int &= ~((1U << (32U - (unsigned int)mask)) - 1U);

    if (ip_int == nw_id)
    {
        return 0;
    }

    return -1;
}
