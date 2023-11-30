#include "net/gcoap.h"
#include "ztimer.h"
#include "periph/cpuid.h"

#define COAP_SERVER_IPV6_ADDR_STR (COAP_SERVER_IPV6_ADDR)
#define COAP_SERVER_PORT "5683"
#define COAP_SERVER_PATH "/sensor_data"
#define MAX_RETRANSMISSIONS 3
#define RETRANSMISSION_TIMEOUT 5000 // ms

char node_id[5];
ipv6_addr_t server_addr;

void init_addresses(void)
{
    // Get the CPU ID
    uint8_t cpuid[CPUID_LEN];
    cpuid_get(cpuid);

    // Use the last four bytes of the CPU ID to create a unique identifier
    uint32_t unique_id = (cpuid[CPUID_LEN - 4] | (cpuid[CPUID_LEN - 2] << 7)) << 8 | cpuid[CPUID_LEN - 3];

    // Convert the unique identifier to a string and store it in node_id
    snprintf(node_id, sizeof(node_id), "%04lx", unique_id & 0xFFFF);


    // Set the server address
    ipv6_addr_from_str(&server_addr, COAP_SERVER_IPV6_ADDR_STR);
}

static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu, const sock_udp_ep_t *remote)
{
    (void)memo;
    (void)pdu;
    (void)remote;
}


void send_to_coap_server(int16_t avg_temp, uint16_t avg_pres, int avg_light)
{
    char msg[128];

    ssize_t pkt_len;

    sock_udp_ep_t remote;
    remote.family = AF_INET6;

    /* parse for interface */
    char *iface = ipv6_addr_split_iface(COAP_SERVER_IPV6_ADDR_STR);
    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            remote.netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            puts("gcoap_cli: interface not valid");
            return;
        }
        remote.netif = pid;
    }

    if ((remote.netif == SOCK_ADDR_ANY_NETIF) && ipv6_addr_is_link_local(&server_addr)) {
        puts("gcoap_cli: must specify interface for link local target");
        return;
    }

    // Create a CoAP POST request
    coap_pkt_t pkt;
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    size_t len;

    memcpy(&remote.addr.ipv6[0], &server_addr.u8[0], sizeof(server_addr.u8));

    /* parse port */
    remote.port = atoi(COAP_SERVER_PORT);
    if (remote.port == 0) {
        puts("gcoap_cli: unable to parse destination port");
        return;
    }

    // Format the sensor data into the payload
    sprintf(msg, "{\"id\":\"%s\",\"t\":\"%i.%u\",\"p\":\"%u\",\"l\":\"%d\"}", node_id, (avg_temp / 100), (avg_temp % 100), avg_pres, avg_light);
    payload_len = strlen(msg);
    printf("Payload: %s\n", msg);

    gcoap_req_init(&pkt, buf, CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_POST, COAP_SERVER_PATH);
    coap_opt_add_format(&pkt, COAP_FORMAT_TEXT);
    pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

    // Add payload to the request
    if (pkt.payload > payload_len) {
        memcpy(pkt.payload, msg, payload_len);
        pkt_len += payload_len;
    }

    // Retry sending the request if it fails
    int retries = 0;
    while (retries < MAX_RETRANSMISSIONS) {
        if (gcoap_req_send(buf, pkt_len, &remote, _resp_handler, NULL) <= 0) {
            printf("Failed to send request, retrying...\n");
            retries++;
            ztimer_sleep(ZTIMER_MSEC, RETRANSMISSION_TIMEOUT);
        } else {
            break;
        }
    }

    if (retries == MAX_RETRANSMISSIONS) {
        printf("Failed to send request after %d attempts\n", MAX_RETRANSMISSIONS);
    }
}
