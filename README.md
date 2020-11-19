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

Before you start assembling the electronics you should measure the true ohm values of resistor #1 and #2.
Put the values into the arduino sketch before uploading. This will increase the precision of the voltage measurement.

## Using the RC-DiveMaster / System behaviour

tbd.

## Impressions
Here's a photo of the first RC-DiveMaster to be fitted into the metric tube design:
![Image of RC-DiveMaster](/images/Divemaster_Image_1.jpeg)

And here's the WIS-Sensor for the metric tube design:
![Image of the WIS-Sensor](/images/WIS_Image_1.jpeg)
