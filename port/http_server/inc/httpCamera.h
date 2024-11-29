
#ifndef __HTTPHANDLER_H
#define __HTTPHANDLER_H

#include <stdint.h>
#include "httpParser.h"

//#define DATA_BUF_SIZE 2048
uint8_t * set_resolution(uint8_t * uri);
uint16_t http_camera_capture(uint8_t * res, uint8_t *buf, uint32_t *len);

#endif //__HTTPHANDLER_H
