/*
 * CDC_USB.h
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#ifndef INC_CDC_USB_H_
#define INC_CDC_USB_H_


#ifdef __cplusplus
 extern "C" {
#endif

void CDC_Receive_data(uint8_t* Buf, uint32_t *Len);

#ifdef __cplusplus
 }
#endif

void initCDCUSB();


#endif /* INC_CDC_USB_H_ */
