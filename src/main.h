#ifndef _MAIN_H_
#define _MAIN_H_

#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK
#define pressures false
#define rumble false

void ps2_init();
void ps2_ctrl();
void analog_to_pwm_value();

#define MAX_PWM 4096

#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2

#endif