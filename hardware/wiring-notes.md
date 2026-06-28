\# Wiring Notes



\## Main Controller



The ESP32 works as the main controller for wireless communication, command processing and camera/web server operation.



\## Motor Driver



The motor driver receives control signals from the ESP32 and drives the DC motors.



\## Motor Connections



\- Left-side motors are connected to one motor output side.

\- Right-side motors are connected to the other motor output side.

\- Motor direction is controlled through ESP32 GPIO signals.



\## LED Connections



\- Front LED is used as the headlight.

\- Back LED is used as the rear light.

\- LEDs are controlled using GPIO pins.



\## Power Supply



\- Li-ion batteries are used as the power source.

\- Motor driver and ESP32 should share common ground.

\- Use proper voltage regulation if required.



\## Safety Notes



\- Check battery polarity before powering the circuit.

\- Avoid short circuits.

\- Test the motors without load first.

\- Do not touch wiring while the car is powered.

