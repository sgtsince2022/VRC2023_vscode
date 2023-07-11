#include <Arduino.h>
#include <PS2X_lib.h>
#include <EEB.h>
#include <main.h>
#include <malloc.h>


PS2X VRC_PS2;
DCMotor VRC_Motor;
Servo_Motor VRC_Servo;   
                    /*fl, fh, b_l, b_h,ll, lh, r_l, r_h, xl, ixh, yl, iyh*/
entry_points JOY_ZONE {0, 85, 170, 255, 0, 85, 170, 255, 85, 170, 85, 170};


int16_t MAX_PWM = GEAR_M.init_MAX_PWM;
int16_t MIN_PWM = GEAR_M.min;
int16_t PWM_U1  = 4000;
int16_t PWM_U2  = 4000; 
int gear = 0;
bool dir_left, dir_right, rolling_dir, roller_running = 0;
int angle_gate = 40, angle_reloader = 50;
bool mode_auto = 0;
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
    // VRC_PS2.read_gamepad(false, false);

    //! @brief L2 PRESSED -> INFOMATION MONITOR
    if (VRC_PS2.ButtonPressed(PSB_L2)) {
        if (roller_running != 1) {
            info_monitor();
            delay(50);
        }
    }

    //! @brief GREEN/TRIANGLE PRESSED -> ACTIVATE THE GATE
    static bool status_GATE = 0;
    if (VRC_PS2.ButtonPressed(PSB_GREEN)) {
        if (status_GATE == 0) { // DEGREE: 0 -> angle_gate
            VRC_Servo.Angle(angle_gate, THE_GATE);
            Serial.println("OPENING.. BALLS into RELOADER");
            Serial.println("============================="); 
            delay(50);
        } else { // // DEGREE: angle_gate -> 0
            VRC_Servo.Angle(0, THE_GATE);
            Serial.println("CLOSING.. BALLS into STORAGE");
            Serial.println("============================"); 
            delay(50);
        }
        status_GATE = !status_GATE;
    }

    //! @brief PINK/SQUARE PRESSED -> ACTIVATE THE RELOADER
    if (VRC_PS2.ButtonPressed(PSB_PINK)) {
            VRC_Servo.Angle(0, THE_RELOADER);
            delay(500);
            VRC_Servo.Angle(angle_reloader, THE_RELOADER);
            
            Serial.println("RELOADING..");
            Serial.println("==========="); 
            delay(50);
    }
    

    //! @brief BLUE/CROSS PRESSED -> TOGGLE THE SHOOTER(S)
    static bool status_SHOOTER = 0;
    if (VRC_PS2.ButtonPressed(PSB_BLUE)) {
        if (status_SHOOTER == 0) { // shooter on
            VRC_Motor.Run(THE_SHOOTER, PWM_U1, 0);
            delay(500); // wait for the shooter to reach balance speed
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
    
    //! @brief L1 PRESSED -> TOGGLE THE ROLLER PART - DIRECTION TO 1 - INTAKE
    
    if (VRC_PS2.ButtonPressed(PSB_L1)) {
        if (roller_running == 0) {
            rolling_dir = 1; // intake
            VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
            roller_running = 1;
        
            Serial.println("INTAKING..");
            Serial.println("=========="); delay(50);
        } else if (roller_running == 1) {
            if (rolling_dir == 0) {
                rolling_dir = 1; // intake
                VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
                roller_running = 1;
        
                Serial.println("INTAKING..");
                Serial.println("=========="); delay(50);
            } else {
                VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
                roller_running = 0;

                Serial.println("ROLLER STOP");
                Serial.println("==========="); delay(50);
            }
        }
    }

    // static bool status_rollerpart_r1 = 0;
    // if (VRC_PS2.ButtonPressed(PSB_R1)) {
    //     rolling_dir = 1;
    //     if (status_rollerpart_r1 == 0) { // roller on
    //         VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
    //         roller_running = 1;
            
    //         Serial.print("THE ROLLER -> ON, DIR = "); Serial.println(rolling_dir, DEC); 
    //         Serial.println("================"); 
    //         delay(50);
    //     } else { // roller off
    //         VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
    //         roller_running = 0;
            
    //         Serial.print("THE ROLLER -> OFF, DIR = "); Serial.println(rolling_dir, DEC); 
    //         Serial.println("================="); 
    //         delay(50);
    //     }
    //     status_rollerpart_r1 = !status_rollerpart_r1;
    // }

    //! @brief R1 PRESSED -> TOGGLE THE ROLLER PART - DIRECTION TO 0 - RELEASE
    
    if (VRC_PS2.ButtonPressed(PSB_R1)) {
        if (roller_running == 0) {
            rolling_dir = 0; // release
            VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
            roller_running = 1;
        
            Serial.println("RELEASING..");
            Serial.println("==========="); delay(50);
        } else if (roller_running == 1) {
            if (rolling_dir == 1) { // intaking ? releasing : off
                rolling_dir = 0; // release
                VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
                roller_running = 1;
        
                Serial.println("RELEASING..");
                Serial.println("==========="); delay(50);
            } else {
                VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
                roller_running = 0;

                Serial.println("ROLLER STOP");
                Serial.println("==========="); delay(50);
            }
        }
    }

    // static bool status_rollerpart_l1 = 0;
    // if (VRC_PS2.ButtonPressed(PSB_L1)) {
    //     rolling_dir = 0;

    //     if (status_rollerpart_l1 == 0) { // roller on
    //         VRC_Motor.Run(THE_ROLLER, PWM_U2, rolling_dir);
    //         roller_running = 1;
    //         Serial.print("THE ROLLER -> ON, DIR = "); Serial.println(rolling_dir, DEC); 
    //         Serial.println("================"); 
    //         delay(50);
    //     } else { // roller off
    //         VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
    //         roller_running = 0;
    //         Serial.print("THE ROLLER -> OFF, DIR = "); Serial.println(rolling_dir, DEC); 
    //         Serial.println("================="); 
    //         delay(50);
    //     }
    //     status_rollerpart_l1 = !status_rollerpart_l1;
    // }

    //! @brief L2 PRESSED -> TURN OFF THE ROLLER
    if (VRC_PS2.ButtonPressed(PSB_L2)) {
        if (roller_running == 1) {
            VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
            roller_running = 0;
        
            Serial.println("ROLLER STOP");
            Serial.println("==========="); delay(50);
        }
    }

    //! @brief L3 PRESSED -> GEAR UP 
    if (VRC_PS2.ButtonPressed(PSB_L3)) {
        if (gear < 4) {gear += 1;} 
        else {gear = 0;}

        switch (gear)
        {
        case 1: Serial.println("GEAR 1"); MAX_PWM = GEAR_M.max_1; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_1, DEC); break;
        case 2: Serial.println("GEAR 2"); MAX_PWM = GEAR_M.max_2; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_2, DEC); break;
        case 3: Serial.println("GEAR 3"); MAX_PWM = GEAR_M.max_3; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_3, DEC); break;
        case 4: Serial.println("GEAR 4"); MAX_PWM = GEAR_M.max_4; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_4, DEC); break;
        default: 
        case 0: Serial.println("GEAR 0"); MAX_PWM = GEAR_M.max_0; Serial.println(" -> PWM = "); Serial.println(GEAR_M.max_0, DEC); break;
        }

        Serial.println("==================");
        delay(50);
    }

    //! @brief R3 PRESSED -> GEAR DOWN  
    if (VRC_PS2.ButtonPressed(PSB_R3)) {
        if (gear > 0) gear -= 1;
        else gear = 4;

        switch (gear)
        {
        case 1: Serial.println("GEAR 1"); MAX_PWM = GEAR_M.max_1; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_1, DEC); break;
        case 2: Serial.println("GEAR 2"); MAX_PWM = GEAR_M.max_2; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_2, DEC); break;
        case 3: Serial.println("GEAR 3"); MAX_PWM = GEAR_M.max_3; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_3, DEC); break;
        case 4: Serial.println("GEAR 4"); MAX_PWM = GEAR_M.max_4; Serial.print(" -> PWM = "); Serial.println(GEAR_M.max_4, DEC); break;
        default: 
        case 0: Serial.println("GEAR 0"); MAX_PWM = GEAR_M.max_0; Serial.println(" -> PWM = "); Serial.println(GEAR_M.max_0, DEC); break;
        }

        Serial.println("==================");
        delay(50);
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

    Serial.print("GEAR of "); Serial.print(current_obj); Serial.print(":\t"); Serial.println(current_gear);
    Serial.print("MAX_PWM of "); Serial.print(current_obj); Serial.print(":\t"); Serial.println(current_max);
    Serial.println(); 
    delay(50);
}

