/*
 * Move.cpp
 *
 *  Created on: May 24, 2025
 *      Author: paolo
 */
#include <string.h>
#include "main.h"
#include "Move.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern Move move;


void Move::init(){
	HAL_TIM_Base_Start(&htim2);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
}
void Move::on(){
	if (!(TIM3->CR1 & TIM_CR1_CEN)){
		HAL_TIM_Base_Start(&htim3);
		TIM3->CCR1 = speed;
		TIM3->CCR2 = speed;
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	}

}
void Move::off(){
	if (TIM3->CR1 & TIM_CR1_CEN){
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		HAL_TIM_Base_Stop(&htim3);
	}
}

void Move::setSpeed(uint32_t speed) {
	if (speed > 100){
		speed=100;
	}
	this->speed = speed;
	if (TIM3->CR1 & TIM_CR1_CEN){
		TIM3->CCR1 = speed;
		TIM3->CCR2 = speed;
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
/*            if (move.motor1 < HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3)){
            	move.motor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3) - move.motor1;
            } else {
            	move.motor1 = TIM2->ARR - move.motor1+HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
            }
  */
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
        {
        	memmove(&move.motor2[1], &move.motor2[0], 9 * sizeof(uint32_t));
        	move.motor2[0] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
        }
    }
}
