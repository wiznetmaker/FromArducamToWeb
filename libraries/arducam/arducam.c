
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "arducam.h"
#include "ov2640.h"
uint8_t cameraCommand = 0;

// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
       cameraCommand = uart_getc(UART_ID);
       // Can we send it back?
       if (uart_is_writable(UART_ID)) {
           uart_putc(UART_ID, cameraCommand);
       }
    }
}
// Define sensor slave address
void picoSystemInit(){
    stdio_init_all();
    // This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL)
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
    // Make the I2C pins available to picotool
    bi_decl( bi_2pins_with_func(PIN_SDA, PIN_SCL, GPIO_FUNC_I2C));
     // This example will use SPI0 at 0.5MHz.
    spi_init(SPI_PORT, 8 * 1000*1000);
    gpio_set_function(APIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(APIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(APIN_MOSI, GPIO_FUNC_SPI);
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(APIN_CS);
    gpio_set_dir(APIN_CS, GPIO_OUT);
    gpio_put(APIN_CS, 1);
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);
     // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
}

//int i2c_write_blocking(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop);

int rdSensorReg8_8(uint8_t regID, uint8_t* regDat ){
    i2c_write_blocking(I2C_PORT, arducam.slave_address, &regID, 1, true );
    i2c_read_blocking(I2C_PORT, arducam.slave_address, regDat,  1, false );
}
 int wrSensorReg8_8(uint8_t regID, uint8_t regDat ){
    uint8_t buf[2];
    buf[0] = regID;
    buf[1] = regDat;
    i2c_write_blocking(I2C_PORT, arducam.slave_address, buf,  2, true );
}

 void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(APIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

 void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(APIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}

 void write_reg(uint8_t address, uint8_t value){
    uint8_t buf[2];
    buf[0] = address|WRITE_BIT ;  // remove read bit as this is a write
    buf[1] = value;
    cs_select();
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect();
    sleep_ms(10); 
}

 uint8_t read_reg(uint8_t address)
{
    uint8_t value = 0;
	address = address& 0x7f;
     cs_select();
	spi_write_blocking(SPI_PORT, &address, 1);
	sleep_ms(10);
    spi_read_blocking(SPI_PORT, 0, &value, 1);
	cs_deselect();
    sleep_ms(10);
	return value;
}

int wrSensorRegs8_8(const struct sensor_reg reglist[])
{
  int err = 0;
  unsigned int reg_addr = 0;
  unsigned int reg_val = 0;
  const struct sensor_reg *next = reglist;
  while ((reg_addr != 0xff) | (reg_val != 0xff))
  {
    reg_addr =next->reg;
    reg_val = next->val;
    err = wrSensorReg8_8(reg_addr, reg_val);
    sleep_ms(10);
    next++;
  }
  return err;
}

unsigned char read_fifo(void)
{
	unsigned char data;
	data = read_reg(SINGLE_FIFO_READ);
	return data;
}
void set_fifo_burst()
{
    uint8_t value;
    spi_read_blocking(SPI_PORT, BURST_FIFO_READ, &value, 1);
}


void flush_fifo(void)
{
	write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void start_capture(void)
{
	write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);
}

void clear_fifo_flag(void )
{
	write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

unsigned int read_fifo_length()
{
    unsigned int len1,len2,len3,len=0;
    len1 = read_reg(FIFO_SIZE1);
  len2 = read_reg(FIFO_SIZE2);
  len3 = read_reg(FIFO_SIZE3) & 0x7f;
  len = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return len;	
}

//Set corresponding bit  
void set_bit(unsigned char addr, unsigned char bit)
{
	unsigned char temp;
	temp = read_reg(addr);
	write_reg(addr, temp | bit);
}
//Clear corresponding bit 
void clear_bit(unsigned char addr, unsigned char bit)
{
	unsigned char temp;
	temp = read_reg(addr);
	write_reg(addr, temp & (~bit));
}

//Get corresponding bit status
unsigned char get_bit(unsigned char addr, unsigned char bit)
{
  unsigned char temp;
  temp = read_reg(addr);
  temp = temp & bit;
  return temp;
}

void OV2640_set_JPEG_size(unsigned char size)
{
	switch(size)
	{
		case res_160x120:
			wrSensorRegs8_8(OV2640_160x120_JPEG);
			break;
		case res_176x144:
			wrSensorRegs8_8(OV2640_176x144_JPEG);
			break;
		case res_320x240:
			wrSensorRegs8_8(OV2640_320x240_JPEG);
			break;
		case res_352x288:
	  	wrSensorRegs8_8(OV2640_352x288_JPEG);
			break;
		case res_640x480:
			wrSensorRegs8_8(OV2640_640x480_JPEG);
			break;
		case res_800x600:
			wrSensorRegs8_8(OV2640_800x600_JPEG);
			break;
		case res_1024x768:
			wrSensorRegs8_8(OV2640_1024x768_JPEG);
			break;
		case res_1280x1024:
			wrSensorRegs8_8(OV2640_1280x1024_JPEG);
			break;
		case res_1600x1200:
			wrSensorRegs8_8(OV2640_1600x1200_JPEG);
			break;
		default:
			wrSensorRegs8_8(OV2640_320x240_JPEG);
			break;
	}
}

void ov2640Init(){
    wrSensorReg8_8(0xff, 0x01);
    wrSensorReg8_8(0x12, 0x80);
    wrSensorRegs8_8(OV2640_JPEG_INIT);
    wrSensorRegs8_8(OV2640_YUV422);
    wrSensorRegs8_8(OV2640_JPEG);
    wrSensorReg8_8(0xff, 0x01);
    wrSensorReg8_8(0x15, 0x00);
    wrSensorRegs8_8(OV2640_320x240_JPEG);
}

void singleCapture(void){
   int i , count;
   uint8_t value[1024*40];
   //Flush the FIFO
   flush_fifo();
   //Start capture
   start_capture(); 
   while(!get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)){;}
   int length = read_fifo_length();
   count = length;
   i = 0 ;
   cs_select();
   set_fifo_burst();//Set fifo burst mode
   spi_read_blocking(SPI_PORT, BURST_FIFO_READ,value, length);
   //uart_write_blocking(UART_ID, value, length);
	 count = 0;
	 cs_deselect();
}

void capture(uint8_t *imageDat, uint32_t *len) {
    
    flush_fifo();
    clear_fifo_flag();
    // Flush the FIFO
    
    // Start capture
    start_capture(); 
    
    // Wait for capture to complete
    while (!get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));

    // Get the length of the data captured
    *len = read_fifo_length();  // Update the length by reference
    printf("fifo length = %u\r\n",*len);
    
    // Check if the data length is too large for the buffer
    if (*len > 0x5FFFF) {
        printf("Data size exceeds buffer size!\n");
        return;
    }

    // Select the chip and set it to burst read mode
    cs_select();
    set_fifo_burst();

    // Directly read the data into the imageDat buffer
    spi_read_blocking(SPI_PORT, BURST_FIFO_READ, imageDat, *len);

    // Deselect the chip
    cs_deselect();
    printf("Capture done, data length: %d\n", *len);

    uint32_t valid_size = 0;
	for (uint32_t i = 0; i < *len - 1; i++) {
		if (imageDat[i] == 0xFF && imageDat[i + 1] == 0xD9) {
			valid_size = i + 2;  // Set the valid size to end after FF D9
			printf("Found JPEG end marker (FF D9) at position: %u\n", i);
                  break;
		}
	}
    
	if (valid_size == 0) {
		printf("JPEG end marker not found. Sending full data as fallback.\n");
		valid_size = *len;  // Fallback to sending full data if no marker found
	}

    *len = valid_size;
   
    printf("First two bytes: %02X %02X\n", imageDat[0], imageDat[1]);  // Should be FF D8
	printf("Last two bytes: %02X %02X\n", imageDat[valid_size-2], imageDat[valid_size-1]);  // Should be FF D9
    
}

uint8_t spiBusDetect(void){
    write_reg(0x00, 0x55);
    if(read_reg(0x00) == 0x55){
        printf("SPI bus normal");
        return 0;
    }else{
        printf("SPI bus error\r\n");
        return 1;
    }      
}
uint8_t ov2640Probe(){
    uint8_t id_H,id_L;
    rdSensorReg8_8(0x0A,&id_H);
    rdSensorReg8_8(0x0B,&id_L);
    if(id_H == 0x26 && (id_L == 0x40 ||id_L == 0x41 || id_L == 0x42)){
        printf("ov2640 detected\r\n");
        return 0;
    }else{
        printf("Can't find ov2640 sensor\r\n");
        return 1;
    }
}


struct camera_operate arducam = {
    .slave_address = 0x30,
    .systemInit  = picoSystemInit,
    .busDetect   = spiBusDetect,
    .cameraProbe = ov2640Probe,
    .cameraInit  = ov2640Init,
    .setJpegSize = OV2640_set_JPEG_size,
};
