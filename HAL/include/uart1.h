/*
 * uart1.h
 *
 *  Created on: 2024年11月8日
 *      Author: yyz
 */

#ifndef INCLUDE_UART1_H_
#define INCLUDE_UART1_H_

void UART1_INIT(void);
void UART1_GETDATA(void);
void parsingdata(void);
void UART1_CMDON(void);
void UART1_CMDOFF(void);
void UART1_CMDPUMPOIL(void);
void cleardata(void);
uint8_t checksum(uint8_t *buff, uint8_t length);
#endif /* INCLUDE_UART1_H_ */
