
#include "port_common.h"
#include "httpParser.h"
#include "httpCamera.h"
#include "arducam.h"
#include "pico/malloc.h"

extern uint8_t *pHTTP_RX;
extern uint8_t *pHTTP_TX;

uint8_t * set_resolution(uint8_t * uri)
{
    uint8_t *param;
    uint8_t str_size;

    if((param = get_http_param_value((char *)uri, "resolution")))
	{
		printf("found resolution in uri: %d\r\n", atoi((char *)param));
        return param;
        /*
        memset(value->module_name, 0x00, 25);
		if((str_size = strlen((char*)param)) > 24) str_size = 24; // exception handling
		memcpy(value->module_name, param, str_size);
        */
	}
}

uint16_t http_camera_capture(uint8_t * res, uint8_t *buf, uint32_t *len)
{
    // To do
	uint8_t ret = 1;
  	//uint8_t *frame_data = NULL;
	uint32_t frame_size = 0;
  	int resolution_value = atoi((char *)res);
  	arducam.setJpegSize(resolution_value);
	// Send the captured image as a JPEG response
	capture(buf, &frame_size);
	printf("camera frame size = %d\r\n",frame_size);
	
	*len = frame_size;
	printf("camera frame size = %u\r\n",*len);
	return ret;
}
