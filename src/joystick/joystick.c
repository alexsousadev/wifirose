#include "src/joystick/joystick.h"
#include <math.h>

// Define os pinos do joystick
int joystick_pins[2] = {JOYSTICK_X_PIN, JOYSTICK_Y_PIN};

// Define a estrutura com os dados do joystick
joystickData joystick_data = {0, 0, CENTRO};

// Inicializa o joystick
void init_joystick(void)
{
    adc_init();

    for (int i = 0; i < 2; i++)
    {
        adc_gpio_init(joystick_pins[i]);
    }
}

// Calcula a direção com base nos valores X e Y
JoystickDirection calculate_direction(int x, int y)
{
    // Normaliza os valores para -1 a 1 (assumindo valor máximo de 4095)
    float nx = (float)(x - 2047.5) / 2047.5;
    float ny = (float)(y - 2047.5) / 2047.5;

    // Define zona morta
    const float deadZone = 0.2;
    if (fabs(nx) < deadZone && fabs(ny) < deadZone)
    {
        return CENTRO;
    }

    // Calcula o ângulo em radianos e converte para graus
    float angle = atan2f(ny, nx) * 180.0f / 3.14159f;

    // Ajusta o ângulo para 0-360
    if (angle < 0)
        angle += 360.0f;

    // Define direção com base no ângulo
    if (angle >= 337.5f || angle < 22.5f)
        return NORTE;
    if (angle >= 22.5f && angle < 67.5f)
        return NORDESTE;
    if (angle >= 67.5f && angle < 112.5f)
        return LESTE;
    if (angle >= 112.5f && angle < 157.5f)
        return SUDESTE;
    if (angle >= 157.5f && angle < 202.5f)
        return SUL;
    if (angle >= 202.5f && angle < 247.5f)
        return SUDOESTE;
    if (angle >= 247.5f && angle < 292.5f)
        return OESTE;
    if (angle >= 292.5f && angle < 337.5f)
        return NOROESTE;

    return CENTRO; // Caso padrão
}

// Atualiza os dados do joystick
void update_joystick_data(int value_x, int value_y)
{
    joystick_data.position_x = value_x;
    joystick_data.position_y = value_y;
    joystick_data.direction = calculate_direction(value_x, value_y);
}

// Lê os dados do joystick e atualiza
void read_joystick()
{
    adc_select_input(0);
    uint adc_x_raw = adc_read();
    adc_select_input(1);
    uint adc_y_raw = adc_read();

    update_joystick_data(adc_x_raw, adc_y_raw);
}