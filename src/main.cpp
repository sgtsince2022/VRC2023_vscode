#include <Arduino.h>
#include<PS2X_lib.h>
#include<EEB.h>
#include<main.h>

PS2X VRC_PS2;
DCMotor VRC_Motor;

int16_t pwm_left, pwm_right; 
bool dir_left, dir_right, dir_cuon;


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

    if (VRC_PS2.ButtonPressed(PSB_BLUE)) { // ban on
        VRC_Motor.Run(3, 4096, 0);
        Serial.println("bantumlum -> on"); delay(50);
    }
    
    if (VRC_PS2.ButtonPressed(PSB_RED)) { // ban off
        VRC_Motor.Run(3, 4096, 0);
        Serial.println("bantumlum -> off"); delay(50);
    }


    if (VRC_PS2.ButtonPressed(PSB_R1)) { 
        dir_cuon = 1;
        Serial.println("dir -> 1"); delay(50);
    }
    
    if (VRC_PS2.ButtonPressed(PSB_R2)) {
        dir_cuon = 0;
        Serial.println("dir -> 0"); delay(50);
    }

    if (VRC_PS2.ButtonPressed(PSB_PINK)) { // bat cuon 
        VRC_Motor.Run(4, 4096, dir_cuon);
        Serial.println("cuon bong -> on"); delay(50);
    }

    if (VRC_PS2.ButtonPressed(PSB_GREEN)) { // tat cuon 
        VRC_Motor.Run(4, 0, dir_cuon);
        Serial.println("cuon bong -> off"); delay(50);
    }

}
void pwm_calc() {
    byte v_LY = VRC_PS2.Analog(PSS_LY);
    byte v_RX = VRC_PS2.Analog(PSS_RX);
    int cmd = 0;
    if ((v_LY >= 0) && (v_LY <= 20)) // forward
    {
        pwm_left = (int16_t) MAX_PWM;
        pwm_right = (int16_t) MAX_PWM;
        dir_left = 0;
        dir_right = 0;
        Serial.println("moving forward.."); delay(50);
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
    } 
    
    if ((v_LY >= 235) && (v_LY <= 255)) // backward
    {
        pwm_left = (int16_t) MAX_PWM;
        pwm_right = (int16_t) MAX_PWM;
        dir_left = 1;
        dir_right = 1;
        Serial.println("moving backward.."); delay(50);
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
    } 
    
    if ((v_RX >= 0) && (v_RX <= 20)) // turning left 
    {
        pwm_left = (int16_t) MAX_PWM/3;
        pwm_right = (int16_t) MAX_PWM/2;
        dir_left = 0;
        dir_right = 0;
        Serial.println("turning left.."); delay(50);
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
    } 

    if ((v_RX >= 235) && (v_RX <= 255)) // turning right 
    {
        pwm_left = (int16_t) MAX_PWM/2;
        pwm_right = (int16_t) MAX_PWM/3;
        dir_left = 0;
        dir_right = 0;
        Serial.println("turning right.."); delay(50);
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
    } 
    
    // stop 
    if ( ( (v_LY > 20) && (v_LY < 235) ) && ( (v_RX > 20) && v_RX < 235) ) {
        pwm_left = (int16_t) 0;
        pwm_right = (int16_t) 0;
        dir_left = 0;
        dir_right = 0;
        Serial.println("idling.."); delay(50);
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
    }
    /*
    switch (cmd)
    {
    case 1: // forward
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
    */
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
}