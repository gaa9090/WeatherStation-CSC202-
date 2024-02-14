//*****************************************************************************                             
//*****************************    C Source Code    ***************************
//*****************************************************************************
//
//  DESIGNER NAME:    GWEN ANTONIO, MATT BONFIGLIO, MARK LUSKIEWICZ, 
//                    JETT KOPALEK
//
//       LAB NAME:    FINAL PROJECT
//
//       FILE NAME:   main_final_proj.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program acts as a weather station using PuTTY as a way to retrieve
//    data. The weather channel will have options to view either temperature, 
//    wind speed, and wind direction. The program also uses a keypad to allow 
//    the user to access the weather data once provided with the correct 
//    password. Once the password is correct, the user will be met with a 
//    PuTTY menu allowing the user to type in a number corresponding to the
//    type of data they would like to see. Once an option is selected, the 
//    data will be displayed on the LCD and the PuTTY menu will come back to
//    let the user choose another type of weather data to view. Once
//    an option is selected, the 7 segment display will display the menu number
//    that was selected and the LEDs will light up the binary choice that 
//    represents the menu number. The user may also press SW2 and then enter to 
//    end the program.
//
//    The external peripherals that this program requires is the SparkFun
//    Wind Vane, and the SparkFun Cup Anemometer,
//  
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------
// Required Dragon12 Board support information
//-----------------------------------------------------------
#include <hidef.h>                  // common defines and macros
#include <mc9s12dg256.h>            // derivative information
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"
#include "csc202_lab_support.h"
#include "main_asm.h"               // interface to the assembly module

//-----------------------------------------------------------
// Define symbolic constants used by program
//-----------------------------------------------------------
#define SW2_MASK                0x08
#define LCD_FIRST_LINE_ADDR     0X00
#define LCD_SECOND_LINE_ADDR    0x40
#define LCD_SECOND_MIDDLE_ADDR  0x44
#define LCD_TEMP_SYMB_ADDR      0x4A
#define CHANNEL3_BITMASK        (1 << 3)
#define MENU_TEMP               '1'
#define MENU_WSPEED             '2'
#define MENU_WDIREC             '3'
#define MENU_EXIT               '6'
#define FAHRENHEIT              '1'
#define CELSIUS                 '2'
#define ADC_TEMP                5
#define NINE_FIFTH              1.8
#define TRUE                    1
#define FALSE                   0
#define POUND                   15
#define PASSWORD_SIZE           4
#define BAUD_RATE               9600
#define ROTATION_FACTOR         1.49
#define LCD_MIDDLE_ADDR         0x08
#define LCD_SECOND_LINE_MIDDLE  0x48
#define DEGREE_SYMBOL_ADDR      0x48
#define VANE_PIN                6

//-----------------------------------------------------------
// Define global variable and structures
//-----------------------------------------------------------
static char   passwordDone     = FALSE;
static char   isDone           = FALSE;
static char   g_switch2pressed = FALSE;
static char   isFahrenheit     = TRUE;
static uint16 count            = 9;
static char   degree_sign      = 0xdf;
static uint8  timer            = 1;
static uint8  turn_count       = 0;
static uint8  most_recent_speed = 0;



// -----------------------------------------------------------------------------
// Description
//   This function handles the interrupt for the switches and sets the flag
//   to the corresponding switch pressed.
//
// input parameters:
//   none
//
// output parameters:
//   none
//
// return:
//   none
//
// -----------------------------------------------------------------------------
void interrupt 25 handler()
{
  char switch_val = PIFH;
  char clear_flag = 0;

  if ((switch_val & SW2_MASK) == SW2_MASK)    // SW2
  {
    g_switch2pressed = TRUE;
    clear_flag |= SW2_MASK;
  }

  PIFH = clear_flag;
}



// -----------------------------------------------------------------------------
// Description
//   This function handles the interrupt for the ISR, and activates every 10.24 
//   ms the fucntion is designed to count for 3 seconds in 10.24 ms intervals 
//   and also calculates the most recent wind speed after 3 seconds the 
//   anemometer count resets to allow a new wind speed to be found
//
// input parameters:
//   none
//
// output parameters:
//   none
//
// return:
//   none
//
// -----------------------------------------------------------------------------

void interrupt 7 RSI_handler()
{
  timer++;
 
  if(timer == 233)
    {
      turn_count /= 3;
      most_recent_speed = turn_count;
      turn_count = 0;
      timer = 0;
    }
 
  clear_RTI_flag();
}


// -----------------------------------------------------------------------------
// Description
//   This function checks for and tracks rising edges coming into port t bit 2 
//   and counts them.
//
// input parameters:
//   none
//
// output parameters:
//   none
//
// return:
//   none
//
// -----------------------------------------------------------------------------

void interrupt 9 timer_channel1(void)
{
 
  if((PTT & 0x02) == 0x02)
  {
    turn_count++;
    ms_delay(10);
  }
  TFLG1 = 0x02;
}



