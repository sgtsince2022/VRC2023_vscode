#include <Arduino.h>
#include<PS2X_lib.h>
#include<EEB.h>
#include<main.h>



PS2X VRC_PS2;
DCMotor VRC_Motor;
Servo_Motor VRC_Reloader;   
                    /*fl, fh, b_l, b_h,ll, lh, r_l, r_h, xl, ixh, yl, iyh*/
entry_points JOY_ZONE {0, 85, 170, 255, 0, 85, 170, 255, 85, 170, 85, 170};


int16_t pwm_left, pwm_right, MAX_PWM = 800, PWM_U1 = 3000, PWM_U2 = 3000, MIN_PWM = 50;
bool dir_left, dir_right, rolling_dir;
int gear = 0, gear_s = 0, gear_r = 0, mode = 0 /* mode: 0 -> moving ; 1 -> utilities. */ ; 
byte vibrate = 0;

void ps2_init() {
    Serial.println("connecting to ps2..");

    int err = -1;
    for(int i = 0; i < 10; i++) {
        delay(1000);
        err = VRC_PS2.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
        Serial.println("attempting..");
        if(!err) {Serial.println("Sucsessfully Connect PS2 Controller!"); break;}
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

    if (VRC_PS2.ButtonPressed(PSB_L2)) {
        info_monitor();
        delay(50);
    }

    //! @brief PINK/SQUARE PRESSED -> ACTIVATE THE RELOADER

    static bool status_RELOADER = 0;
    if (VRC_PS2.ButtonPressed(PSB_PINK)) {
        if (status_RELOADER == 0) { // DEGREE: 0 -> 60
            VRC_Reloader.Angle(60, 1);
            // VRC_Reloader.Angle(180, 2);
            Serial.println("RELOADING.. (0 -> 30)");
            Serial.println("STAGE: SHOOTER SHOOTS NOW"); delay(50);
        } else { // // DEGREE: 60 -> 0
            VRC_Reloader.Angle(0, 1);
            // VRC_Reloader.Angle(0, 2);
            Serial.println("RELOADING.. (60 -> 0)");
            Serial.println("STAGE: INSERT"); delay(50);
        }
        status_RELOADER = !status_RELOADER;
    }
    
    //! @brief BLUE/CROSS PRESSED -> TOGGLE THE SHOOTER(S)
    
    static bool status_SHOOTER = 0;
    if (VRC_PS2.ButtonPressed(PSB_BLUE)) {
        if (status_SHOOTER == 0) { // shooter on
            VRC_Motor.Run(THE_SHOOTER, PWM_U1, 0);
            Serial.println("THE SHOOTER -> on"); delay(50);
        } else { // shooter off
            VRC_Motor.Run(THE_SHOOTER, 0, 0);
            Serial.println("THE SHOOTER -> off"); delay(50);
        }
        status_SHOOTER = !status_SHOOTER;
    }
    
    //! @brief R1 PRESSED -> CHANGE THE DIRECTION OF THE ROLLER PART
    
    static bool status_rolling_direction = 1;
    if (VRC_PS2.ButtonPressed(PSB_R1)) {
        if (status_rolling_direction == 1) { // rolling dir = 1
            rolling_dir = 1;
            Serial.println("ROLLING DIRECTION -> 1"); delay(50);
        } else { // rolling dir = 0
            rolling_dir = 0;
            Serial.println("ROLLING DIRECTION -> 0"); delay(50);
        }
        status_rolling_direction = !status_rolling_direction;
    }

    //! @brief ORANGE/CIRCLE PRESSED -> TOGGLE THE ROLLER PART
    
    static bool status_rollerpart = 0;
    if (VRC_PS2.ButtonPressed(PSB_CIRCLE)) {
        if (status_rollerpart == 0) { // roller on
            VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
            Serial.println("THE ROLLER -> on"); delay(50);
        } else { // roller off
            VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
            Serial.println("THE ROLLER -> off"); delay(50);
        }
     status_rollerpart = !status_rollerpart;
    }

    //! @brief L1 PRESSED -> CHANGE SETTING MODE
    
    if (VRC_PS2.ButtonPressed(PSB_L1)) {
        if (mode == 0) {mode = 1; Serial.println("SETTING MODE IS NOW 1: SHOOTER"); delay(50);} 
        else if (mode == 1) {mode = 2; Serial.println("SETTING MODE IS NOW 2: ROLLER"); delay(50);} 
        else if (mode == 2) {mode = 0; Serial.println("SETTING MODE IS NOW 0: MOVING"); delay(50);} 
    }
    
    if (mode == 0) { /* change the moving's pwm value */

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
    // else if (VRC_PS2.ButtonReleased(PSB_L3)) {delay(100);}

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

    if (mode == 1) { // change the shooter's pwm value 

        if (VRC_PS2.ButtonPressed(PSB_L3)) {
            if (gear_s < 3) gear_s += 1;
            else gear_s = 0;

            switch (gear_s)
            {
            case 1: PWM_U1 = 3200; Serial.println("GEAR 1 of SHOOTER"); Serial.println("-> PWM_U1 = 3200."); delay(50); break;
            case 2: PWM_U1 = 3400; Serial.println("GEAR 2 of SHOOTER"); Serial.println("-> PWM_U1 = 3400."); delay(50); break;
            default: case 0: PWM_U1 = 3000; Serial.println("GEAR 0 of SHOOTER"); Serial.println("-> PWM_U1 = 3000."); delay(50); break;
            }
        }

        if (VRC_PS2.ButtonPressed(PSB_R3)) {
            if (gear_s > 0) gear_s -= 1;
            else gear_s = 2;

            switch (gear_s)
            {
            case 1: PWM_U1 = 3200; Serial.println("GEAR 1 of SHOOTER"); Serial.println("-> PWM_U1 = 3200."); delay(50); break;
            case 2: PWM_U1 = 3400; Serial.println("GEAR 2 of SHOOTER"); Serial.println("-> PWM_U1 = 3400."); delay(50); break;
            default: case 0: PWM_U1 = 3000; Serial.println("GEAR 0 of SHOOTER"); Serial.println("-> PWM_U1 = 3000."); delay(50); break;
            }
        }
        
    }
    
    if (mode == 2) { // change the roller's pwm value 

        if (VRC_PS2.ButtonPressed(PSB_L3)) {
            if (gear_r < 3) gear_r += 1;
            else gear_r = 0;

            switch (gear_r)
            {
            case 1: PWM_U2 = 3400; Serial.println("GEAR 1 of ROLLER"); Serial.println("-> PWM_U2 = 3400."); delay(50); break;
            case 2: PWM_U2 = 3800; Serial.println("GEAR 2 of ROLLER"); Serial.println("-> PWM_U2 = 3800."); delay(50); break;
            default: case 0: PWM_U2 = 3000; Serial.println("GEAR 0 of ROLLER"); Serial.println("-> PWM_U2 = 3000."); delay(50); break;
            }
        }

        if (VRC_PS2.ButtonPressed(PSB_R3)) {
            if (gear_r > 0) gear_r -= 1;
            else gear_r = 2;

            switch (gear_r)
            {
            case 1: PWM_U2 = 3400; Serial.println("GEAR 1 of ROLLER"); Serial.println("-> PWM_U2 = 3400."); delay(50); break;
            case 2: PWM_U2 = 3800; Serial.println("GEAR 2 of ROLLER"); Serial.println("-> PWM_U2 = 3800."); delay(50); break;
            default: case 0: PWM_U2 = 3000; Serial.println("GEAR 0 of ROLLER"); Serial.println("-> PWM_U2 = 3000."); delay(50); break;
            }
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

    if ((v_RX >= 235) && (v_RX <= 255)) // turning right 
    {
        pwm_left = (int16_t) MAX_PWM;
        pwm_right = (int16_t) MAX_PWM;
        dir_left = 0;
        dir_right = 1;
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
        // Serial.println("idling..");
        delay(50);
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
    }
}

void info_monitor() {
    String current_obj;
    uint16_t current_max;
    int current_gear;

    Serial.println("### INFOMATION MONITOR ###");
    Serial.println("==========================");
    Serial.print("Setting mode:\t\t"); Serial.println(mode, DEC);

    switch (mode)
    {
    case 1:
        current_obj = "THE SHOOTER";
        current_max = PWM_U1;
        current_gear = gear_s;
        break;
    
    case 2:
        current_obj = "THE ROLLER";
        current_max = PWM_U2;
        current_gear = gear_r;
        break;
    
    default: case 0:
        current_obj = "THE ROBOT";
        current_max = MAX_PWM;
        current_gear = gear;
        break;
    }
    
    Serial.print("OBJ currently is:\t"); Serial.println(current_obj);
    Serial.print("GEAR of "); Serial.print(current_obj); Serial.print(":\t"); Serial.println(current_gear);
    Serial.print("MAX_PWM of "); Serial.print(current_obj); Serial.print(":\t"); Serial.println(current_max);
    if (mode == 2) {Serial.print("ROLLING DIRECTION:\t"); Serial.println(rolling_dir);}
}

void setup() {
    Serial.begin(9600);
    Serial.println("VIA B successfully initiated.");
    ps2_init(); 
    VRC_Motor.Init();
    VRC_Reloader.Init();
}

void loop() {
    ps2_ctrl();
    pwm_calc();
}




//comment