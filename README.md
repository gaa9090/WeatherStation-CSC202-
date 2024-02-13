# WeatherStation-CSC202-

Weather Station
Gwen Antonio
Matthew Bonfiglio
Jett Kopalek
Mark Luskiewicz
CSC-202: Programming Embedded Microcontrollers
Prof. Bruce Link
May 19, 2023





Summary: 
A weather station is a system of peripherals that works together to collect current samples of a given climate. When constructing a weather station, common features implemented are wind speed sensors (anemometer), wind direction sensors (wind vane), and a temperature sensor that is embedded in the Dragon12 microcontroller.

This program is a simplified version of a weather station that emulates similar functionality found in real-world weather channels. This project has provided valuable hands-on experience and foundational knowledge that aligns with practical weather monitoring and collection.
Upon opening the program, the user is prompted to enter a password on a keypad in order to gain access to the weather channel. Then the program allows access to display the weather station using putty that allows the user to experience a weather channel interface. Once a menu option is selected, the numerical value that the user enters is displayed on the seven segment display for one second, and then in binary on the leds for one second before turning the leds off.

The weather program incorporates high-quality sensors such as the anemometer, wind vane, and the embedded temperature sensor which ensures precise weather measurements. By using timers and interrupts, the program achieves exceptional accuracy and reduces potential measurement errors. Besides using high quality sensors, the program implements a user-friendly interface through a serial port using Putty. This allows the user to interact with the system and access various weather options. The integration of Putty enhances the usability and ensures a smooth experience compared to switch buttons.
Features include:
User interface that allows the user to access the weather information through Putty where the main menu is displayed.
Temperature option that reads the information from the ADC sensor displayed on the LCD, where the user can choose between Celsius and Farenheight.
Wind speed option which displays the current wind speed on the LCD where the anemometer calculates rotation count over a fixed time period.
Wind direction option displays the wind direction on the LCD where the ADC reads analog voltage and determines the corresponding wind direction.
Internal Components:
Temperature sensor uses the embedded ADC to return the current temperature
Interrupts and timers are utilized to control the timing and frequency of calculations and data sampling.
LCD Screen displays the weather information.
External Components:
SparkFun Cup Anemometer to measure the wind speed in miles per hour.
SparkFun Wind Vane to measure the direction of the wind.
Design Details:
This block diagram represents the entire system: 

Timer Interrupt
The timer interrupt is used to increment the anemometer turn count for each rising edge detected from port T. The timer interrupt is set to activate only on a rising edge of bit 2 of the timer port. This interrupt is constantly counting the total number of rotations such that the system at any point will be able to display the current wind speed during the most recent 3 second increment. The timer function only handles incrementing the count, all other processing is done in the main function. 
ADC
The Analog to Digital Converter (ADC) is an integrated circuit on the HCS12 that converts analog signals to a more discrete digital form consisting of 1’s and 0’s. The resolution of the ADC is 8 bits at 3.3 volts, meaning any digital signal obtained from the ADC will be an 8-bit value. The resolution of the converter enables the mapping of any analog value to a corresponding digital value. Consequently, a higher number of bits in the ADC leads to a finer division of the analog range and increased accuracy in the output.

The ADC contains multiple components that obtain different analog values, including a thermometer, a photosensor, and rotary potentiometer. These components allow you to obtain temperature, light level, and a given voltage based on the rotation of the potentiometer handle. 









SparkFun Wind Vane
The wind vane is connected with the black wire being put to ground, and the green wire is connected to a row on the breadboard that is ultimately connected to pin A6 of the ADC. The row for the green wire contains a 10k ohm resistor going from the row to 5 volts VCC, and a jumper wire connecting the row to the ADC. In total, this row contains a 10k ohm resistor, the green wire from the wind vane, and a jumper wire.


The above schematic shows the inner workings of the wind vane. The wind vane works by closing the circuit on different resistors in the wind vane. Every cardinal direction corresponds to a different combination of resistors, and therefore provides a way to turn the voltage reading into a user-friendly direction. From the table above, the direction is calculated based on the voltage reading from the ADC. Zero degrees would correspond to east, and the angle in degrees is the angle from east measured counterclockwise. Therefore, north would be 90 degrees, west would be 180, and south would be 270. Directly in the middle of those angles, would be corresponding to NE, NW, SW, and SE in that order from zero degrees. Again, going halfway between the four cardinal directions and the in-between directions would correspond to ENE, NNE, NNW, WNW, WSW, SSW, SSE, and ESE. With ENE being 22.5 degrees counterclockwise of east. These are all of the obtainable directions offered from the SparkFrom wind vane. The following code snippet shows how the program handles the wind vane voltage reading: 
vane_value = ad0conv(VANE_PIN);
if (vane_value >= 270 && vane_value <= 286)
   {
     type_lcd("North");
   }

