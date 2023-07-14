#include <Arduino.h>
#include <PS2X_lib.h>
#include <EEB.h>
#include <main.h>
#include <malloc.h>
#include <FastLED.h>

PS2X VRC_PS2;
DCMotor VRC_Motor;
Servo_Motor VRC_Servo;   
                    /*fl, fh, b_l, b_h,ll, lh, r_l, r_h, xl, ixh, yl, iyh*/
entry_points JOY_ZONE {0, 85, 170, 255, 0, 85, 170, 255, 85, 170, 85, 170};
CRGB  VRC_leds[NUM_LEDS];


int16_t MAX_PWM = GEAR_M.init_MAX_PWM;
int16_t MIN_PWM = GEAR_M.min;
int16_t PWM_U1  = 3200;
int16_t PWM_U2  = 4000; 
int gear = 0;
bool dir_left, dir_right, rolling_dir;
bool roller_running = 0;
int angle_gate = 40, angle_reloader = 50;
bool mode_auto = 0, mode_manual =0;

int16_t pwm_left, pwm_right;
uint16_t shooting_pwm = 0, rolling_pwm=0;

byte vibrate = 0;
led_color_t prev_color=NONE;

static unsigned long prev_time_loader = 0;

void led_all_color(int red, int green, int blue);
void set_led(led_color_t color);

void set_led(led_color_t color){
    if(color != prev_color){
        switch(color){
            prev_color = color;
            case RED:
                led_all_color(120,0,0);
            break;
            case BLUE:
                led_all_color(0,0,120);
            break;
            case GREEN:
                led_all_color(0,120,0);
            break;
            case YELLOW:
                led_all_color(120,120,0);
            break;
            case PURPLE:
                led_all_color(100,0,100);
            break;
            case WHITE:
                led_all_color(120,120,120);
            break;
            case CYAN:
                led_all_color(0,120,120);
            break;
            default:
            break;
        }
    }
}
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

