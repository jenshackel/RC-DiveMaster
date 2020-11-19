# RC-DiveMaster
Dive Controller for RC-Submarines based on Arduino Nano

It was designed to fit Robert Cook's https://www.thingiverse.com/thing:4560120 (Subnautica Cyclops Submarine).

## Schematics / Parts assembly

![RC DiveMaster Assembly](/images/RC-Divemaster.jpg)

## Features
- Control of the trim tank (fill / drain)
- Compatible with an analog RC-Receiver, e.g. Graupner JR Superhet C16
- Providing 5V power for RC-Receiver
- Auto shut off trim pump when trim tank is full
- Input for water ingress sensor (WIS)
- Monitoring the battery voltage 
- System status via multicolor LEDs
- Auto drain trim tank when RC Signal is lost
- Auto surface and dive protection when water ingress is detected or Battery voltage is low

## Project Status
This design is still in an early stage. Report of problems, errors and improvement suggestions is appreciated. 

## BoM / parts list
- Arduino Nano
- Step-Down-Module DayPOWER 2596-2
- Relay module KY-019 (2 EA)
- Resistor 10k 1/4W
- Resistor 18k 1/4W
- Resistor 470R 1/4W
- Switch for dive tank limit / end.
- Water ingress sensor, self-made (just two metal pins at the bottom of the acrylic tube)

## Building tips

Before you start assembling the electronics you should measure the true ohm values of resistor R1 and R2.
Put the values into the arduino sketch before uploading. This will increase the precision of the voltage measurement.

### User defined parameters
![User parameters of Rev.1](/images/User_parameters_for_Rev1.JPG)

#### int debugLevel = 1;             
0 = Debug information will not be displayed. Select this value if your system is running as wanted.
1 = Debug information is displayed via the serial monitor to find bugs or correct settings.

#### int rcSamples = 8;              // Number of samples for RC-Quality
If the Remote Control transmitter is switched of or out of range the signal for the dive / surface commands may oscillate. This is causing unwanted relay activation. To prevend this, Signal samples are collected and evaluated. The more samples the less relay-on-errors will appear - but will slow down the reaction of the system. The preset of 8 samples is a good compromise.

#### int ledBrightness = 20;         
Set the brightness of the Status-LED(s). A value from 1 to 255 is possible. Should be set to a high value after system adjustments are completed.

#### float batThreshold = 8.0;
This variable sets the threshold (in Volts) for Low-Battery state. For a battery pack of 8 NiMH cells (9,6V in total) should be set to 8,0V as threshold value.
Other battery types with different cell amount or technology may need another value. Make sure the value fits your battery type.

#### float R1 = 18000.0;
#### float R2 = 10000.0;             
The resistors R1 and R2 create an voltage devider which is used for battery voltage measurement. To increase accuracy measure the exact value of your resistors BEFORE assembling the electronics. 

#### long autoTrimTimeout = 30000;   
This variable sets the time which is needed to drain the trim tank completely. It is used in case of low-battery-voltage and water ingress to get your submarine back to surface immediately. It is measured in milliseconds (1 second = 1000 milliseconds). 30 seconds equals 30000 milliseconds.
Make sure the value is at least as loang as your pump needs for a full drain. If it is larger - the pump runs longer and discarges the (maybe almosst empty) battery a bit more.

## Using the RC-DiveMaster / System behaviour

After applying electrical power the status LED should be yellow (remote control is OFF). 
Switch your remote to ON. The LED should now light up in green.
You can use the remote control to switch the drain pump adjusting the trim tank water level.

### Status LED colors
The color of the LED(s) states the present codition of your RC-submarine:

#### red
Radio Signal is lost and battery is low.

#### blue
Water ingress is detected by the WIS sensor.

#### orange
Battery's low.

#### yellow
Radio signal is lost.

#### green
Everything's fine!

### Safety functions 
If the trim tank dive (fill) command was given once the automatic surface function is enabled. That means that if RC-signal is lost the trim tank will be drained until RC-signal is received again.

In case of low battery voltage or water ingress the tank is drained completely. After that it is not possible to control the trim pump (for diving), a power down / up is required as reset. To make sure everything is ok, a visual inspection of your RC-submarine is recommended. Check status LED(s) color for troubleshooting / damage isolation.

## Impressions
Here's a photo of the first RC-DiveMaster to be fitted into the metric tube design:
![Image of RC-DiveMaster](/images/Divemaster_Image_1.jpeg)

And here's the WIS-Sensor for the metric tube design:
![Image of the WIS-Sensor](/images/WIS_Image_1.jpeg)
