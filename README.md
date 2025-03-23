# IV-27 VFD Clock

> Work in progress

## Features

- Current time/date (NTP or RTC)
- Temperature + Humidity
- Select displayed info with a rotary knob
- Display simple messages from Home Assistant

## Bill of materials

- IV-27 VFD tube
- MAX6921 serial-interface
- SOP28 to DIP28 adapter
- Seeed Studio Xiao ESP32C3 MCU
- (optional) BME280 ambient sensor
- (optional) DS3231 RTC
- MT3608 boost converter (25V)
- Mini560 or LM2596 buck converter (3.3V)
- rotary encoder with push action
- 5v power supply (or USB)

## Interactions

In display mode:
- single click: toggle on-off
- rotate left or right: switch between time/date/temperature
- long click: enter menu

In menu mode:
- rotate left or right: select menu item
- single click: select menu item

In date/time edit (RTC only):
- rotate left or right: change value
- single click: next element
- long click: previous element

## Configuration

### Home Assistant

Add a custom sensor whose state value is the message to display on the clock.

**Example (extracting data from a todo list):**
```yaml
template:
  - trigger:
      - platform: state
        entity_id: todo.my_list
    action:
      - service: todo.get_items
        data:
          status: needs_action
        target:
          entity_id: todo.my_list
        response_variable: items
    sensor:
      - unique_id: todo_message
        state: >
            {% set tdate = (now().date() + timedelta(days=7)) | string %}
            {{ items['todo.my_list']['items'] 
				| selectattr('due', 'defined') | selectattr('due', 'lt', tdate) 
				| map(attribute='summary') 
				| list | join('|') | truncate(255) }}
```

### Firmware

Copy `secrets.tpl.h` into `secrets.h` and fill the values.

- `HA_TOKEN`: go to your HA profile page, then Security, and create a new Long lived token at the bottom of the page (keep the `Bearer ` prefix)
- `HA_URL`: fill in your HA hostname and the id of the sensor created above
- `OTA_PASS`: choose a password to secure the Wifi OTA update

Copy `upload_params.tpl.ini` into `upload_params.ini` and fill the OTA password.

----

In `constants.hpp` you can also configure:

- `USE_NTP`: use Network Time Prototal, comment to use RTC instead
- `USE_WIFI_OTA`: comment to disable the Wifi OTA
- `USE_BME280_SENSOR`: comment to disable the ambient sensor
- `USE_HA_MESSAGE`: comment to disable the message display from HA
- `TIMEZONE`: timezone for NTP ([check available TZ](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h))
- `TEMP_OFFSET`: applies an offset to the measured temperature
- `DIN`, `CLK`, `LOAD`, `BLANK`: MCU pins to the MAX6921
- `ENCODER_A`, `ENCODER_B`, `ENCODER_SW`: MCU pins to the rotary encoder
- `GRID`, `SEGMENTS`: MAX6921 pins to the display

## Resources

- https://www.instructables.com/VFD-Alarm-Clock/
- https://www.barbouri.com/2020/07/04/iv-27-icetube-clock-project/

## License

Creative Commons 3.0 BY-SA
