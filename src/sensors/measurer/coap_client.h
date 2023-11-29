#ifndef COAP_CLIENT_H
#define COAP_CLIENT_H

void init_addresses(void);
void send_to_coap_server(int16_t avg_temp, uint16_t avg_pres, int avg_light);

#endif /* COAP_CLIENT_H */
