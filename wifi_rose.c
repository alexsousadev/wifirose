
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "src/joystick/joystick.h"
#include "src/wifi_connect/wifi_connect.h"
#include "src/server_html/send_html.h"
#include "src/send_joystick/send_joystick.h"

int main()
{
    stdio_init_all();
    init_joystick();
    init_wifi();
    create_server_button();
    create_server_html();

    while (true)
    {
        sleep_ms(100);
    }
    return 0;
}