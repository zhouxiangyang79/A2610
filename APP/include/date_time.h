/*
 * date_time.h
 *
 *  Created on: 2023年6月24日
 *      Author: wx030
 */

#ifndef INCLUDE_DATE_TIME_H_
#define INCLUDE_DATE_TIME_H_

/* Includes ------------------------------------------------------*/
#include <stdint.h>
#include <time.h>
/* Private define ------------------------------------------------*/
/* Private typedef -----------------------------------------------*/
// typedef struct
// {
//     uint16_t Year;
//     uint16_t Mon;
//     uint16_t Day;
//     uint16_t Hour;
//     uint16_t Min;
//     uint16_t Sec;
// } ST_DATE_TIME, *PST_DATE_TIME;
/* Private constants ---------------------------------------------*/
/* Private variables ---------------------------------------------*/
/* Private function prototypes -----------------------------------*/
/* Public constants ----------------------------------------------*/
/* Public variables ----------------------------------------------*/

/// @brief
/// @param stDateTime
void Time_offset(struct tm* stDateTime);
struct tm gettimebyoffset(void);
void datetime_init(void);
#endif /* INCLUDE_DATE_TIME_H_ */