This code firstly captures the value of the ADC on the first line, this corresponds to a different value depending on the direction of the wind vane. Then, the ADC value is compared to a list of possible directions to determine which compass direction the vane is pointing. However, only north was shown in this code snippet. 

SparkFun Cup Anemometer
The anemometer has 2 different wires, one of which is connected through a 10k ohm resistor (any resistor is fine, we are merely avoiding a short) to VCC, and the other is connected to port T bit 2. It does not matter which wire is connected to which pin.

Each arm of the anemometer has a magnet that will close the switch connecting VCC to ground. The output to pin 1 is inverted such that for every switch closing a logic level 1 is sent to GPIO 23. The program uses the time module to track 3 seconds worth of total rising edges and divides it by 9 to get average rising edges per second (1 rotation = 3 rising edges, each arm has a switch closing magnet). The program constantly calculated the total anemometer rising edges for every 3 seconds, and the interrupt service routine is used to count every 3 seconds and reset the value to 0 after each iteration. Only when the user requests to see wind speed in the Putty user interface does the program calculate the actual MPH. The following pseudo code demonstrates how the program calculates wind speed in MPH: 
ROTATION_FACTOR = 1.49


if user requests wind speed:
    write_to_lcd(total_anemometer_RE * ROTATION_FACTOR)

The rotation factor is how many MPH the wind would be moving if the anemometer was turning once per second.





Operation Details:
In order for this project to run, the FTDI cable must be used to allow communication with the Putty based user interface. 
Upon starting the program, the user is met with a welcome message in the putty menu and a prompt to enter a password. The password is defaulted to “1234” and must be entered through the keypad on the HCS12. 


After entering the password, the putty interface displays each option the program offers, and the user must use their computer keyboard to enter each option. 



The information under each option is displayed on the liquid crystal display on the HCS12. Note that the decimal number you enter in the menu is displayed on the 7 segment display and the binary conversion is displayed on the LEDs integrated on the HCS12.





In order to end the program, the user must press switch 2 on the HCS12 and then in the putty menu, press ENTER.

Future work needed:

The Sparkfun weather station kit comes with an additional component that was not implemented yet in the design. The rain meter is a device that measures the amount of rainfall in a particular area. This addition would increase the functionality of the project and add a new value recorded by the weather station.  

In addition, the successful implementation of the BMP180 would aid in recording air pressure and altitude, a new dimension to the weather station. The initial impressions of the IC showed to be difficult to work with in this time constrained environment, but can be resolved by understanding how ro read in the data successfully. 

Finally, the Putty terminal could use cosmetic enhancement. It looks very primitive and looks convoluted once multiple choices have been selected. Spacing out the terminal window as well as redesigning the UI of the program would make the program more readable and easier to use. Within the Putty, when exiting the program, the sequence of having to press SW2 on the Dragonboard and enter on the keyboard seems complicated and could be resolved by designing the program to end when SW2 is pressed. The issue was that the interrupt used with SW2 controls the main while loop condition of the program, so when the interrupt was pressed the interrupt would activate, but a blocking function within the loop was keeping it from exiting the program. The inchar function used to collect input from the terminal was blocking it from exiting, so when the enter key is pressed, the program finally ends. So changing how the program exits and making the program exit when SW2 is pressed would make the program easier to use. 

With the collection of these improvements, the weather station would be more capable and easier to use.


































References
Argent Data Systems. (n.d.). Weather Sensor Assembly p/n 80422. Retrieved May 18, 2023, from https://cdn.sparkfun.com/assets/8/4/c/d/6/Weather_Sensor_Assembly_Updated.pdf
Freenove. (n.d.). GitHub - Freenove/Freenove_RFID_Starter_Kit_for_Raspberry_Pi: Apply to FNK0025. GitHub. https://github.com/Freenove/Freenove_RFID_Starter_Kit_for_Raspberry_Pi

