#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include "pico/stdlib.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"

#define PORT_SERVER 80 
#define PORT_API 8080

// Declarações de funções
struct tcp_pcb *create_connection_tcp(void);
int set_port_to_connection(struct tcp_pcb *connection, uint16_t port);
struct tcp_pcb *make_connection_listen(struct tcp_pcb *connection);
void send_confirmation_data_received(struct tcp_pcb *connection, struct pbuf *packet_buffer);
bool enqueue_data_to_send(struct tcp_pcb *pcb, const void *dataptr, u16_t len, u8_t apiflags);
struct tcp_pcb *prepare_connection(struct tcp_pcb *connection, int port);
bool send_data(struct tcp_pcb *pcb);
bool close_connection(struct tcp_pcb *connection);
struct tcp_pcb *prepare_connection(struct tcp_pcb *connection, int port);
bool send_tcp_data(struct tcp_pcb *tpcb, const void *data, u16_t len, u8_t apiflags);
static bool timer_callback(struct repeating_timer *t);
void setup_periodic_timer(struct tcp_pcb *tpcb);

#endif // TCP_UTILS_H