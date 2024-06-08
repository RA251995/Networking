#include "CppUTest/TestHarness.h"
#include <string.h>

extern "C"
{
    #include "ip_maths.h"
}

TEST_GROUP(IP_Math)
{
    char ip_address[PREFIX_LEN];

    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(IP_Math, Test_Cardinality)
{
    LONGS_EQUAL(4094, get_subnet_cardinality(20));
    LONGS_EQUAL(254, get_subnet_cardinality(24));
    LONGS_EQUAL(2, get_subnet_cardinality(30));
}

TEST(IP_Math, Test_IntegralEquivalent)
{
    strncpy(ip_address, "192.168.2.10", PREFIX_LEN);
    LONGS_EQUAL(3232236042, get_ip_integral_equivalent(ip_address));
    strncpy(ip_address, "10.1.23.10", PREFIX_LEN);
    LONGS_EQUAL(167843594, get_ip_integral_equivalent(ip_address));
}

TEST(IP_Math, Test_ABCDFormat)
{
    get_abcd_ip_format(2058138165, ip_address);
    STRCMP_EQUAL("122.172.178.53", ip_address);
    get_abcd_ip_format(3232236042, ip_address);
    STRCMP_EQUAL("192.168.2.10", ip_address);
    get_abcd_ip_format(167843594, ip_address);
    STRCMP_EQUAL("10.1.23.10", ip_address);
}

TEST(IP_Math, Test_BroadcastAddress)
{
    char out_address[PREFIX_LEN];

    strncpy(ip_address, "192.168.2.10", PREFIX_LEN);
    get_broadcast_address(ip_address, 24, out_address);
    STRCMP_EQUAL("192.168.2.255", out_address);

    strncpy(ip_address, "10.1.23.10", PREFIX_LEN);
    get_broadcast_address(ip_address, 20, out_address);
    STRCMP_EQUAL("10.1.31.255", out_address);
}

TEST(IP_Math, Test_NetworkId)
{
    char out_address[PREFIX_LEN];

    strncpy(ip_address, "192.168.2.10", PREFIX_LEN);
    get_network_id(ip_address, 24, out_address);
    STRCMP_EQUAL("192.168.2.0", out_address);

    strncpy(ip_address, "10.1.23.10", PREFIX_LEN);
    get_network_id(ip_address, 20, out_address);
    STRCMP_EQUAL("10.1.16.0", out_address);

    strncpy(ip_address, "10.1.23.10", PREFIX_LEN);
    get_network_id(ip_address, 30, out_address);
    STRCMP_EQUAL("10.1.23.8", out_address);
}

TEST(IP_Math, Test_Membership)
{
    char nw_id[PREFIX_LEN];

    strncpy(nw_id, "192.168.0.0", PREFIX_LEN);
    strncpy(ip_address, "192.168.0.13", PREFIX_LEN);
    LONGS_EQUAL(0, check_ip_subnet_membership(nw_id, 24, ip_address));

    strncpy(nw_id, "192.168.0.0", PREFIX_LEN);
    strncpy(ip_address, "192.168.0.13", PREFIX_LEN);
    LONGS_EQUAL(-1, check_ip_subnet_membership(nw_id, 30, ip_address));
}