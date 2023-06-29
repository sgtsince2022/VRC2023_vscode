#include <Arduino.h>
#include<PS2X_lib.h>
#include<EEB.h>
#include<main.h>



PS2X VRC_PS2;
DCMotor VRC_Motor;
Servo_Motor VRC_Reloader, VRC_Gate;   
                    /*fl, fh, b_l, b_h,ll, lh, r_l, r_h, xl, ixh, yl, iyh*/
entry_points JOY_ZONE {0, 85, 170, 255, 0, 85, 170, 255, 85, 170, 85, 170};


int16_t MAX_PWM = GEAR_M.init_MAX_PWM;
int16_t MIN_PWM = GEAR_M.min;
int16_t PWM_U1  = 4000;
int16_t PWM_U2  = 4000; 
int gear = 0;
bool dir_left, dir_right, rolling_dir;
int16_t pwm_left, pwm_right;
byte vibrate = 0;

void ps2_init() {
    Serial.println("connecting to ps2..");

    int err = -1;
    for(int i = 0; i < 10; i++) {
        delay(1000);
        err = VRC_PS2.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
        Serial.println("attempting..");
        if(!err) {Serial.println("Successfully Connected PS2 Controller!"); 
                  Serial.println("===================================="); break;}
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

    //! @brief L2 PRESSED -> INFOMATION MONITOR
    if (VRC_PS2.ButtonPressed(PSB_L2)) {
        info_monitor();
        delay(50);
    }

    //! @brief PINK/SQUARE PRESSED -> ACTIVATE THE RELOADER
    static bool status_RELOADER = 0;
    if (VRC_PS2.ButtonPressed(PSB_PINK)) {
        if (status_RELOADER == 0) { // DEGREE: 0 -> 60
            VRC_Reloader.Angle(60, 1);
            Serial.println("RELOADING.. (0 -> 60) - STAGE: SHOOTER SHOOTS NOW");
            Serial.println("================================================="); 
            delay(50);
        } else { // // DEGREE: 60 -> 0
            VRC_Reloader.Angle(0, 1);
            Serial.println("RELOADING.. (60 -> 0) - STAGE: INSERT");
            Serial.println("====================================="); 
            delay(50);
        }
        status_RELOADER = !status_RELOADER;
    }
    
    //! @brief GREEN/TRIANGLE PRESSED -> ACTIVATE THE GATE
    static bool status_GATE = 0;
    if (VRC_PS2.ButtonPressed(PSB_GREEN)) {
        if (status_GATE == 0) { // DEGREE: 0 -> 80
            VRC_Gate.Angle(80, 2);
            Serial.println("OPENING.. BALLS into RELOADER");
            Serial.println("============================="); 
            delay(50);
        } else { // // DEGREE: 80 -> 0
            VRC_Gate.Angle(0, 2);
            Serial.println("CLOSING.. BALLS into STORAGE");
            Serial.println("============================"); 
            delay(50);
        }
        status_GATE = !status_GATE;
    }

    //! @brief BLUE/CROSS PRESSED -> TOGGLE THE SHOOTER(S)
    static bool status_SHOOTER = 0;
    if (VRC_PS2.ButtonPressed(PSB_BLUE)) {
        if (status_SHOOTER == 0) { // shooter on
            VRC_Motor.Run(THE_SHOOTER, PWM_U1, 0);
            Serial.println("THE SHOOTER -> ON"); 
            Serial.println("================="); 
            delay(50);
        } else { // shooter off
            VRC_Motor.Run(THE_SHOOTER, 0, 0);
            Serial.println("THE SHOOTER -> OFF"); 
            Serial.println("=================="); 
            delay(50);
        }
        status_SHOOTER = !status_SHOOTER;
    }
    
    //! @brief R1 PRESSED -> CHANGE THE DIRECTION OF THE ROLLER PART
    static bool status_rolling_direction = 1;
    if (VRC_PS2.ButtonPressed(PSB_R1)) {
        if (status_rolling_direction == 1) { // rolling dir = 1
            rolling_dir = 1;
            Serial.println("ROLLING DIRECTION -> 1"); 
            Serial.println("======================"); 
            delay(50);
        } else { // rolling dir = 0
            rolling_dir = 0;
            Serial.println("ROLLING DIRECTION -> 0"); 
            Serial.println("======================"); 
            delay(50);
        }
        status_rolling_direction = !status_rolling_direction;
    }

    //! @brief ORANGE/CIRCLE PRESSED -> TOGGLE THE ROLLER PART
    static bool status_rollerpart = 0;
    if (VRC_PS2.ButtonPressed(PSB_CIRCLE)) {
        if (status_rollerpart == 0) { // roller on
            VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
            Serial.println("THE ROLLER -> ON"); 
            Serial.println("================"); 
            delay(50);
        } else { // roller off
            VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
            Serial.println("THE ROLLER -> OFF"); 
            Serial.println("================="); 
            delay(50);
        }
        status_rollerpart = !status_rollerpart;
    }

    //! @brief L3 PRESSED -> GEAR UP 
    if (VRC_PS2.ButtonPressed(PSB_L3)) {
        if (gear < 4) {gear += 1;} 
        else {gear = 0;}

        switch (gear)
        {
        case 1: MAX_PWM = 1000; Serial.println("GEAR 1"); Serial.println("-> PWM = 1000."); delay(50); break;
        case 2: MAX_PWM = 1200; Serial.println("GEAR 2"); Serial.println("-> PWM = 1200."); delay(50); break;
        case 3: MAX_PWM = 1300; Serial.println("GEAR 3"); Serial.println("-> PWM = 1300."); delay(50); break;
        case 4: MAX_PWM = 1500; Serial.println("GEAR 4"); Serial.println("-> PWM = 1500."); delay(50); break;
        default: case 0: MAX_PWM = 800; Serial.println("GEAR 0"); Serial.println("-> PWM = 800."); delay(50); break;
        }
    }

    //! @brief R3 PRESSED -> GEAR DOWN  
    if (VRC_PS2.ButtonPressed(PSB_R3)) {
        if (gear > 0) gear -= 1;
        else gear = 4;

        switch (gear)
        {
        case 1: MAX_PWM = 1000; Serial.println("GEAR 1"); Serial.println("-> PWM = 1000."); delay(50); break;
        case 2: MAX_PWM = 1200; Serial.println("GEAR 2"); Serial.println("-> PWM = 1200."); delay(50); break;
        case 3: MAX_PWM = 1300; Serial.println("GEAR 3"); Serial.println("-> PWM = 1300."); delay(50); break;
        case 4: MAX_PWM = 1500; Serial.println("GEAR 4"); Serial.println("-> PWM = 1500."); delay(50); break;
        default: case 0: MAX_PWM = 800; Serial.println("GEAR 0"); Serial.println("-> PWM = 800."); delay(50); break;
        }
    }

    

}

void pwm_calc() {
    byte v_LY = VRC_PS2.Analog(PSS_LY);
    byte v_RX = VRC_PS2.Analog(PSS_RX);
    int cmd = 0;
    
    // FORWARD
    if ((v_LY >= JOY_ZONE.forward_low) && (v_LY <= JOY_ZONE.forward_high)) 
    {
        pwm_left  = (int16_t) map(v_LY, JOY_ZONE.forward_low, JOY_ZONE.forward_high, MAX_PWM, MIN_PWM);
        pwm_right = (int16_t) map(v_LY, JOY_ZONE.forward_low, JOY_ZONE.forward_high, MAX_PWM, MIN_PWM);
        dir_left  = 0;
        dir_right = 0;
        
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
        Serial.print("moving forward (left, right): "); 
        Serial.print(pwm_left, DEC); Serial.print(","); Serial.println(pwm_right, DEC);
        delay(50);
    } 
    
    // BACKWARD
    if ((v_LY >= JOY_ZONE.backward_low) && (v_LY <= JOY_ZONE.backward_high)) 
    {
        pwm_left  = (int16_t) map(v_LY, JOY_ZONE.backward_low, JOY_ZONE.backward_high, MIN_PWM, MAX_PWM);
        pwm_right = (int16_t) map(v_LY, JOY_ZONE.backward_low, JOY_ZONE.backward_high, MIN_PWM, MAX_PWM);
        dir_left  = 1;
        dir_right = 1;
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
        Serial.print("moving backward (left, right): "); 
        Serial.print(pwm_left, DEC); Serial.print(","); Serial.println(pwm_right, DEC);
        delay(50);
    } 
    
    // TURNING LEFT
    if ((v_RX >= JOY_ZONE.left_low) && (v_RX <= JOY_ZONE.left_high)) // turning left 
    {
        pwm_left  = (int16_t) map(v_RX, JOY_ZONE.left_low, JOY_ZONE.left_high, MAX_PWM, MIN_PWM);
        pwm_right = (int16_t) map(v_RX, JOY_ZONE.left_low, JOY_ZONE.left_high, MAX_PWM, MIN_PWM);
        dir_left  = 1;
        dir_right = 0;
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
        Serial.print("turning left (-left, right): "); 
        Serial.print(pwm_left, DEC); Serial.print(","); Serial.println(pwm_right, DEC);
        delay(50);
    } 

    // TURNING RIGHT
    if ((v_RX >= JOY_ZONE.right_low) && (v_RX <= JOY_ZONE.right_high))  
    {
        pwm_left  = (int16_t) map(v_RX, JOY_ZONE.right_low, JOY_ZONE.right_high, MIN_PWM, MAX_PWM);
        pwm_right = (int16_t) map(v_RX, JOY_ZONE.right_low, JOY_ZONE.right_high, MIN_PWM, MAX_PWM);
        dir_left = 0;
        dir_right = 1;
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
        Serial.print("turning right (left, -right): "); 
        Serial.print(pwm_left, DEC); Serial.print(","); Serial.println(pwm_right, DEC);
        delay(50);
    } 
    
    // IDLING 
    if ( ( (v_LY > JOY_ZONE.idle_y_low) && (v_LY < JOY_ZONE.idle_y_high) ) 
      && ( (v_RX > JOY_ZONE.idle_x_low) && (v_RX < JOY_ZONE.idle_x_high) ) ) 
    {
        pwm_left = (int16_t) 0;
        pwm_right = (int16_t) 0;
        dir_left = 0;
        dir_right = 0;
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
        delay(50);
    }
}

void info_monitor() {
    String current_obj = "THE ROBOT";
    uint16_t current_max = MAX_PWM;
    int current_gear = gear;

    Serial.println("### INFOMATION MONITOR ###");
    Serial.println("==========================");

    Serial.print("GEAR of "); Serial.print(current_obj); Serial.print(":\t"); Serial.println(current_gear);
    Serial.print("MAX_PWM of "); Serial.print(current_obj); Serial.print(":\t"); Serial.println(current_max);
    Serial.println("=========================="); 
    delay(50);
}

void setup() {
    Serial.begin(9600); Serial.println("VIA B successfully initiated.");
    ps2_init(); 
    info_monitor();
    VRC_Motor.Init();
    VRC_Reloader.Init();
    VRC_Gate.Init();
}

void loop() {
    ps2_ctrl();
    pwm_calc();
}