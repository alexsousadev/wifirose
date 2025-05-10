#include <stdio.h>
#include "lwip/tcp.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "src/tcp_utils/tcp_utils.h"

static struct repeating_timer timer;     
static struct tcp_pcb *server_pcb = NULL;

// Cria uma nova conexão TCP
struct tcp_pcb *create_connection_tcp(void)
{
    struct tcp_pcb *connection = tcp_new();
    if (connection == NULL)
    {
        printf("Falha ao criar servidor TCP (PCB)\n");
        return NULL;
    }
    return connection;
}

// Associa uma porta à conexão TCP (tcp_bind)
int set_port_to_connection(struct tcp_pcb *connection, uint16_t port)
{
    err_t err = tcp_bind(connection, IP_ADDR_ANY, port);
    if (err != ERR_OK)
    {
        printf("Falha ao associar servidor TCP à porta %d: %d\n", port, err);
        tcp_close(connection);
        return 0;
    }
    return 1;
}

// Configura o PCB para escutar conexões de entrada (tcp_listen)
struct tcp_pcb *make_connection_listen(struct tcp_pcb *connection)
{
    connection = tcp_listen_with_backlog(connection, 1);
    if (connection == NULL)
    {
        printf("Falha ao configurar servidor para escuta (tcp_listen)\n");
        return NULL;
    }
    return connection;
}

// Confirma que os dados foram recebidos com sucesso
void send_confirmation_data_received(struct tcp_pcb *connection, struct pbuf *packet_buffer)
{
    tcp_recved(connection, packet_buffer->tot_len);
}

// Enfileira dados para envio
bool enqueue_data_to_send(struct tcp_pcb *pcb, const void *dataptr, u16_t len, u8_t apiflags)
{
    err_t write_err = tcp_write(pcb, dataptr, len, apiflags);
    if (write_err != ERR_OK)
    {
        printf("Erro ao enfileirar dados (tcp_write: %d)\n", write_err);
        return false;
    }
    return true;
}

// Envia os dados enfileirados
bool send_data(struct tcp_pcb *pcb)
{
    err_t output_err = tcp_output(pcb);
    if (output_err != ERR_OK)
    {
        printf("Erro ao enviar dados (tcp_output: %d)\n", output_err);
        return false;
    }
    return true;
}

// Fecha a conexão TCP
bool close_connection(struct tcp_pcb *connection)
{
    if (connection == NULL)
    {
        printf("Conexão já é NULL\n");
        return true; // Já está "fechada"
    }

    err_t close_err = tcp_close(connection);
    if (close_err != ERR_OK)
    {
        printf("Erro ao fechar conexão (tcp_close: %d), abortando\n", close_err);
        tcp_abort(connection); // Força o encerramento
        return false;
    }
    return true;
}

// Prepara a conexão TCP (associa porta e ativa modo de escuta)
struct tcp_pcb *prepare_connection(struct tcp_pcb *connection, int port)
{
    int connected = set_port_to_connection(connection, port);
    if (!connected)
    {
        printf("Falha ao associar porta %d\n", port);
        return NULL;
    }
    connection = make_connection_listen(connection);
    if (!connection)
    {
        printf("Falha ao configurar servidor para escuta\n");
        return NULL;
    }
    return connection;
}

// Enfileira e envia dados pela conexão TCP
bool send_tcp_data(struct tcp_pcb *tpcb, const void *data, u16_t len, u8_t apiflags)
{
    if (tpcb == NULL)
    {
        printf("Erro: PCB é NULL\n");
        return false;
    }

    if (tcp_sndbuf(tpcb) < len)
    {
        printf("Erro: Buffer de envio TCP insuficiente (%d < %d)\n", tcp_sndbuf(tpcb), len);
        return false; // Não aborta, deixa o chamador decidir
    }

    if (!enqueue_data_to_send(tpcb, data, len, apiflags))
    {
        printf("Falha ao enfileirar dados para envio\n");
        return false;
    }

    if (!send_data(tpcb))
    {
        printf("Falha ao enviar dados\n");
        return false;
    }

    return true;
}

// Callback do timer periódico para anunciar IP e porta
static bool timer_callback(struct repeating_timer *t)
{
    if (netif_default == NULL || !netif_is_up(netif_default))
    {
        printf("Interface de rede não configurada ou inativa\n");
        return true; // Continua o timer
    }
    if (server_pcb == NULL)
    {
        printf("Servidor TCP não configurado\n");
        return true; // Continua o timer
    }
    printf("Servidor executando em: http://%s:%d\n",
           ipaddr_ntoa(netif_ip4_addr(netif_default)), PORT_SERVER);
    return true; // Continua o timer
}

// Configura o timer periódico
void setup_periodic_timer(struct tcp_pcb *tpcb)
{
    server_pcb = tpcb; // Armazena o PCB do servidor
    if (!add_repeating_timer_ms(10000, timer_callback, NULL, &timer))
    {
        printf("Erro ao configurar o timer periódico\n");
    }
    else
    {
        printf("Timer periódico configurado para 10 segundos\n");
    }
}
