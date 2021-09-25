#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>

#include <pcap/pcap.h>

#include "net_headers.h"


#define array_sz(a) (sizeof(a)/sizeof(*a))

typedef enum {
    UINT8T=8, UINT16T=16, UINT32T=32
} Type;

typedef union {
    uint8_t Uint8t;
    uint16_t Uint16t;
    uint32_t Uint32t;
} Value;



struct data_type {
    Type type;
    Value value;
};


char *to_binary(struct data_type dt) 
{
    int32_t i = 0;
    static char ret[sizeof(UINT32T)*8];

    ret[0] = '0';
    ret[1] = 'b';

    if (dt.type == UINT8T) {
        for (i = UINT8T-1; i >= 0; i--) {
            ret[1+UINT8T-i] = (dt.value.Uint8t & (1u << i))  ? '1' : '0';
        }
    } else if (dt.type == UINT16T) {
        for (i = UINT16T-1; i >= 0; i--) {
            ret[1+UINT16T-i] = (dt.value.Uint16t & (1u << i)) ? '1' : '0';
        }
    } else {
        for (i = UINT32T-1; i >= 0; i--) {
            ret[1+UINT32T-i] = (dt.value.Uint32t & (1u << i)) ? '1' : '0';
        }
    }

    ret[dt.type+2] = 0;
    return ret;
}

struct data_packet {
    const uint8_t *data;
    struct ip_header* iphdr;
    uint8_t type;
};

void print_ipheader(const struct data_packet *data)
{
    printf("-----------------------IP HEADER-------------------------\n");
    printf("%-42s %"PRIu8"\n","-IP Version:", data->iphdr->version);
    printf("%-42s %"PRIu8"\n", "-IP Header Lenght:",  data->iphdr->header_len);
    uint8_t tmp = data->iphdr->dscp;

    char *strtmp = to_binary((struct data_type){.type=UINT8T, .value=tmp});
    printf("%-42s %s\n","-IP Differentiated Services Code Point:", strtmp);

    printf("%-42s %s\n", 
            "    -Explicit Congestion notification: ",
            !(tmp&NON_ECT)?"None ECN-capable transport"
            :tmp&ECT0?"ECN-Capable transport ECT0"
            :tmp&ECT1?"ECN-Capable transport ECT1"
            :tmp&CONG_ENC?"Congestion Encountered":"?");

    printf("%-42s %"PRIu16"\n", "-IP Total Length:", data->iphdr->total_len);

    strtmp = to_binary((struct data_type){.type=UINT16T, 
            .value=data->iphdr->id});
    printf("%-42s %s\n", "-IP Identification:", strtmp);

    tmp = data->iphdr->frag_offset;
    strtmp = to_binary((struct data_type){.type=UINT16T, 
            .value=data->iphdr->frag_offset});
    printf("%-42s %s %s\n", 
            "-IP Fragmentation Offset:",
            strtmp,
            tmp&DONT_FRAG?"(Don't fragment)"
            : tmp&MORE_FRAG?"(More fragments)":"");


    printf("%-42s %"PRIu8"\n", "-IP Time To Live: ", data->iphdr->ttl);
    printf("%-42s %s\n", "-IP Protocol: ",
            getprotobynumber(data->iphdr->protocol)->p_name);

    strtmp = to_binary((struct data_type){.type=UINT16T, 
            .value=data->iphdr->checksum});
    printf("%-42s %s\n", "-IP checksum:", strtmp);

    printf("%-42s %s\n", "-IP Source Address: ",  
            inet_ntoa(*(struct in_addr*)&data->iphdr->src_adr));
    printf("%-42s %s\n", "-IP Destination Address: ", 
            inet_ntoa(*(struct in_addr*)&data->iphdr->dst_adr));
}

void print_icmpheader(const struct data_packet *data) 
{
    puts("-----------------------ICMP HEADER-------------------------");
}

void list_devices(pcap_if_t **ds) 
{
    puts("Devices available:");
    printf("{");
    for (pcap_if_t *tmp = *ds; tmp != NULL; tmp = tmp->next) {
        printf("%s%s", tmp->name, tmp->next ? ", ":"}\n");

        
        /*
        struct sockaddr * saddr = tmp->addresses->addr;
        for (pcap_addr_t *a = tmp->addresses; a != NULL; a = a->next) {
            printf("SA_FAMILY = %d, AF_INET = %d, AF_INET6 = %d\n", 
                    saddr->sa_family, AF_INET, AF_INET6);
            if (saddr->sa_family == AF_INET) {
                puts("AF_INTET");
                printf("%s, ", saddr->sa_data); 
            } else if (saddr->sa_family == AF_INET6) {
                puts("AF_INTET6");
                printf("%s, ", saddr->sa_data); 
            }
        }
        */
    } 
}


void print_etherheader(struct data_packet *dp) 
{
}


int main(int argc, char **argv)
{
    const uint8_t *data = NULL;
    char error_buf[PCAP_ERRBUF_SIZE];

    pcap_if_t *device_lst = 0;
    if(pcap_findalldevs(&device_lst, error_buf)) {
        perror("Pcap find all");
        exit(EXIT_FAILURE);
    }

    list_devices(&device_lst);
    printf("Capturing packets from device: %s\n", device_lst->name);

    struct data_packet datapacket = {0};

    pcap_t *dscr = pcap_open_live(device_lst->name, 
            MAX_IP_PACKET + ETH_HDR_SZ, 0, 1000, error_buf);
    if (dscr == NULL) {
        fprintf(stderr, "Pcap_open_live %s\n", error_buf);
        exit(EXIT_FAILURE);
        pcap_freealldevs(device_lst);
    }

    struct pcap_pkthdr hdr = {0};
    
    data = pcap_next(dscr, &hdr);
    if (data == NULL) {
        fprintf(stderr, "Unable to retrieve packet\n");
        exit(EXIT_FAILURE);
    }

    printf("\n----Recieved a packet. captured length: %d length: %d at dtg %s\n",
            hdr.caplen,
            hdr.len,
            ctime((const time_t*)&hdr.ts.tv_sec));

    datapacket.data = data;
    datapacket.iphdr = (struct ip_header *)(data + sizeof(struct ethernet_header));

    print_etherheader(&datapacket);
    print_ipheader(&datapacket);

    switch (datapacket.iphdr->protocol) {
        case IPPROTO_ICMP :
            print_icmpheader(&datapacket);
            break;
        case IPPROTO_TCP :
            break;
        default:
            printf("Not implemented: %s\n",
                    getprotobynumber(datapacket.iphdr->protocol)->p_name);
            break;
    }
    pcap_freealldevs(device_lst);
    return 0;
}