TimerHandle_t xTimers[2];
unsigned long count = 0;
bool running_permission = 0;
void timerCallBack(TimerHandle_t xTimer){
    configASSERT(xTimer);
    int ulCount = (uint32_t) pvTimerGetTimerID(xTimer);

    // timer 0 reading gamepad
    // khong co PS2 thi ham nay khong chay thanh cong, bi treo
    if(ulCount==0){
       // Task 1
        if(VRC_PS2.read_gamepad(0, 0)){
            running_permission = 1;
        }
        else{
            running_permission = 0;
        }

        // VRC_PS2.read_gamepad(0, 0);
    }

    //timer 1 heart beat
    if(ulCount==1){
        Serial.print("Hello ESP: ");
        Serial.println(count);
        count++;
    }
}
void setup() {
    Serial.begin(9600); Serial.println("VIA B successfully initiated.");
    ps2_init(); 
    info_monitor();

    // Create Timer
    xTimers[ 0 ] = xTimerCreate("Timer PS2",pdMS_TO_TICKS(50),pdTRUE,( void * ) 0,timerCallBack);
    xTimerStart(xTimers[0],0);

    xTimers[ 1 ] = xTimerCreate("Timer test",pdMS_TO_TICKS(1000),pdTRUE,( void * ) 1,timerCallBack);
    xTimerStart(xTimers[1],0);

    VRC_Motor.Init();
    VRC_Servo.Init();
    VRC_Servo.Angle(angle_reloader,THE_RELOADER);
    VRC_Servo.Angle(angle_gate, THE_GATE);

}

void loop() {
    if(running_permission){
        ps2_ctrl();
        pwm_calc();
    }

    // ps2_ctrl();
    // pwm_calc();
}