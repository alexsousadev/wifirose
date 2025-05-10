#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdio.h>
#include <hardware/adc.h>
#include <pico/stdlib.h>

#define JOYSTICK_Y_PIN 26
#define JOYSTICK_X_PIN 27

// Definição das direções possíveis
typedef enum {
    CENTRO,
    NORTE,
    NORDESTE,
    LESTE,
    SUDESTE,
    SUL,
    SUDOESTE,
    OESTE,
    NOROESTE
} JoystickDirection;

// Estrutura com os dados do joystick
typedef struct joystick_data
{
    int position_x;
    int position_y;
    JoystickDirection direction;
} joystickData;

extern joystickData joystick_data;

void init_joystick(void);
void read_joystick();

#endif // JOYSTICK_H