#ifndef SEND_JOYSTICK_H
#define SEND_JOYSTICK_H

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h" 

static err_t
tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static const char* direction_to_string(JoystickDirection dir);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
void create_server_button();

#endif // SEND_JOYSTICK_H