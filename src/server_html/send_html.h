#ifndef SEND_HTML_H
#define SEND_HTML_H

#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"

static err_t tcp_server_recv_html(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_server_sent_html_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t tcp_server_accept_html(void *arg, struct tcp_pcb *newpcb, err_t err);
void create_server_html();

#endif // SEND_HTML_H