/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _WEB_PAGE_H_
#define _WEB_PAGE_H_

#define index_page "<!DOCTYPE html>"\
                    "<html lang=\"en\">"\
                    "<head>"\
                        "<meta charset=\"UTF-8\">"\
                        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"\
                        "<title>Camera Capture</title>"\
                        "<style>"\
                            "body { font-family: Arial, sans-serif; text-align: center; margin: 50px; }"\
                            "button { padding: 10px 20px; font-size: 16px; cursor: pointer; }"\
                            "img { margin-top: 20px; max-width: 100%; height: auto; border: 2px solid #ddd; }"\
                            "#loading { display: none; font-size: 14px; color: gray; }"\
                        "</style>"\
                    "</head>"\
                    "<body>"\
                        "<h1>Capture an Image</h1>"\
                        "<label for=\"resolution\">Select Resolution: </label>"\
                        "<select id=\"resolution\">"\
                            "<option value=\"0\">160x120</option>"\
                            "<option value=\"1\">176x144</option>"\
                            "<option value=\"2\">320x240</option>"\
                            "<option value=\"3\">352x288</option>"\
                            "<option value=\"4\">640x240</option>"\
                            "<option value=\"5\">800x600</option>"\
                            "<option value=\"6\">1024x768</option>"\
                            "<option value=\"7\">1280x1024</option>"\
                            "<option value=\"8\">1600x1200</option>"\
                        "</select>"\
                        "<br><br>"\
                        "<button onclick=\"captureImage()\">Capture Image</button>"\
                        "<p id=\"loading\">Capturing image...</p>"\
                        "<br><br>"\
                        "<img id=\"cameraImage\" src=\"\" alt=\"Captured Image will appear here\" />"\
                        "<script>"\
                            "function captureImage() {"\
                                "const resolution = document.getElementById('resolution').value;"\
                                "const loading = document.getElementById('loading');"\
                                "const img = document.getElementById('cameraImage');"\
                                "loading.style.display = 'inline';"\
                                "img.style.display = 'none';"\
                                "const xhr = new XMLHttpRequest();"\
                                "xhr.open('POST', '/capture.cgi', true);"\
                                "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"\
                                "xhr.responseType = 'blob';"\
                                "xhr.onload = function() {"\
                                    "if (xhr.status === 200) {"\
                                        "const blob = xhr.response;"\
                                        "const url = URL.createObjectURL(blob);"\
                                        "img.src = url;"\
                                        "img.style.display = 'block';"\
                                        "loading.style.display = 'none';"\
                                    "} else {"\
                                        "alert('Failed to capture image.');"\
                                        "loading.style.display = 'none';"\
                                    "}"\
                                "};"\
                                "xhr.onerror = function() {"\
                                    "alert('An error occurred while capturing the image.');"\
                                    "loading.style.display = 'none';"\
                                "};"\
                                "xhr.send('resolution=' + encodeURIComponent(resolution));"\
                            "}"\
                        "</script>"\
                    "</body>"\
                    "</html>"



#define stream_page "HTTP/1.1 200 OK\r\n"\
                    "Content-Type: text/html\r\n"\
                    "Connection: keep-alive\r\n"\
                    "\r\n"\
                    "<!DOCTYPE html>"\
                    "<html lang=\"en\">"\
                    "<head><title>Stream</title></head>"\
                    "<body><h1>Live Stream</h1></body>"\
                    "</html>"

#endif /* _WEB_PAGE_H_ */
