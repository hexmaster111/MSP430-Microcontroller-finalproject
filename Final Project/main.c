#include <msp430.h> 
#include <helpers.h>

/**
 * main.c
 */
#define FRACTS_PER_SEC 55 //deff not just a guess....

unsigned long curr_time_s, lastLightTime = 0;
int fract_curr_time = 0;

int sw_0_flag = 0;
int sw_1_flag = 0;

int buzzer_enabled = 0; // crosswalk flag

unsigned int current_state = 0; //Start with a green light
//                   0  1  2  3    state delay time in sec
int delayTimes[] = { 1, 2, 2, 3 };

void change_lights(int state)
{
    switch (state)
    {
    case 0: //Green light
        pin_off(RED_PORT, RED_BIT);
        pin_off(YELLOW_PORT, YELLOW_BIT);
        pin_on(GREEN_PORT, GREEN_BIT);
        break;

    case 1: //Yellow Light
        pin_off(RED_PORT, RED_BIT);
        pin_on(YELLOW_PORT, YELLOW_BIT);
        pin_off(GREEN_PORT, GREEN_BIT);
        break;

    case 3: //cross walk + red light
        //crosswalk sound code start here
        buzzer_enabled = 1;
        __no_operation();
    case 2: //red light
        pin_on(RED_PORT, RED_BIT);
        pin_off(YELLOW_PORT, YELLOW_BIT);
        pin_off(GREEN_PORT, GREEN_BIT);
        break;

    default: //flashing red (something broken....)
        break;
    }
}

void handleTasks()
{

    change_lights(current_state); //set lights to what they should be

    if (curr_time_s - lastLightTime >= (delayTimes[current_state]))
    {
        current_state++;
        lastLightTime = curr_time_s;

        if (current_state == 4) //if we are done with the crosswalk
        {
            current_state = 0; //back to green
            buzzer_enabled = 0;
        }

        if (current_state == 3) //state 3 is the crosswalk
        {
            if (sw_1_flag || sw_0_flag) //if crosswalk
            {
                sw_1_flag = 0; //reset walk flag
                sw_0_flag = 0; //reset walk flag
            }
            else
            {
                current_state = 0; //back to green

            }
        }
    }
}

/*
 * ALL SETUP STUFF AND TIMER STUFF PAST THIS POINT...
 */

void setup()
{
    WDTCTL = WDTPW + WDTHOLD; //shouldent be holding the timer, we should make sure we dident crash...
    FLL_CTL0 |= XCAP18PF;
    P5DIR |= 0x02;
    BTCTL = BTSSEL + BT_fCLK2_DIV256; //1MHz/256 = ~244us Interval
    RTCCTL = RTCMODE_1 + RTCTEV_0 + RTCIE; // 1MHz/(128*256) =32 Hz
    IE2 |= BTIE;

//setting up our switches for crosswalk interupts
    P1IE |= BIT0 + BIT1;
    P1IES |= BIT0 + BIT1;
    P1IFG &= (BIT0 | BIT1);

//Setup our output leds
    GREEN_DIR |= GREEN_BIT;
    RED_DIR |= RED_BIT;
    YELLOW_DIR |= YELLOW_BIT;

    P3DIR |= BIT5;                  //(buzzer)

    __bis_SR_register(GIE); //let the interupts roll
}
int buzz = 0;
void main(void)
{
    setup(); //run once

    __no_operation(); //Breakpoint for debugger

    while (1)
    {
        if (buzzer_enabled == 1)
        {

            if (buzz == 1)
            {
                pin_on(P3OUT, BIT5);
                buzz = 0;
            }
            else
            {
                pin_off(P3OUT, BIT5);
                buzz = 1;
            }
        }
        handleTasks();
        __no_operation(); //breakpoint for debugger
    }
}

/**************
 * this interupt is to get us a freerunning clock (curr_time_s)
 **************/

#pragma vector=BASICTIMER_VECTOR
__interrupt void basic_timer_ISR(void)
{
    fract_curr_time++;

    if (fract_curr_time >= FRACTS_PER_SEC)
    {
        fract_curr_time = 0;
        curr_time_s++;
    }
}
/**************
 * this interupt gets when the user pushes the walk button
 **************/
#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR()
{
    if ((SW0) == 0)
    {
        sw_0_flag = 1;
    }
    if ((SW1) == 0)
    {
        sw_1_flag = 1;
    }
    P1IFG &= ~BIT0;
    P1IFG &= ~BIT1;
}
