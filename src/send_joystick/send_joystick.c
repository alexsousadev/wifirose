#include "src/joystick/joystick.h"
#include "src/server_html/send_html.h"
#include "src/tcp_utils/tcp_utils.h"

// Função auxiliar para converter enum JoystickDirection em string
static const char* direction_to_string(JoystickDirection dir) {
    switch (dir) {
        case CENTRO: return "CENTRO";
        case NORTE: return "NORTE";
        case NORDESTE: return "NORDESTE";
        case LESTE: return "LESTE";
        case SUDESTE: return "SUDESTE";
        case SUL: return "SUL";
        case SUDOESTE: return "SUDOESTE";
        case OESTE: return "OESTE";
        case NOROESTE: return "NOROESTE";
        default: return "DESCONHECIDO";
    }
}

// Callback para lidar com os dados enviados e fechar a conexão
static err_t
tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(len);

    // Clear callbacks
    tcp_sent(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);

    // Fecha a conexão TCP
    bool connection_closed = close_connection(tpcb);
    if (!connection_closed)
    {
        printf("Erro ao fechar a conexão TCP, abortando.\n");
        tcp_abort(tpcb);
        return ERR_ABRT;
    }

    return ERR_OK;
}


// Callback para lidar com os dados recebidos
static err_t
tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    LWIP_UNUSED_ARG(arg);

    // Lida com o fechamento da conexão pelo cliente ou erro
    if (!p)
    {
        printf("Conexão fechada pelo cliente ou erro.\n");
        tcp_sent(tpcb, NULL);
        tcp_recv(tpcb, NULL);
        tcp_err(tpcb, NULL);
        tcp_poll(tpcb, NULL, 0);
        close_connection(tpcb); 
        return ERR_OK;
    }

    // Verifica se houve erro na recepção
    if (err != ERR_OK)
    {
        printf("Erro na recepção TCP: %d\n", err);
        if (p != NULL)
        {
            pbuf_free(p);
        }
        tcp_abort(tpcb);
        return ERR_ABRT;
    }

    send_confirmation_data_received(tpcb, p);
    pbuf_free(p);
    read_joystick();

    // Constrói a resposta HTTP
    char response[256];
    const char* direction_str = direction_to_string(joystick_data.direction);

    size_t response_len = snprintf(response, sizeof(response),
                                   "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Access-Control-Allow-Origin: *\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "{\"x\": %d, \"y\": %d, \"direction\": \"%s\"}",
                                   joystick_data.position_x, joystick_data.position_y, direction_str);

    // Envia a resposta
    if (!send_tcp_data(tpcb, response, response_len, TCP_WRITE_FLAG_COPY))
    {
        printf("Failed to send TCP data, aborting.\n");
        close_connection(tpcb);
        tcp_abort(tpcb);
        return ERR_ABRT;
    }

    // Define o callback para quando os dados são enviados
    tcp_sent(tpcb, tcp_server_sent);

    // Garante que a saída seja esvaziada antes de fechar
    tcp_output(tpcb);

    return ERR_OK;
}

// Callback para aceitar novas conexões
static err_t
tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);

    if (err != ERR_OK || newpcb == NULL)
    {
        printf("Erro ao aceitar a conexão: %d\n", err);
        return ERR_VAL;
    }

    tcp_setprio(newpcb, TCP_PRIO_NORMAL);
    tcp_recv(newpcb, tcp_server_recv);
    tcp_err(newpcb, NULL);
    return ERR_OK;
}

// Cria o servidor TCP
void create_server_button()
{

    printf("Servidor executando em: http://%s:%d\n",
           ipaddr_ntoa(netif_ip4_addr(netif_default)), PORT_API);

    struct tcp_pcb *server_pcb = create_connection_tcp();
    if (!server_pcb)
    {
        printf("Erro ao criar o servidor TCP (PCB)\n");
        return;
    }

    server_pcb = prepare_connection(server_pcb, PORT_API);
    if (!server_pcb)
    {
        printf("Erro ao configurar o servidor TCP (PCB)\n");
        return;
    }

    tcp_accept(server_pcb, tcp_server_accept);
}



