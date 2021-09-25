#ifndef NET_HEADERS_H
#define NET_HEADERS_H

#include <inttypes.h>

#define MAX_IP_PACKET  65535
#define ETH_HDR_SZ 1518

struct ip_header {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t header_len : 4;
    uint8_t version : 4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint8_t version : 4;
    uint8_t header_len : 4;
#else
#error "endianess error"
#endif
    uint8_t dscp;
#define NON_ECT 0b00000011
#define ECT0 0b00000010
#define ECT1 0b00000001
#define CONG_ENC 0b00000011
    uint16_t total_len;
    uint16_t id;
    uint16_t frag_offset;
#define DONT_FRAG 0b01000000
#define MORE_FRAG 0b00100000
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_adr;
    uint32_t dst_adr;
};

struct icmp_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t rest;
};

#define ETHERNET_ADDR_LEN 6
struct ethernet_header {
    uint8_t ethernet_dstaddr[ETHERNET_ADDR_LEN]; 
    uint8_t ethernet_srcaddr[ETHERNET_ADDR_LEN]; 
    uint16_t ethernet_type;
}__attribute__((__packed__));

#endif /* __HEADERS_H__ */