// -----------------------------------------------------------------------------
// Description
//   This function prints a given string to the console using the outchar1()
//   function. It iterates through the characters in the string and outputs them
//   one-by-one until the null character is reached.
//
// input parameters:
//   string - a pointer to the string to be printed
//
// output parameters:
//   none
//
// return:
//   none
//
// -----------------------------------------------------------------------------
 void print_statement(sint8 string[70])
{
  int i;
  for (i = 0; string[i] != '\0'; i++)
  {
    outchar1(string[i]);
  }
}



// -----------------------------------------------------------------------------
// DESCRIPTION
//   This function lights up the 7 seg display depending on the menu choice
//   number.
//  
//  INPUT PARAMETERS
//    menu_choice - the integer value of the menu option selected by the user
//
//  OUTPUT PARAMETERS
//    none
//
//  RETURN
//    none
//
// -----------------------------------------------------------------------------
void menu_choice_led(uint8 menu_choice)
{
  led_disable();
  seg7_enable();
  seg7dec(menu_choice, DIG0);
  ms_delay(1000);
  seg7_disable();
  led_enable();
  leds_on(menu_choice);
  ms_delay(1000);
  leds_off();
}
         


// -----------------------------------------------------------------------------
// Description
//   This function reads ADC channel 5 to capture the value of the thermometer.
//   The temperature value is then displayed on the LCD in either celsius
//   or fahrenheit depending on what the user selected.
//
// input parameters:
//   none
//
// output parameters:
//   none
//
// return:
//   none
//
// -----------------------------------------------------------------------------
void display_temperature(void)
{
  uint16 temperature = ad1conv(ADC_TEMP);
 
  if(isFahrenheit) // F
  {
    temperature = (temperature * NINE_FIFTH) + 32;
    clear_lcd();
    set_lcd_addr(LCD_FIRST_LINE_ADDR);
    type_lcd("Temp: ");
    set_lcd_addr(LCD_MIDDLE_ADDR);
    write_int_lcd(temperature);
    set_lcd_addr(DEGREE_SYMBOL_ADDR);
    data8(degree_sign);
    set_lcd_addr(LCD_TEMP_SYMB_ADDR);
    type_lcd("F");
  }
  else // C
  {  
    clear_lcd();
    set_lcd_addr(LCD_FIRST_LINE_ADDR);
    type_lcd("Temp: ");
    set_lcd_addr(LCD_MIDDLE_ADDR);
    write_int_lcd(temperature);
    set_lcd_addr(DEGREE_SYMBOL_ADDR);
    data8(degree_sign);
    set_lcd_addr(LCD_TEMP_SYMB_ADDR);
    type_lcd("C");
  }
}



// -----------------------------------------------------------------------------
// Description
//   This function measures the wind speed of the cup anemometer and then
//   displays the speed in MPH on the LCD.
//
// input parameters:
//   none
//
// output parameters:
//   none
//
// return:
//   none
//
// -----------------------------------------------------------------------------
void display_wind_speed(void)
{
  clear_lcd();
  set_lcd_addr(LCD_FIRST_LINE_ADDR);
  type_lcd("Wind Speed:");
  set_lcd_addr(LCD_SECOND_MIDDLE_ADDR);
  write_int_lcd(most_recent_speed / ROTATION_FACTOR);
  type_lcd("MPH");
}



// -----------------------------------------------------------------------------
// Description
//   This function reads the ADC value of the wind vane and then displays
//   the direction on the LCD.
//
// input parameters:
//   none
//
// output parameters:
//   none
//
// return:
//   none
//
// -----------------------------------------------------------------------------
void display_wind_direction(void)
{
  uint16 vane_value = 0;
  vane_value = ad0conv(VANE_PIN);
  clear_lcd();
  set_lcd_addr(LCD_FIRST_LINE_ADDR);
  type_lcd("Wind Direction: ");
  set_lcd_addr(LCD_SECOND_LINE_ADDR);
  if (vane_value >= 270 && vane_value <= 286)
    {
      type_lcd("North");
    }
    else if (vane_value >= 165 && vane_value <= 185) 
    {
      type_lcd("Northwest");
    }
    else if (vane_value >= 85 && vane_value <= 95)
    {
      type_lcd("West");
    }
    else if (vane_value >= 440 && vane_value <= 460)
    {
      type_lcd("Southwest");
    }
    else if (vane_value >= 760 && vane_value <= 785)
    {
      type_lcd("South");
    }
    else if (vane_value >= 865 && vane_value <= 888)
    {
      type_lcd("Southeast");
    }
    else if (vane_value >= 900 && vane_value <= 947)
    {
      type_lcd("East");
    }
    else if (vane_value >= 600 && vane_value <= 630)
    {
      type_lcd("Northeast");
    }
  else
  {
    set_lcd_addr(LCD_FIRST_LINE_ADDR);
    type_lcd("Wind Direction: ");
    set_lcd_addr(LCD_SECOND_LINE_ADDR);
    type_lcd("Not Detected.");
  }
}



