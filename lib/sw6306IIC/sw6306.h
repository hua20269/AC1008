#ifdef _SW6306_H_
#define _SW6306_H_

#include <iic.h>
#define SW6208_address 0x3C // 设备地址
// define MAX_AES_BUFFER_SIZ 1
//  ntc相关参数
#define BX 3435 // B值
#define T25 298.15
#define R25 10000
#ifndef uint16_t
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
#endif

#endif