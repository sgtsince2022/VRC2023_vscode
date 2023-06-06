#include <Arduino.h>
#include <PS2X_lib.h>
#include "main.h"
#include <EEB.h>

PS2X VRC_PS2;
DCMotor VRC_Motor;
int pwm_left, pwm_right; bool dir_left, dir_right;

void setup() {
    Serial.begin(115200);
    Serial.println("Mach khoi tao thanh cong!");
    ps2_init();
    VRC_Motor.Init();
}

void loop() {
    ps2_ctrl();
    analog_to_pwm_value();
    delay(50);
    VRC_Motor.Run(1, pwm_left, 0);
    VRC_Motor.Run(2, pwm_right, dir_right);
}

void ps2_init() {
    Serial.println("connecting to ps2..");

    int err = -1;
    for (int i = 0; i < 10; i++) {
        delay(1000);
        err = VRC_PS2.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
        Serial.println("attempting..");
    }

    switch (err)
    {
    case 0:
    Serial.println(" Ket noi tay cam PS2 thanh cong");
    break;
  case 1:
    Serial.println(" LOI: Khong tim thay tay cam, hay kiem tra day ket noi vơi tay cam ");
    break;
  case 2:
    Serial.println(" LOI: khong gui duoc lenh");
    break;
  case 3:
    Serial.println(" LOI: Khong vao duoc Pressures mode ");
    break;
    }
}

void ps2_ctrl() {
    VRC_PS2.read_gamepad(false, false);

    if (VRC_PS2.Button(PSB_L1)) {
        Serial.print("LY, RX: ");
        Serial.print(VRC_PS2.Analog(PSS_LY), DEC); 
        Serial.print(", "); Serial.println(VRC_PS2.Analog(PSS_RX));
    }
}

void analog_to_pwm_value() {
    byte v_LY = VRC_PS2.Analog(PSS_LY);
    byte v_RX = VRC_PS2.Analog(PSS_RX);

    if ((v_LY >= 0) && (v_LY <= 20)) {
        pwm_left = MAX_PWM*1;
        pwm_right = MAX_PWM*1;
    } else {pwm_left = 0; pwm_right = 0;}

    if ((v_LY >= 235) && (v_LY <= 255)) {
        pwm_left = -MAX_PWM*1;
        pwm_right = -MAX_PWM*1;
    } else {pwm_left = 0; pwm_right = 0;}

    if ((v_RX >= 0) && (v_RX <= 20)) {
        pwm_left = MAX_PWM/2;
        pwm_right = MAX_PWM/3*2;
    } else {pwm_left = 0; pwm_right = 0;}

    if ((v_RX >= 235) && (v_RX <= 255)) {
        pwm_left = (int16_t) MAX_PWM/3*2;
        pwm_right = (int16_t) MAX_PWM/3*2;
        dir_right = 1;
    } else {pwm_left = 0; pwm_right = 0;}
    
}   