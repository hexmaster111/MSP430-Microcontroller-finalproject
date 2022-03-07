/*
 * helpers.h
 *
 *  Created on: Dec 2, 2021
 *      Author: 5050
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#define pin_on(OUT_PORT, OUT_BIT)  (OUT_PORT |= OUT_BIT)
#define pin_off(OUT_PORT, OUT_BIT) (OUT_PORT &= ~OUT_BIT)

#define SW0 BIT0&P1IN
#define SW1 BIT1&P1IN

#define RED_BIT BIT1
#define RED_PORT P5OUT
#define RED_DIR P5DIR

#define GREEN_BIT BIT2
#define GREEN_PORT P2OUT
#define GREEN_DIR P2DIR

#define YELLOW_BIT BIT1
#define YELLOW_PORT P2OUT
#define YELLOW_DIR P2DIR




#endif /* HELPERS_H_ */
