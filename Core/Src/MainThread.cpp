/*
 * MainThread.cpp
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#include <stdio.h>
#include <FreeRTOS.h>
#include <cmsis_os2.h>
#include "MainThread.h"
#include "Commands.h"
#include "CDC_USB.h"
#include "Move.h"

extern osMessageQueueId_t commandQueueHandle;

Move move;

void mainThread() {
	move.init();
	Command command;
	initCDCUSB();

	while(true){

	  osStatus_t status = osMessageQueueGet(commandQueueHandle,&command, nullptr, 0 );
	  if (status == osOK){
		  switch (command.commandType){
		  case CommandType::MOVE:
			  if (command.move.on)
				  move.on();
			  else
				  move.off();
			  break;
		  case CommandType::SPEED:
			  move.setSpeed(command.speed.velocity);
			  break;
		  case CommandType::MOTOR:
		  	  	  {
			  	  const uint32_t * value = command.motor.info;
			  	  uint32_t last = value[0];
			  	  for(int i=1; i < 10; i++){
			  		  float freq = 0;
			  		  if (last > value[i]){
			  			  freq = 1000000.0/(last-value[i]);
			  		  } else {
			  			  freq = 1000000.0/(0xFFFFFFFF-last + value[i]+1);
			  		  }
			  		  printf("%f ", freq);
			  		  last = value[i];
			  	  }
			  	  printf("\r\n");
			  	  }
			  	  {
					  const uint32_t * value = command.motor.info;
					  uint32_t last = value[0];
					  for(int i=1; i < 10; i++){
						  uint32_t count=0;
						  if (last > value[i]){
							  count = last-value[i];
						  } else {
							  count = (0xFFFFFFFF-last + value[i]+1);
						  }
						  printf("%lu ", count);
						  last = value[i];
					  }
				  printf("\r\n");
				  }


			  break;
		  case CommandType::NOTHING:
			  printf("NO command\r\n");
		  }
	  }

	osDelay(1000/ portTICK_PERIOD_MS);
	}
}