void main(void)
{
  // Declaration of local variables
  uint8 response_choice;
  uint8 menu_choice = 0;
  uint8 other_choice = 0;
  uint8 temp_choice = 0;
  uint8 character = 0;
  uint8 old_character = 0;
  uint8 index;
  uint8 valid_password = TRUE;
 
  // For password handling
  uint8 password[4] = {1, 2, 3, 4};
  uint8 user_entry[4] = {0, 0, 0, 0};
  uint8 password_size = 4;
  uint8 current_key = 0;
 
  PLL_init();
  seg7_disable();
  led_enable();
  lcd_init();
  SPI1_init();
  SCI1_init(BAUD_RATE);
  SW_enable();
  ad1_enable();
  ad0_enable();
  keypad_enable();
  PIFH = 0xff;                
  PPSH = 0x00;
  PIEH = SW2_MASK;
  leds_off();
  PTP = 0x00;
  HILO1_init();
  RTI_init();
  clear_lcd();
  
  _asm cli;
  
  //------------------------------------------------------
  // Password
  //------------------------------------------------------
  print_statement("Please enter password to proceed: \n\r");
  print_statement("Please press the pound key after each number\n\r");
  print_statement("PIN: ");
  while(!passwordDone)
  {
    old_character = character;
    character = getkey();
    wait_keyup();
   
    if (character == POUND)
    {
      user_entry[current_key] = old_character;
      current_key++;
      print_statement("#");
    }

    if (current_key == 4)
    {
      valid_password = TRUE;
      for (index = 0; index < PASSWORD_SIZE; index++)
      {
        if (user_entry[index] != password[index])
        {
          valid_password = FALSE;
        }
      }
    }
   
    if ((valid_password == TRUE) && (current_key == PASSWORD_SIZE))
    {
      print_statement("\n\r");
      print_statement("\n\rSuccess! Access has been granted.\n\r");
      passwordDone = TRUE;
    }
    else if ((valid_password == FALSE) && (current_key == PASSWORD_SIZE))
    {
      print_statement("\n\rYOU ARE A FAILURE! TRY AGAIN!\n\r");
      print_statement("PIN: ");
      for (index = 0; index < PASSWORD_SIZE; index++)
      {
        user_entry[index] = 0;
        current_key = 0;
      }
    }
  }
 
  //------------------------------------------------------
  // Menu Options
  //------------------------------------------------------
  while(!isDone)
  {
    //------------------------------------------------------
    // Handle switch 2 being pressed
    // This switch will clear the LCD and end the program.
    //------------------------------------------------------
    if (g_switch2pressed)
    {
      isDone = TRUE;
      clear_lcd();
      type_lcd("Program Stopped");
      g_switch2pressed = FALSE;
    }
  
    if(!isDone)
    {
      print_statement("\n\r");
      print_statement("\n\rWelcome to our local weather program.\n\r");
      print_statement("Please select an option to visit our many different weather choices: \n\r");
      print_statement("1. Display Temperature\n\r");
      print_statement("2. Display Wind Speed\n\r");
      print_statement("3. Display Wind Direction\n\r");
      print_statement("Press SW2 and then ENTER to end Program.");

      menu_choice = inchar1(); // read user input from SCI1 serial port
      outchar1(menu_choice); // echo user input back to SCI1 serial port
      other_choice = (menu_choice & 0x0f);
      print_statement("\n\r");
      menu_choice_led(other_choice);
    
      switch (menu_choice)
      {
        case MENU_TEMP:
          print_statement("\n\r");
          print_statement("\n\rTemperature option has been selected.\n\r");
          print_statement("Press 1 for Fahrenheit and 2 for Celsius\n\r");
          print_statement("Please refer to the LCD display");
          temp_choice = inchar1();
          outchar1(temp_choice);
          print_statement("\n\r");
          switch (temp_choice)
          {
            case FAHRENHEIT :
              isFahrenheit = TRUE;
              display_temperature();
              break;
            case CELSIUS:
              isFahrenheit = FALSE;
              display_temperature();
              break;
            default:
              print_statement("Invalid choice. Going back to main menu...\n\r");
          }
          break;
        case MENU_WSPEED:
          print_statement("\n\r");
          print_statement("\n\rWind Speed option has been selected.\n\r");
          print_statement("Please refer to the LCD display");
          display_wind_speed();
          break;
        case MENU_WDIREC:
          print_statement("\n\r");
          print_statement("\n\rWind Direction option has been selected.\n\r");
          print_statement("Please refer to the LCD display");
          display_wind_direction();
          break;
        default:
          print_statement("\n\r");
          if(g_switch2pressed && !isDone)
          {
            print_statement("Invalid choice. Please select a valid option:\n\r");
          }
      }
    }
  }
  
  print_statement("\n\r");
  print_statement("\n\rThank you for using our weather station program.\n\r");
  _asm sei;
} /* main */