#ifndef __INCLUDES_H
#define __INCLUDES_H

// Define CPU Frequency
// This must be defined, if __delay_ms() or 
// __delay_us() functions are used in the code
#define _XTAL_FREQ   10000000  

#include <htc.h>
#include "I2C.h"
#include "LCD.h"
#include "DS1307.h" 

#endif