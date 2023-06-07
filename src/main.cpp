#include <Arduino.h>
#include<PS2X_lib.h>
#include<EEB.h>
#include<main.h>

PS2X VRC_PS2;
DCMotor VRC_Motor;

int16_t pwm_left, pwm_right; 
bool dir_left, dir_right;


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
        delay(50);
    }
}
void pwm_calc() {
    byte v_LY = VRC_PS2.Analog(PSS_LY);
    byte v_RX = VRC_PS2.Analog(PSS_RX);
    int cmd = 0;
    if ((v_LY >= 0) && (v_LY <= 20))    {cmd = 1; if (v_LY > 20)  cmd = 0;} // forward
    if ((v_RX >= 0) && (v_RX <= 20))    {cmd = 3; if (v_RX > 20)  cmd = 0;} // turn left 
    if ((v_LY >= 235) && (v_LY <= 255)) {cmd = 2; if (v_LY < 235) cmd = 0;} // backward
    if ((v_RX >= 235) && (v_RX <= 255)) {cmd = 4; if (v_RX < 235) cmd = 0;} // turn right 

    switch (cmd)
    {
    case 1: // forward
        pwm_left = (int16_t) MAX_PWM;
        pwm_right = (int16_t) MAX_PWM;
        dir_left = 0;
        dir_right = 0;
        Serial.println("moving forward.."); delay(50);
        break;
    case 2: // backward
        pwm_left = (int16_t) MAX_PWM;
        pwm_right = (int16_t) MAX_PWM;
        dir_left = 1;
        dir_right = 1;
        Serial.println("moving backward.."); delay(50);
        break;
    case 3: // turn left
        pwm_left = (int16_t) MAX_PWM/3;
        pwm_right = (int16_t) MAX_PWM/3*2;
        dir_left = 0;
        dir_right = 0;
        Serial.println("turning left.."); delay(50);
        break;
    case 4: // turn right
        pwm_left = (int16_t) MAX_PWM/3*2;
        pwm_right = (int16_t) MAX_PWM/3;
        dir_left = 0;
        dir_right = 0;
        Serial.println("turning right.."); delay(50);
        break;
    default: case 0: // stop
        pwm_left = (int16_t) MAX_PWM/3*2;
        pwm_right = (int16_t) MAX_PWM/3;
        break;
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

void setup() {
    Serial.begin(9600);
    Serial.println("Mach khoi tao thanh cong.");
    ps2_init();
    VRC_Motor.Init();
}

void loop() {
    ps2_ctrl();
    pwm_calc();
    VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
    VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
}