@mqtt_trigger("environment/sensors_async/v0/0/up/status")
def controller_update(topic=None, payload=None, **kwargs):
    try:
        parts = payload.strip().split(",")

        if len(parts) != 6:
            log.error(f"Invalid payload length: {payload}")
            return

        radio_id = parts[0]
        firmware = parts[1]
        rssi = int(parts[2])
        battery = int(parts[3])
        temperature = int(parts[4])
        humidity = int(parts[5])

        base = f"envsensor_{radio_id}"

        # Use entity attributes to store a "unique ID"
        attrs_temp = {
            "unique_id": f"{base}_temperature",
            "unit_of_measurement": "°C",
            "friendly_name": f"EnvSensor {radio_id} Temperature",
        }
        attrs_humidity = {
            "unique_id": f"{base}_humidity",
            "unit_of_measurement": "%",
            "friendly_name": f"EnvSensor {radio_id} Humidity",
        }
        attrs_battery = {
            "unique_id": f"{base}_battery",
            "unit_of_measurement": "%",
            "friendly_name": f"EnvSensor {radio_id} Battery",
            "device_class": "battery",
        }
        attrs_rssi = {
            "unique_id": f"{base}_rssi",
            "unit_of_measurement": "dBm",
            "friendly_name": f"EnvSensor {radio_id} RSSI",
            "device_class": "signal_strength",
            "entity_category": "diagnostic",
        }
        attrs_firmware = {
            "unique_id": f"{base}_firmware",
            "friendly_name": f"EnvSensor {radio_id} Firmware",
            "entity_category": "diagnostic",
        }

        # Set states
        state.set(f"sensor.{base}_temperature", temperature, attrs_temp)
        state.set(f"sensor.{base}_humidity", humidity, attrs_humidity)
        state.set(f"sensor.{base}_battery", battery, attrs_battery)
        state.set(f"sensor.{base}_rssi", rssi, attrs_rssi)
        state.set(f"sensor.{base}_firmware", firmware, attrs_firmware)

    except Exception as e:
        log.error(f"Failed to process MQTT payload '{payload}': {e}")