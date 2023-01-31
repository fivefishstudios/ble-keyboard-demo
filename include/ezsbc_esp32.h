/**********************************

EZSBC ESP32-01 Dev Board Pin Outs

***********************************/

/*  
These are the other PDIP pins mapping on the ESP32-01 EZSBC Dev Board: 
PDIP3 GND
PDIP4 Vin
PDIP9 3.3V
PDIP13 NOTUSED
PDIP17 RESET/EN
PDIP18 3.3V
PDIP19 Vin
PDIP20 GND
PDIP21 GND
PDIP22 Vin
PDIP23 3.3V
PDIP32 3.3V
PDIP37 Vin
*/

// GPIO PINS
// All GPIOs can be configured as interrupts.
// All OUTPUT-capable pins can be configured to generate PWM signals.
// For PWM signal, define these parameters: frequency, duty cycle, PWM channel, GPIO output pin
#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
// #define D6 6   // not recommended, used by SPI Flash
// #define D7 7   // not recommended, used by SPI Flash
// #define D8 8   // not recommended, used by SPI Flash
// #define D9 9   // not recommended, used by SPI Flash
// #define D10 10 // not recommended, used by SPI Flash
// #define D11 11 // not recommended, used by SPI Flash
#define D12 12
#define D13 13
#define D14 14
#define D15 15
#define D16 16
#define D17 17
#define D18 18
#define D19 19
#define D20 20
#define D21 21
#define D22 22
#define D23 23
#define D24 24
#define D25 25
#define D26 26
#define D27 27
#define D28 28
#define D29 29
#define D30 30
#define D31 31
#define D32 32
#define D33 33
#define D34 34 // INPUT only
#define D35 35 // INPUT only
#define D36 36 // INPUT only
#define D37 37 // not available
#define D38 38 // not available
#define D39 39 // INPUT only

// BUILT-IN RGB LEDs, specific only for EZSBC ESP32-01 Board
#define LED1RED D16
#define LED1GREEN D17
#define LED1BLUE D18
#define LED2 D19
#define LED2BLUE D19 // only blue LED for LED2 is user accessible

// TOUCH PINS (T)
// The ESP32 has 10 internal capacitive touch sensors.
// These can sense variations in anything that holds an electrical charge, like the human skin.
// So they can detect variations induced when touching the GPIOs with a finger.
// These pins can be easily integrated into capacitive pads, and replace mechanical buttons.
// The capacitive touch pins can also be used to wake up the ESP32 from deep sleep.
#define TOUCH0 D4
#define TOUCH1 D0
#define TOUCH2 D2
#define TOUCH3 D15
#define TOUCH4 D13
#define TOUCH5 D12
#define TOUCH6 D14
#define TOUCH7 D27
#define TOUCH8 D33
#define TOUCH9 D32

// Analog to Digital Converter (ADC)
// The ESP32 has 18 x 12 bits ADC input channels (while the ESP8266 only has 1x 10 bits ADC). 
// 12 bit resolution. Analog readings from 0 to 4095, 0 corresponds to 0V and 4095 to 3.3V.
// You also have the ability to set the resolution of your channels on the code, as well as the ADC range.
// Note: ADC pins don’t have a linear behavior. Can't distinguish between 0 and 0.1V, or between 3.2 and 3.3V.
#define ADC1_CH0 D36
#define ADC1_CH1 D37 // not available
#define ADC1_CH2 D38 // not available
#define ADC1_CH3 D39
#define ADC1_CH4 D32
#define ADC1_CH5 D33
#define ADC1_CH6 D34
#define ADC1_CH7 D35

#define ADC2_CH0 D4
#define ADC2_CH1 D0
#define ADC2_CH2 D2
#define ADC2_CH3 D15
#define ADC2_CH4 D13
#define ADC2_CH5 D12
#define ADC2_CH6 D14
#define ADC2_CH7 D27
#define ADC2_CH8 D25
#define ADC2_CH9 D26

// Digital to Analog Converter (DAC)
// There are 2 x 8 bits DAC channels on the ESP32/
#define DAC1 D25
#define DAC2 D26

// UART SERIAL PROTOCOL
#define TX0 D1
#define RX0 D3
#define TX2 D17
#define RX2 D16

#define UART0_TX D1
#define UART0_RX D3
#define UART2_TX D17
#define UART2_RX D16

// SPI PROTOCOL
#define VSPI_MOSI D23
#define VSPI_MISO D19
#define VSPI_CLK D18
#define VSPI_CS D5

#define HSPI_MOSI D13
#define HSPI_MISO D12
#define HSPI_CLK D14
#define HSPI_CS D15

// I2C (WIRE) PROTOCOL
// External pull-up resistors to 3.3V are required for the bus to behave correctly. 
// 2K2 resistor recommended for correct timing with the specified capacative load. 10K works for breadboard projects.
#define SDA D21
#define SCL D22

#define I2C_SDA D21
#define I2C_SCL D22

#define WIRE_SDA D21
#define WIRE_SCL D22

// RTC GPIO
// The GPIOs routed to the RTC low-power subsystem can be used when the ESP32 is in deep sleep.
// RTC GPIOs can be used to wake up the ESP32 from deep sleep when the Ultra Low Power (ULP) co-processor is running.
#define RTC_GPIO0 D36
#define RTC_GPIO3 D39
#define RTC_GPIO4 D34
#define RTC_GPIO5 D35
#define RTC_GPIO6 D25
#define RTC_GPIO7 D26
#define RTC_GPIO8 D33
#define RTC_GPIO9 D32
#define RTC_GPIO10 D4
#define RTC_GPIO11 D0
#define RTC_GPIO12 D2
#define RTC_GPIO13 D15
#define RTC_GPIO14 D13
#define RTC_GPIO15 D12
#define RTC_GPIO16 D14
#define RTC_GPIO17 D27
