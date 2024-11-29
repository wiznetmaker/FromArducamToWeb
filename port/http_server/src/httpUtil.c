/**
 * @file	httpUtil.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpUtil.h"
#include "socket.h"
#include "httpCamera.h"
#include "arducam.h"

extern uint8_t camera_flag;
extern uint8_t * camera_buf;

uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;

	if(predefined_get_cgi_processor(uri_name, buf, &len))
	{
		;
	}
	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
	{
		// To do
		;
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint32_t len = 0;
	uint8_t val = 0;
	

	if(predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
	{
		;
	}
	/*else if(strcmp((const char *)uri_name, "capture.cgi") == 0)
	{
		//device_ip = set_basic_config_setting(p_http_request->URI);
		printf("I AM HERE\r\n");
		resolution = set_resolution(p_http_request->URI);
		printf("Selected resolution: %d\r\n", resolution);
		//make_cgi_basic_config_response_page(5, device_ip, buf, &len);

		//ret = HTTP_RESET;

	}*/
	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
	{
		// To do
		val = 1;
		len = sprintf((char *)buf, "%d", val);
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
{
	;
}

uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint32_t * len)
{
	uint8_t ret = 1;	// ret = 1 means 'uri_name' matched
	uint8_t * resolution;

	if(strcmp((const char *)uri_name, "capture.cgi") == 0)
	{
		camera_flag = 1;
		resolution = set_resolution(uri);
		printf("Selected resolution: %d\r\n", atoi((char *)resolution));
		
		camera_buf = (uint8_t *)malloc(1024 * 100);
		if (camera_buf == NULL) {
			printf("Error: Failed to allocate memory for camera buffer.\n");
			return 0; // Failure
		}

		//make_cgi_basic_config_response_page(5, device_ip, buf, &len);
		if (http_camera_capture(resolution, camera_buf, len)) {
			printf("Capture successful. Frame size: %d bytes.\n", *len);
		} else {
			printf("Capture failed.\n");
		}
			//ret = HTTP_RESET;

	}

	return ret;
}
