
# Project Title

 STM32_L452REP data collection using the MMA8451 accelerometer

# Project Description

- Application usage

So, in a nutshell, this project is aiming to simulate the behavior of a vehicle's processing unit responsile for keeping track of acceleration data, processing of said data and transmission of relevant information through CAN.

- Technologies used

The project is built around STM32's L452REP board housing the Arm Cortex-M4 microcontroller, mainly due to the fact that the power consumption on said board is fairly low and that this is the board I had available. That being said, the project is higly portable, so switching the board won't create much of an issue.

Keeping track of the acceleration data is the MMA8451 accelerometer, which was the cause of most of the headaches present during the creation of this project, mainly due to poor documentation of the chip itself, 3 different datasheets presenting all different information regarding the chip's characteristics, but also due to little to no documentation regarding the pcb on which the chip is mounted, having to measure the values of the pull up resistors used for I2C communication myself, realizing that they are outside of the recomanded range, having to adjust the frequency for the I2C communication so that there won't be any data transfer issues according to said values, etc. More on that later.

CAN communication is done through one of the built-in CAN peripherals of the microcontroller for the sequential part, the conversion to differential signal being performed with the help of the TJA1043 transceiver, which was very straight-forward to use and presented the opportunity to get my hands on the soldering iron and create a simple board design centered around the transceiver as recomanded in the datasheet, exercise that was surprisingly calming and a very enjoyable break from coding.

It is worth mentioning that the local reset of the board was both a nice yet simple puzzle on the hardware side and an opportunity to use the discover something about the I/O of the board. Reset is done through an I/O pin that is connected to a 120 ohm resistor that leads to the base of a BC547 NPN transistor. The emitter of the transitor is connected to the GND of the board, and the collector to the NRST pin of the board. When the pin on the base goes high, the transistor opens, the GND an NRST get connected and so the master reset is initiated. More on why this action has to be done like this in the next section.

- Challenges faced and future improvements
 
As mentioned previously, setting up the MMA8451 accelerometer brought plenty of roadblocks, most of which are mentioned in its dedicated library, MMA8451.h and its associated .c file. 

The NRST pin presented a challenge due to the fact that reset is performed through connecting this pin to ground, it being pulled high by default. Thus, a GPIO pin set up as output pin can't be directly connected to the NRST due to the fact that, after initialization, the pin outputs 0 by default and so the board resets reading 0 on the NRST pin. This being said, the solution with the BC547 is simple yet elegant, providing the opportunity for a little more soldering action.

Remember to use an 120 ohm resistor between the CAN_HIGH and CAN_LOW lines to cancel out the interferences, more often than not the absence of this resistor leading to CAN acking errors on the CAN bus.

Other minor problems and challenges that showed up consist of setting up the timers used for waiting for the startup sequence and for data transmission, setting up the CAN mask for receiveing data, implementation of various callbacks and so on. More on these can be found all throughout the project as comments. Don't forget to check them out, mostly the ones present in main.c and QualityOfLife.c.

Regarding future improvements, there is plenty of experimentation to be done with the MMA8451 accelerometer, it having plenty of registers that written correctly can make the data acquisition smoother and cleaner.

# How to run

In order to run the project, all that needs to be done is to have Cube IDE installed, to download the project into your current workspace, to open it and to load it onto your L452REP board ( or one compatible in pinout ). The assembly of hardware will be presented in detail in the next section, and its functionality can be checked through any CAN debugging software and hardware. Take not that communication is performed at 500 kbps, with the sampling point at 87.5%. 

# How to use

Most of the assembly work is taken by the TJA1043 transceiver, for which I highly recomand the use of the split pin along two 60 ohm resistors and a small ceramic capacitor, as shown in its datasheet. After that you need to locate the +5V, +3.3V and GND pins on your board and connect them to the matching pins of the transceiver, according to the datasheet. Check out the .ioc file of your project in order to fin the pins reseved for CAN1_RX and CAN1_TX and connect them to the matching pins of the transceiver.

Connecting the MMA8451 accelerometer should be fairly straight forward, the pins required for this one being the +3.3V, GND, I2C2_SCL and I2C2_CLK ( the placement of the later two can be found in the .ioc file ).

Also remember to connect the NRST pin in the configuration explained earlyer in this document, the reset pin being pin 14 of GPIOB port.

Remember to check out the schematic for the L452REP board to find the possitions of the pins mentioned above.

After all the connections are performed, all you need to do is load the project onto the board, connect CAN_HIGH and CAN_LOW pins of the transceiver to a CAN bus and let it run. Remember, can is a differential communication protocol so twist the wires for CAN_HIGH and CAN_LOW together so that both channels are affected by the same noise. You can also use a terminal and the usb cable for debugging, various debug messages being sent through UART. To do that, set the terminal to communicate in serial mode with the matching COM port of the board, the baud rate of the communication being 115200.