# ECE4220_FinalProject
Marshall Lindsay  
Max Houck  
ECE4220  
## Requirements  
#### RTUs  
- 3 Digital Inputs (0, 3V3)(Switches).
- 3 Digital Outputs (LEDS).
- 1 Analog Input (1v, 2v, 10hz).
- Event detection.
  - Changes in switch values.
  - Line overload (V < 1 or V > 2).
  - No power.
  - Command received.
- Information needs to be recorded.
  - Time Stamp.
  - RTU identification number.
  - The status of all of the switches (open/closed) and LEDs (on/off).
  - Voltage at the line at the time of the event.
  - Type of event / current status.
- Send a periodic update every 1 second.
  - Should consist of all of the events since the last update.
  - If no events, send the current state.

#### Historian  
- Save all of the updates.
- Synchronize all of the clocks.
- Display the information.

#### Hardware
- Analog signal.
  - Sine wave.
  - 1V < V < 2V (Nominal Values).
    - Should be able to increase or decrease.

#### Extra Credit
- Use Object Oriented programming.
  - Classes.
  - Exception Handling.
- Use a database to manage the data.
  - A mysql database on my laptop?
- Add additional sensors to be read from.
  - I2C Temp Sensor?
- Use the seven segment display on the auxiliary boards.
