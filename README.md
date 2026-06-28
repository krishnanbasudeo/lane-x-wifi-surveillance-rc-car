# Lane X – Wi-Fi Enabled Surveillance RC Car

🏆 3rd Place Winner – KIIT Electrical Expo

Lane X is a Wi-Fi-enabled surveillance RC car built using ESP32, camera streaming, web server control, motor driver circuitry and MIT App Inventor mobile application.

The project demonstrates real-time wireless control, live visual feedback and embedded hardware-software integration.

## Project Overview

This project creates a self-contained Wi-Fi controlled robotic vehicle. The ESP32 works as a wireless controller and hosts a web server. The user can control the car using a mobile app or web interface.

The system supports:

- Forward movement
- Backward movement
- Left and right turns
- Stop control
- Speed control
- Front and back light control
- Camera-based surveillance
- Web server based control
- MIT App Inventor mobile control

## Team Members

- Basudeo Krishnan
- Bhargavi Siva

## Achievement

This project secured **3rd position at KIIT Electrical Expo**.

## Hardware Components

| Component | Quantity | Purpose |
|---|---:|---|
| ESP32 S3 Sense / ESP32 Camera Board | 1 | Main controller and camera streaming |
| Motor Driver | 1 | Controls the motors |
| DC Coreless Motors | 4 | Car movement |
| Li-ion Batteries | 2 | Power supply |
| LEDs | 2 | Front and back lights |
| Connecting Wires | As required | Circuit connections |
| Chassis | 1 | RC car body |
| Wheels | 4 | Movement |

## Software Used

- Arduino IDE
- ESP32 Board Package
- MIT App Inventor
- HTML, CSS and JavaScript

## Repository Structure

    firmware/     ESP32 / Arduino code
    app/          MIT App Inventor source file and screenshots
    hardware/     Circuit diagram, flowchart and wiring details
    docs/         GitHub Pages website files
    media/        Demo video link

## How It Works

The ESP32 creates a Wi-Fi network and hosts a web server. The mobile app or browser connects to this network and sends movement commands. The ESP32 receives these commands and controls the motors through the motor driver. The camera module provides live visual feedback for surveillance and inspection.

## Applications

- Remote inspection
- Mining safety inspection
- Warehouse automation
- Search and rescue robotics
- Educational robotics
- Low-cost surveillance rover

## Project Report

The full project report is available here:

    docs/assets/Lane-X-Report.pdf

## Demo Video

Demo video link will be added in:

    media/demo-video-link.md

## License

This project is shared for educational and learning purposes.