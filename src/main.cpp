#include <Arduino.h>
#include<PS2X_lib.h>
#include<EEB.h>
#include<main.h>



PS2X VRC_PS2;
DCMotor VRC_Motor;
Servo_Motor VRC_Reloader;

int16_t pwm_left, pwm_right, MAX_PWM = 800, PWM_U1 = 3000, PWM_U2 = 3000;
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
    VRC_PS2.read_gamepad(false, vibrate);

    if (VRC_PS2.Button(PSB_L2)) {
        Serial.print("LY, RX: ");
        Serial.print(VRC_PS2.Analog(PSS_LY), DEC); 
        Serial.print(", "); Serial.println(VRC_PS2.Analog(PSS_RX));
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
        if (gear < 4) {gear += 1; vibrate = 255;} 
        else {gear = 0; vibrate = 255;}

        switch (gear)
        {
        case 1: MAX_PWM = 1000; Serial.println("GEAR 1"); Serial.println("-> PWM = 1000."); delay(50); break;
        case 2: MAX_PWM = 1200; Serial.println("GEAR 2"); Serial.println("-> PWM = 1200."); delay(50); break;
        case 3: MAX_PWM = 1300; Serial.println("GEAR 3"); Serial.println("-> PWM = 1300."); delay(50); break;
        case 4: MAX_PWM = 1500; Serial.println("GEAR 4"); Serial.println("-> PWM = 1500."); delay(50); break;
        default: case 0: MAX_PWM = 800; Serial.println("GEAR 0"); Serial.println("-> PWM = 800."); delay(50); break;
        }
    }
    else if (VRC_PS2.ButtonReleased(PSB_L3)) {delay(100); vibrate = 0;}

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
    if ((v_LY >= 0) && (v_LY <= 60)) // forward
    {
        pwm_left = (int16_t) map(v_LY, 0, 50, MAX_PWM, 50);
        pwm_right = (int16_t) map(v_LY, 0, 50,MAX_PWM, 50);
        dir_left = 0;
        dir_right = 0;
        Serial.println("moving forward..       "); 
        Serial.print(pwm_left, DEC); Serial.print(","); Serial.println(pwm_right, DEC); 
        delay(50);
    
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
        pwm_left = (int16_t) MAX_PWM;
        pwm_right = (int16_t) MAX_PWM;
        dir_left = 1;
        dir_right = 0;
        Serial.println("turning left.."); delay(50);
    
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
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