# Environmental Sensor Controller
Temperature and humidity sensor controller (Terminal) that periodically sends its data over LoRa, which then a Gateway that receives it, Publishes to a MQTT broker. This project started as a small project to improve my personal [PiniCore-Embedded](https://github.com/PiniponSelvagem/PiniCore-Embedded) library, and to add some sensors to my Home Assistant system.


## How it works (LoRa system)
At least one Gateway should be installed in the vicinity of the Terminal controllers.
#### Terminal
1. Terminal wakes up
2. Reads the sensors
3. Packs the necessary data, battery %, temperature ºC and humidity %
4. Sends the payload over LoRa
5. Waits for a ACK reply up to a defined timeout
6. Receives an ACK reply
7. Goes to sleep until next sensor read

#### Gateway
1. Gateway is always listening for LoRa payloads from the Terminals
2. Receives the payload, validates that it is valid and sends an ACK reply
3. Unpacks the data and creates a MQTT message
4. MQTT message is then published in the ***status*** topic
5. Listens for next LoRa payload

## How it works (Standalone system)
1. Connects to the congfigured WiFi
2. Connects to the configured MQTT
3. When connected to MQTT, periodically publishes the sensors current values
- Note:
    - Currently it does not sleep, so do not leave it on battery. Planned feature, sleep just like the *Terminal* variant.
    - Only supports SprigLabs-C3.

## Hardware
### Terminal
- **LILYGO / TTGO LoRa32 Display**
    - version used during developemnt: T3_V1.6.1
    - environment: *TERMINAL*
- **DHT11**
    - Digital Humidity Temperature sensor
- **Battery**
    - 18650 Li-Ion

### Gateway
- **LILYGO / TTGO LoRa32 Display**
    - environment: *GATEWAY*
    - version used during developemnt: T3_V1.6.1

### Standalone
- **Sprig-C3** + **SprigLabs ROOT module**
    - environment: *STANDALONE_SPRIGLABS*


## Credentials
Sensitive information and credentials should be placed in a header file named ```secrets.hpp``` inside the ```src``` folder.

Here is an example on how to structure it:
``` C++
//// --------------- WIFI ---------------- ////
#define WIFI_SSID "MyWifi"
#define WIFI_PASS "supersecretpassword"
//// ------------------------------------- ////

//// --------------- MQTT ---------------- ////
#define MQTT_SERVER "mymqtt.broker"
#define MQTT_PORT 1883
#define MQTT_USER "mymqtt_user"
#define MQTT_PASS "mymqtt_password"
//// ------------------------------------- ////
```


## MQTT - GATEWAY

### Online status
Function: Gateway current online status<br>
Topic: ```environment/sensors_async/v0/{gateway_serial}/up/lwt```<br>
Payload: ```0``` or ```1```<br>
Parameters:
- **gateway_serial**: ```uint64_t```, 64 bit positive value

---

### Version
Function: Gateway current firmware version<br>
Topic: ```environment/sensors_async/v0/{gateway_serial}/up/version```<br>
Payload: ```uint16_t```, positive integer value<br>
Parameters:
- **gateway_serial**: ```uint64_t```, 64 bit positive value

---

### Status
Function: Terminal status payload will be placed in this topic by the receiving gateway<br>
Topic: ```environment/sensors_async/v0/{gateway_serial}/up/status```<br>
Payload:<br>
- **radioId**: ```uint64_t```, 64 bit positive value, serial of the Terminal that sent the payload
- **version**: ```uint16_t```, 16 bit positive value, firmware version of the Terminal that sent the payload
- **rssi**: ```int```, negative value tht represents the signal strength in dB
- **battery**: ```uint8_t```, 8 bit positive value that ranges from 0 to 100 and represents the battery status of the Terminal
- **temperature**: ```int8_t```, 8 bit value that has the temperature read by the sensor from the Terminal
- **humidity**: ```uint8_t```, 8 bit positive value that has the humidity read by the sensor from the Terminal

Parameters:
- **gateway_serial**: ```uint64_t```, 64 bit positive value

Payload example: ```6,1,-83,74,22,68```
- ```6```: Terminal identifier
- ```1```: Terminal firmware version
- ```-83```: Signal strength received by the Gateway from the Terminal
- ```74```: Terminal battery percentage
- ```22```: Temperature read in ºC
- ```68```: Humidity read in %

---


## MQTT - STANDALONE

### Online status
Function: Standalone current online status<br>
Topic: ```environment/sensors_async_sad/v0/{serial}/up/lwt```<br>
Payload: ```0``` or ```1```<br>
Parameters:
- **serial**: ```char*```, string 12 characters

---

### Version
Function: Standalone current firmware version<br>
Topic: ```environment/sensors_async_sad/v0/{serial}/up/version```<br>
Payload: ```uint16_t```, positive integer value<br>
Parameters:
- **serial**: ```char*```, string 12 characters

---

### Type (WIP)
**NOTE: This topic is not final, might be changed/removed later in development**<br>
Function: Identify the type of standalone system<br>
Topic: ```environment/sensors_async_sad/v0/{serial}/up/type```<br>
Payload: ```char*```, name of the standalone type<br>
Parameters:
- **serial**: ```char*```, string 12 characters

---

### Battery
Function: Standalone current battery percentage status<br>
Topic: ```environment/sensors_async_sad/v0/{serial}/up/battery```<br>
Payload: ```int```, current battery percentage

Parameters:
- **serial**: ```char*```, string 12 characters

---

### Lux
Function: Standalone lux sensor current value<br>
Topic: ```environment/sensors_async_sad/v0/{serial}/up/lux/{sensor_idx}```<br>
Payload: ```int```, light value in lux

Parameters:
- **serial**: ```char*```, string 12 characters
- **sensor_idx**: ```int```, sensor index in the same sensor type, see [sensor_idx table](#topics-sensor_idx-table)

---

### Temperature
Function: Standalone temperature sensor current value<br>
Topic: ```environment/sensors_async_sad/v0/{serial}/up/temperature/{sensor_idx}```<br>
Payload: ```int```, temperature in celcius (ºC)

Parameters:
- **serial**: ```char*```, string 12 characters
- **sensor_idx**: ```int```, sensor index in the same sensor type, see [sensor_idx table](#topics-sensor_idx-table)

---

### Humidity
Function: Standalone humidity sensor current value<br>
Topic: ```environment/sensors_async_sad/v0/{serial}/up/humidity/{sensor_idx}```<br>
Payload: ```int```, humidity in percentage (%)

Parameters:
- **serial**: ```char*```, string 12 characters
- **sensor_idx**: ```int```, sensor index in the same sensor type, see [sensor_idx table](#topics-sensor_idx-table)

---

### Topics sensor_idx table
| Standalone Type  | Sensor Topic | sensor_idx | Sensor Description  |
|------------------|--------------|------------|---------------------|
| SprigLabs        | Lux          | 0          | Light               |
| SprigLabs        | Temperature  | 0          | Ambient Temperature |
| SprigLabs        | Humidity     | 0          | Ambient Humidity    |
| SprigLabs        | Humidity     | 1          | Soil Humidity       |

---


## TODO and/or WIP
- Gateway:
    - encrypt credentials
    - admin topic to ask gateway some debug information
    - into topic to ask gateway for statistics like uptime, bytes sent/received, etc
- Standalone:
    - maybe??? by not using LoRa and using WiFi, with the cost of more battery usage, but for home use might be fine
- Common features:
    - WiFi AP that is active on reset button press, that runs a HTTP web server tht provides a configuration page, possible features:
        - config: credentials
        - ping nearby gateways and show then on a list with their signal quality
        - factory reset
