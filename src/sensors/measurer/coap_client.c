#include "net/gcoap.h"
#include "ztimer.h"
#include "periph/cpuid.h"

#define COAP_SERVER_IPV6_ADDR_STR (COAP_SERVER_IPV6_ADDR)
#define COAP_SERVER_PORT 5683
#define COAP_SERVER_PATH "/sensor_data"
#define MAX_RETRANSMISSIONS 3
#define RETRANSMISSION_TIMEOUT 5000 // ms

char node_id[9];
ipv6_addr_t server_addr;

void init_addresses(void)
{
    // Get the CPU ID
    uint8_t cpuid[CPUID_LEN];
    cpuid_get(cpuid);

    // Use the last four bytes of the CPU ID to create a unique identifier
    uint32_t unique_id = (cpuid[CPUID_LEN - 4] | (cpuid[CPUID_LEN - 2] << 7)) << 8 | cpuid[CPUID_LEN - 3];

    // Convert the unique identifier to a string and store it in node_id
    snprintf(node_id, sizeof(node_id), "%08lx", unique_id);


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
    char payload[128];
    ssize_t payload_len;

    // Format the sensor data into the payload
    payload_len = snprintf(payload, sizeof(payload), "{\"node_id\": \"%s\", \"temperature\": \"%i.%u\", \"pressure\": \"%u\", \"light\": \"%d\"}",
                       node_id, (avg_temp / 100), (avg_temp % 100), avg_pres, avg_light);

    // Create a CoAP POST request
    coap_pkt_t pkt;
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    size_t len;

    len = gcoap_request(&pkt, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_PUT, COAP_SERVER_PATH);
    if (len == 0) {
        printf("Failed to initialize request\n");
        return;
    }

    // Add payload to the request
    if (payload_len > 0) {
        memcpy(pkt.payload, payload, payload_len);
        len += payload_len;
    }

    // Send the request
    sock_udp_ep_t remote = { .family = AF_INET6, .port = COAP_SERVER_PORT };
    memcpy(remote.addr.ipv6[0], &server_addr.u8[0], sizeof(addr.u8[0]));

    // Retry sending the request if it fails
    int retries = 0;
    while (retries < MAX_RETRANSMISSIONS) {
        if (gcoap_req_send(buf, len, &remote, _resp_handler, NULL) <= 0) {
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
