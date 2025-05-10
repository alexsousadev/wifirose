#include "src/server_html/send_html.h"
#include <string.h>
#include "src/tcp_utils/tcp_utils.h"

// Declaração antecipada para o callback de envio
static err_t tcp_server_sent_html_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);

// Entregando o HTML para o cliente
static err_t tcp_server_recv_html(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    LWIP_UNUSED_ARG(arg);

    if (err != ERR_OK)
    {
        if (p != NULL)
        {
            send_confirmation_data_received(tpcb, p);
            pbuf_free(p);
        }
        if (err != ERR_CLSD)
        { 
            printf("Erro TCP recv %d em send_html, abortando conexão.\n", err);
            tcp_abort(tpcb); // Aborta a conexão
            return ERR_ABRT;
        }
    }

    // Se p for NULL, o peer fechou a conexão (ou err era ERR_CLSD)
    if (p == NULL)
    {
        printf("Conexão fechada pelo peer em send_html (p é NULL ou err era ERR_CLSD).\n");
        tcp_sent(tpcb, NULL);
        tcp_recv(tpcb, NULL);
        tcp_err(tpcb, NULL);
        tcp_poll(tpcb, NULL, 0);
        close_connection(tpcb);
        return ERR_OK;
    }

    // Dados recebidos com sucesso
    send_confirmation_data_received(tpcb, p);
    pbuf_free(p);

    // Monta a resposta HTML com o JS para mostrar o estado do botão e temperatura
    char html_response[2048]; 
    memset(html_response, 0, sizeof(html_response));
    size_t html_len = snprintf(html_response, sizeof(html_response),
                               "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Connection: keep-alive\r\n"
                               "\r\n"
                               "<!DOCTYPE html>\n"
                               "<html>\n"
                               "<head>\n"
                               "    <title>Status do Joystick</title>\n"
                               "    <meta charset=\"utf-8\">\n"
                               "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
                               "</head>\n"
                               "<body>\n"
                               "    <h1><b>Estado do Joystick</b></h1>\n"
                               "    <div id=\"joystick-status\"></div>\n"
                               "    <div class=\"compass-section\">\n"
                               "        <h2>Direção</h2>\n"
                               "        <div id=\"direction\" class=\"direction\">-</div>\n"
                               "    </div>\n"
                               "</body>\n"
                               "<script>\n"
                               "function fetchJoystickState() {\n"
                               "    fetch(`http://${window.location.hostname}:%d`)\n"
                               "        .then(response => {\n"
                               "            if (!response.ok) {\n"
                               "                throw new Error(`HTTP error! status: ${response.status}`);\n"
                               "            }\n"
                               "            return response.json();\n"
                               "        })\n"
                               "        .then(data => {\n"
                               "            console.log('Dados do Joystick:', data);\n"
                               "            const joystickDiv = document.getElementById('joystick-status');\n"
                               "            const directionDiv = document.getElementById('direction');\n"
                               "            \n"
                               "            joystickDiv.innerHTML = `\n"
                               "                <div class=\"coordinates\">\n"
                               "                    <div><span class='value-label'>Eixo X:</span> ${data.x}</div>\n"
                               "                    <div><span class='value-label'>Eixo Y:</span> ${data.y}</div>\n"
                               "                </div>\n"
                               "            `;\n"
                               "            \n"
                               "            directionDiv.textContent = data.direction || '-';\n"
                               "        })\n"
                               "        .catch(error => {\n"
                               "            console.error('Erro ao buscar estado do joystick:', error);\n"
                               "            const joystickDiv = document.getElementById('joystick-status');\n"
                               "            joystickDiv.textContent = 'Erro ao carregar dados do joystick.';\n"
                               "            document.getElementById('direction').textContent = '-';\n"
                               "        });\n"
                               "}\n"
                               "\n"
                               "fetchJoystickState();\n"
                               "setInterval(fetchJoystickState, 1000);\n"
                               "</script>\n"
                               "</html>\n",
                               PORT_API);

    if (html_len >= sizeof(html_response))
    {
        printf("Resposta HTML truncada, necessário %u temos %u\n", (unsigned int)html_len, (unsigned int)sizeof(html_response));
        tcp_abort(tpcb);
        return ERR_ABRT;
    }

    // Prepara e envia os dados
    if (!send_tcp_data(tpcb, html_response, html_len, TCP_WRITE_FLAG_COPY))
    {
        printf("Falha ao enviar dados HTML usando send_tcp_data. Abortando.\n");
        tcp_abort(tpcb); // Aborta se send_tcp_data falhar
        return ERR_ABRT;
    }

    // Define o callback de envio para fechar a conexão assim que os dados forem confirmados
    tcp_sent(tpcb, tcp_server_sent_html_callback);
    return ERR_OK;
}

// Callback para lidar com dados enviados e fechar conexão para o servidor HTML
static err_t tcp_server_sent_html_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(len);

    printf("Dados HTML enviados com sucesso.\n");
    return ERR_OK;
}

// Função de callback chamada quando uma nova conexão é aceita
static err_t tcp_server_accept_html(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    if (err != ERR_OK)
    {
        return err;
    }
    tcp_recv(newpcb, tcp_server_recv_html); // Define a função de recebimento
    return ERR_OK;
}

// Cria o servidor para servir o HTML
void create_server_html()
{
    // Cria o servidor TCP para servir o HTML
    struct tcp_pcb *html_server = create_connection_tcp();
    if (!html_server)
    {
        printf("Falha ao criar servidor TCP\n");
        return;
    }

    // Configurando a conexão TCP
    html_server = prepare_connection(html_server, PORT_SERVER);
    if (!html_server)
    {
        printf("Falha ao configurar servidor TCP (PCB)\n");
        return;
    }

    // Configurando o timer
    setup_periodic_timer(html_server);
    tcp_accept(html_server, tcp_server_accept_html);
}


