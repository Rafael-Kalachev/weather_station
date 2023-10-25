/// @file my-project.c
#include <stdio.h>
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/i2c.h"
#include "libopencm3/stm32/usart.h"

#define I2C1_CLCK_MHZ					(1000000UL)		/// Used in calculation of the I2C1 timing
#define I2C1_SCL_PIN					(GPIO6)			/// Pin definition for SCL of I2C1
#define I2C1_SDA_PIN					(GPIO9)			/// Pin definition for SDA of I2C1
#define TEMP_SENS_ADDR					(0xEE)			/// Address of the temperature sensor
#define TEMP_REG						(0x2E)			
#define PRES_REG			 			(0X34)

#define USART_PORT						(USART1)
#define USART_BAUDRATE					(115200)
#define USART1_TX_PIN					(GPIO9)			
#define USART1_RX_PIN					(GPIO10)	
#define USART_WORD_LEN					(8)			

/**
 * Delay
*/
static void delay(void)
{
	for(int i = 0; i < 250000; i++);
}

/**
 * @brief Calculates the I2C1 clock in MHZ 
 * Formula: APB1_clock / 1 Mhz
 * @param[out] clock
*/
static uint32_t i2c1_clock(void)
{ 
	uint32_t clock;
	clock = rcc_apb1_frequency / I2C1_CLCK_MHZ;
	return clock;
}

/** 
 * Function to enable the HSE 
*/
static void clock_enable(void)
{
	/// Enable the HSE clock
	rcc_osc_on(RCC_HSE);
	rcc_wait_for_osc_ready(RCC_HSE);

	/// Set the HSE as the system clock source 
	rcc_set_sysclk_source(RCC_HSE);
}

/**
 * Function for initializing the I2C1 peripheral
*/
static void i2c1_setup(void)
{
	/// Enable the peripheral clock of the I2C1
	rcc_periph_clock_enable(RCC_I2C1); 
	i2c_reset(I2C1);
	/// Enable the peripheral clock for the GPIO port B
	rcc_periph_clock_enable(RCC_GPIOB);
	/// Setup for the SCL pin (Pin PB6)
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SCL_PIN); 
	/// Alternate fucntion setup (SCL) 
	gpio_set_af(GPIOB, GPIO_AF4, I2C1_SCL_PIN);
	/// Setup for the SDA pin (Pin PB7)
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SDA_PIN);
	/// Alternate function setup (SDA)
	gpio_set_af(GPIOB, GPIO_AF4, I2C1_SDA_PIN);

	/// Disabling the peripheral before configuration to make sure no communication is running
	i2c_peripheral_disable(I2C1);
	/// Set speed setup of I2C1
	i2c_set_speed(I2C1, i2c_speed_sm_100k, i2c1_clock());
	/// Set the address of the slave (Temp. sensor) 
	i2c_set_own_7bit_slave_address(I2C1, TEMP_SENS_ADDR >> 1);   ///< Right shift by 1, because the LSB is for Write/Read
	/// Enable the I2C1 peripheral 
	i2c_peripheral_enable(I2C1);	
}

/** 
 * Function to read the data from the Temperature sensor 
 * The temperature data is send in the form of 16 bits 
 * @param [in] reg - (Possible values TEMP_REG, PRES_REG)
*/
static uint16_t i2c_read(uint8_t reg)
{
	uint16_t data = 0; 

	/// Send start command 
	i2c_send_start(I2C1);
	/// Send the address with write command 
	i2c_send_7bit_address(I2C1, TEMP_SENS_ADDR, I2C_WRITE);
	/// Send the register specifying the measurement that need to be read
	i2c_send_data(I2C1, reg);
	/// Stop the sending of data 
	i2c_send_stop(I2C1);
	
	i2c_send_start(I2C1);
	/// Send the address with read command 
	i2c_send_7bit_address(I2C1, TEMP_SENS_ADDR, I2C_READ);
	/// Recieve the first byte and left shift it by 1 byte to make room for the next byte 
	data = (i2c_get_data(I2C1) << 8); 
	/// Acknowledge the data reception 
	i2c_enable_ack(I2C1);
	/// Recieve the next byte of data 
	data |= i2c_get_data(I2C1);
	/// Stop the recieving of data 
	i2c_disable_ack(I2C1);
	i2c_send_stop(I2C1);

	return data;
}

/** 
 * Initialization of USART module 
*/
static void usart_setup(void)
{
	/// Enable the clock of USART1
	rcc_periph_clock_enable(RCC_USART1);

	/// Configuration of USART1 GPIO pins
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART1_TX_PIN);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART1_RX_PIN);
	// Configuration of Alternate function mode 
	gpio_set_af(GPIOA, GPIO_AF7, USART1_TX_PIN);
	gpio_set_af(GPIOA, GPIO_AF7, USART1_RX_PIN);

	/// Configuration of USART1 parameters
	usart_set_baudrate(USART_PORT, USART_BAUDRATE);
	usart_set_databits(USART_PORT, USART_WORD_LEN);
	usart_set_stopbits(USART_PORT, USART_STOPBITS_1);
	usart_set_mode(USART_PORT, USART_MODE_TX);
	usart_set_parity(USART_PORT, USART_PARITY_NONE);
	usart_set_flow_control(USART_PORT, USART_FLOWCONTROL_NONE);

	usart_enable(USART_PORT);
}

/** 
 * Transfer the data 
*/
static void usart_transfer(uint16_t data)
{
	/// Wait until data is transfered to the shift register 
	while( ! (USART_SR(USART_PORT) & USART_SR_TXE));

	/// Transfer the data 
	usart_send(USART_PORT, data);

	/// Wait until the transmission is complete 
	while( ! (USART_SR(USART_PORT) & USART_SR_TC));
}

int main(void) 
{
	uint16_t temp; 

	/// Enable the clock 
	clock_enable();
	i2c1_setup();
	usart_setup();

	while(1)
	{
		/// Read temperature 
		temp = i2c_read(TEMP_REG);
		usart_transfer((uint16_t)temp);

		delay();
	}

}