static bool status_SHOOTER = 0;
static bool status_GATE = 0;
static bool status_AUTO = 0;
void ps2_ctrl() {

    //! @brief GREEN/TRIANGLE PRESSED -> ACTIVATE THE GATE

    if (VRC_PS2.ButtonPressed(PSB_GREEN)) {
        if (status_GATE == 0) { // DEGREE: 0 -> angle_gate
            VRC_Servo.Angle(angle_gate, THE_GATE);
            // Serial.println("OPENING.. BALLS into RELOADER");
            // Serial.println("============================="); 
            // delay(50);
        } else { // // DEGREE: angle_gate -> 0
            VRC_Servo.Angle(0, THE_GATE);
            // Serial.println("CLOSING.. BALLS into STORAGE");
            // Serial.println("============================"); 
            // delay(50);
        }
        status_GATE = !status_GATE;
    }
    
    //! @brief PINK/SQUARE PRESSED -> ACTIVATE THE RELOADER
    if (VRC_PS2.ButtonPressed(PSB_PINK)) {
        while(VRC_PS2.ButtonPressed(PSB_PINK));
            set_led(CYAN);
            delay(10);
            mode_auto = 0;
            mode_manual = 1;
            VRC_Servo.Angle(0, THE_RELOADER);
            prev_time_loader = millis();
            //delay(800);
            //VRC_Servo.Angle(angle_reloader, THE_RELOADER);
            // Serial.println("RELOADING..");
            // Serial.println("==========="); 
            //delay(5);
    }
    

    //! @brief BLUE/CROSS PRESSED -> TOGGLE THE SHOOTER(S)
    
    if (VRC_PS2.ButtonPressed(PSB_BLUE)) {
        status_SHOOTER = !status_SHOOTER;
    }
    
    //! @brief L1 PRESSED -> TOGGLE THE ROLLER PART - DIRECTION TO 1 - INTAKE
    if (VRC_PS2.ButtonPressed(PSB_L1)) {
        if (roller_running == 0) {
            rolling_dir = 1; // intake
            rolling_pwm = PWM_U2;
            roller_running = 1;
            // Serial.println("INTAKING..");
            // Serial.println("=========="); delay(50);
        } 
        else{
            rolling_pwm = 0;
            roller_running = 0;
        }
    }


    //! @brief R1 PRESSED -> TOGGLE THE ROLLER PART - DIRECTION TO 0 - RELEASE
    if (VRC_PS2.ButtonPressed(PSB_R1)) {
        if (roller_running == 0) {
            rolling_dir = 0; // release
            rolling_pwm = PWM_U2;
            roller_running = 1;
            // Serial.println("INTAKING..");
            // Serial.println("=========="); delay(50);
        } 
        else{
            rolling_pwm = 0;
            roller_running = 0;
        }
    }

    //! @brief L2 PRESSED -> TURN OFF FUNCTIONS
    if (VRC_PS2.ButtonPressed(PSB_L2)) {
            //VRC_Motor.Run(THE_ROLLER, 0, rolling_dir);
            // reset All
            roller_running = 0;
            rolling_pwm = 0;
            pwm_right = 0;
            pwm_left= 0;
            shooting_pwm = 0;
            status_AUTO = 0;
            mode_manual = 0;
            mode_auto =0;
            status_SHOOTER = 0;
            status_GATE = 0;
            VRC_Servo.Angle(ANGLE_LOAD,THE_RELOADER);
            VRC_Servo.Angle(angle_gate, THE_GATE);
            // Serial.println("ROLLER STOP");
            // Serial.println("==========="); delay(50);
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

        // Serial.println("==================");
        // delay(50);
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

        // Serial.println("==================");
        // delay(50);
    }
    
    //! @brief R2 PRESSED -> GEAR SET TO NEUTRAL (GEAR 0)
    if (VRC_PS2.ButtonPressed(PSB_R2)) {
        gear = GEAR_M.init_MAX_PWM;
    }

    //Auto shooting
    if (VRC_PS2.ButtonPressed(PSB_CIRCLE)){
        status_AUTO = !status_AUTO;
        mode_auto = 1;
        mode_manual = 0;
        if(status_AUTO == 0){
            set_led(GREEN);
        }
        else
        {
            set_led(CYAN);
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
        
        // VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        // VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
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
    
        // VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        // VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
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
    
        // VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        // VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
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
    
        // VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        // VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        
        Serial.print("turning right (left, -right): "); 
        Serial.print(pwm_left, DEC); Serial.print(","); Serial.println(pwm_right, DEC);
        // delay(50);
    } 
    
    // IDLING 
    if ( ( (v_LY > JOY_ZONE.idle_y_low) && (v_LY < JOY_ZONE.idle_y_high) ) 
      && ( (v_RX > JOY_ZONE.idle_x_low) && (v_RX < JOY_ZONE.idle_x_high) ) ) 
    {
        pwm_left = (int16_t) 0;
        pwm_right = (int16_t) 0;
        dir_left = 0;
        dir_right = 0;
    
        // VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        // VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
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

TimerHandle_t xTimers[4];
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
        // Serial.print("Hello ESP: ");
        // Serial.println(count);
        // count++;

        if((pwm_left>0 || pwm_right >0) && status_AUTO == 0){
            //led_all_color(180,180,0); //Yelow running
            set_led(YELLOW);
        }
        else if(shooting_pwm>0 && status_AUTO == 0){
            //led_all_color(100,0,0); //Red shooting
            set_led(RED);
        }
        else{
            //led_all_color(0,180,0); //Green Waiting
            if(status_AUTO == 0){
                set_led(GREEN);
            }
        }
    }

    //timer 2 acceleration shooting
    if(ulCount==2){
        if(status_SHOOTER == 1){
            if(shooting_pwm < PWM_U1){
                shooting_pwm += 200;
            }
            else{
                shooting_pwm = PWM_U1;
            }
        }
        else{
            shooting_pwm = 0;
        }
    }

    //timer 3 auto shooting 
    if(ulCount==3){
        if(status_AUTO == 1){
            //Auto shoot
            if(angle_reloader==0){
                VRC_Servo.Angle(ANGLE_LOAD, THE_RELOADER);
                angle_reloader = ANGLE_LOAD;
            }
            else{
                VRC_Servo.Angle(ANGLE_SHOOT, THE_RELOADER);
                angle_reloader = ANGLE_SHOOT;
            }
        }
        else{
            VRC_Servo.Angle(ANGLE_LOAD, THE_RELOADER);
        }
    }
}

void led_all_color(int red, int green, int blue){
  for(int i=0;i<NUM_LEDS;i++){
    VRC_leds[i] = CRGB(red, green, blue);
    FastLED.show();
  }
}

void setup() {
    Serial.begin(9600); Serial.println("VIA B successfully initiated.");

    // led config
    FastLED.addLeds<WS2812, LED_PIN, GRB>(VRC_leds, NUM_LEDS);
    //led_all_color(100,0,100); //purple
    set_led(PURPLE);

    ps2_init(); 
    info_monitor();

    //led_all_color(0,180,0); //green, done Init
    set_led(GREEN);
    // Create Timer
    xTimers[ 0 ] = xTimerCreate("Timer PS2",pdMS_TO_TICKS(50),pdTRUE,( void * ) 0,timerCallBack);
    xTimerStart(xTimers[0],0);

    xTimers[ 1 ] = xTimerCreate("Timer test",pdMS_TO_TICKS(500),pdTRUE,( void * ) 1,timerCallBack);
    xTimerStart(xTimers[1],0);

    xTimers[ 2 ] = xTimerCreate("Timer accel shoot",pdMS_TO_TICKS(50),pdTRUE,( void * ) 2,timerCallBack);
    xTimerStart(xTimers[2],0);

    xTimers[ 3 ] = xTimerCreate("Timer auto shoot",pdMS_TO_TICKS(800),pdTRUE,( void * ) 3,timerCallBack);
    xTimerStart(xTimers[3],0);

    VRC_Motor.Init();
    VRC_Servo.Init();
    VRC_Servo.Angle(ANGLE_LOAD,THE_RELOADER);
    VRC_Servo.Angle(angle_gate, THE_GATE);

}

void loop() {
    if(running_permission){
        ps2_ctrl();
        pwm_calc();
        VRC_Motor.Run(LEFT_MOTOR, pwm_left, dir_left);
        VRC_Motor.Run(RIGHT_MOTOR, pwm_right, dir_right);
        VRC_Motor.Run(THE_SHOOTER, shooting_pwm,0);
        VRC_Motor.Run(THE_ROLLER, rolling_pwm, rolling_dir);
        if(millis()-prev_time_loader > 1000 && mode_manual == 1){
            VRC_Servo.Angle(angle_reloader, THE_RELOADER);
        }
    }

}