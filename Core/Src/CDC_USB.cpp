/*
 * CDC_USB.cpp
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#include "usbd_def.h"
#include "usbd_cdc_if.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "CDC_USB.h"
#include "Token.h"

#include "CommandTokens.h"
#include "Commands.h"


#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)


struct USB_RxPacket {
	USB_RxPacket():len(0){}
	USB_RxPacket(uint8_t * buf, uint32_t *len)
		:len((*len < 64) ? *len : 64){
		memcpy(buffer, buf, this->len);
	}
	uint8_t buffer[64];
	uint32_t len;
};



osMessageQueueId_t usbRxQueueHandle;
osMessageQueueId_t commandQueueHandle;

const osMessageQueueAttr_t usbQueue_attributes = {
  .name = "usbQueue"
};

const osMessageQueueAttr_t commandQueue_attributes = {
  .name = "commandQueue"
};

const uint32_t STACK_SIZE=2048;
uint8_t UsbThreadStack[STACK_SIZE];
static StaticTask_t usbThreadcb;

const osThreadAttr_t  	usbThread_attributes = {
  .name = "usbThread",
  .cb_mem = (void*) &usbThreadcb,
  .cb_size = sizeof(usbThreadcb),
  .stack_mem =  UsbThreadStack,
  .stack_size = STACK_SIZE

};



extern USBD_HandleTypeDef hUsbDeviceFS;

extern "C" int _write(int file, char *ptr, int len);
void usbTask(void *argument);

int _write(int file, char *ptr, int len){
    UNUSED(file);
    CDC_Transmit_FS((uint8_t *)ptr, len);
    return len;
}


void initCDCUSB() {
	usbRxQueueHandle = osMessageQueueNew(5, sizeof(USB_RxPacket), &usbQueue_attributes);
	commandQueueHandle = osMessageQueueNew(5, sizeof(Command), &commandQueue_attributes);
	memset(UsbThreadStack, 0xFF, STACK_SIZE);
	osThreadNew(&usbTask,nullptr,&usbThread_attributes );
}

void CDC_Receive_data(uint8_t* buf, uint32_t *len) {
	USB_RxPacket rx_packet(buf, len);
	osMessageQueuePut(usbRxQueueHandle, &rx_packet, 0U, 0U);
}

inline
bool  isEndOfComand(uint8_t byte){
	return byte == '\n' || byte == '\r';
}

inline
bool  isEndOfToken(uint8_t byte){
	return byte == '\t' || byte == ' ';
}

void usbTask(void *argument){
	USB_RxPacket rx_packet;
	CommandTokens cmdTokens;
	Token token;

	while(true) {
		if (osMessageQueueGet(usbRxQueueHandle, &rx_packet, NULL, osWaitForever) == osOK) {
			for (uint32_t i=0; i < rx_packet.len; i++){
				auto newByte = rx_packet.buffer[i];
				if (!isEndOfComand(newByte)){
					if (isEndOfToken(newByte)){
						if (token.size() > 0 ){
							cmdTokens.addToken(token);
							token.reset();
						}
					} else {
						token.addByte(newByte);
					}
				} else {
					if (token.size() > 0 ){
						cmdTokens.addToken(token);
						token.reset();
					}
					Command command = createCommand(cmdTokens);
					cmdTokens.free();
					osMessageQueuePut(commandQueueHandle, &command, 0,0);
				}
			}

		}
	}
}

