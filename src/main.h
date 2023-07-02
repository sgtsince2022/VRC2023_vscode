#ifndef _MAIN_H_
#define _MAIN_H_

// OLD - 2022
#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK

// NEW - 2023
// #define PS2_DAT 25 //MISO  
// #define PS2_CMD 0 //MOSI  
// #define PS2_SEL 15 //SS    
// #define PS2_CLK 14 //SLK  

#define pressures false
#define rumble true 

void ps2_init();
void ps2_ctrl();
void pwm_calc();
void info_monitor();

/*! @brief set up entry points of joysticks
 *  @param forward_low  @param forward_high 
 *  @param backward_low @param backward_high 
 *  @param left_low     @param left_high 
 *  @param right_low    @param right_high 
 *  @param idle_x_low   @param idle_x_high  
 *  @param idle_y_low   @param idle_y_high  
 */   
struct entry_points {
    int forward_low, forward_high, backward_low, backward_high, left_low, left_high, right_low, right_high;
    int idle_x_low, idle_x_high, idle_y_low, idle_y_high;
};

//! @brief MAX_PWM values for MOVING's gear
struct gear {
    int16_t max_0 = 1100;    // base - neutral
    int16_t& init_MAX_PWM = this->max_0;
    int16_t max_1 = 1500;   // gear 1
    int16_t max_2 = 2000;   // gear 2
    int16_t max_3 = 2500;   // gear 3
    int16_t max_4 = 3000;   // gear 4
    int16_t min = 0;
} GEAR_M;

// MOTOR NO.
#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2
#define THE_SHOOTER 3
#define THE_ROLLER 4

// SERVO NO.
#define THE_RELOADER 1 
#define THE_GATE 2

#endif