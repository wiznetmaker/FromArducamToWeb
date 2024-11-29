# Arducam Webserver on W5500

This repository demonstrates a **proof-of-concept (POC)** project that integrates an **Arducam SPI Camera Module** with a **WIZnet W5500 Ethernet module** to capture high-resolution images and serve them via a webserver. The implementation focuses on efficient memory management and seamless transmission of large image data in an embedded environment.

---

## Features

- Capture high-resolution images using **Arducam SPI Mini**.
- Serve images via a lightweight **webserver** running on W5500.
- Adjustable **resolution settings** accessible from the web interface.
- Custom HTTP response handling for efficient image transmission.
- Optimized for handling large image data with a 100 KB buffer.

---

## Project Structure

The repository is organized as follows:

- **`Examples/`**: Contains the main application code integrating the camera and webserver.
- **`Libraries/`**: Includes the Arducam library and WIZnet ioLibrary for camera and networking functionality.
- **`Port/`**: Holds customized code for the project, including changes to SPI handling, buffer management, and camera capture logic.

---

## Arducam Integration

The **Arducam library** was sourced from the official [Arducam RPI-Pico-Cam GitHub Repository](https://github.com/ArduCAM/RPI-Pico-Cam). Key modifications include:

1. Updating SPI pin definitions to avoid conflicts with the WIZnet ioLibrary.
2. Adding a custom `capture` function to pass buffer and size as parameters.
3. Validating the JPEG end marker (`FF D9`) to ensure image integrity.

---

## Webserver Implementation

The webserver code is customized to handle large image transmissions:

- A **custom function** was implemented to send the HTTP header and image buffer together for better efficiency.
- The image buffer is divided into **8 KB chunks** and transmitted sequentially to avoid overloading the W5500's buffer.
- CGI processor code was updated to handle camera-specific POST requests.

---

## Memory Management

- A **global buffer of 100 KB** is allocated to store the captured image.
- Memory allocation and usage were refined through trial and error to ensure stability and performance.
- Both **TX** and **RX** buffers in the W5500 were set to **16 KB** for optimized data handling.

---

## Results

The project demonstrates seamless integration of Arducam and W5500 for serving high-resolution images over a webserver. The results can be seen in the accompanying [video demonstration](https://www.youtube.com/watch?v=vg2ZzavHhbI).

---

## Future Improvements

Planned enhancements for the next iteration include:

1. **Dual-Core Operation:**  
   - Using one core for camera operations and the other for networking.
2. **Advanced Camera Settings:**  
   - Adding features like light mode, contrast adjustment, and image compression.
3. **Live Streaming:**  
   - Implementing real-time video streaming via the webserver.

---

## Acknowledgments

Arducam Team for their open-source library: [Arducam RPI-Pico-Cam](https://github.com/ArduCAM/RPI-Pico-Cam).

WIZnet for their ioLibrary and hardware support: [WIZnet-PICO-C GitHub Repository](https://github.com/WIZnet-ioNIC/WIZnet-PICO-C)