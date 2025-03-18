# IV-27 VFD Clock

> Work in progress

## Bill of materials

- IV-27 VFD tube
- MAX6921 serial-interface
- SOP28 to DIP28 adapter
- Seeed Studio Xiao ESP32C3 µC (or SAMD21)
- BME280 ambient sensor
- DS3231 RTC
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

In date/time edit:
- rotate left or right: change value
- single click: next element
- long click: previous element

## Resources

- https://www.instructables.com/VFD-Alarm-Clock/
- https://www.barbouri.com/2020/07/04/iv-27-icetube-clock-project/

## License

Creative Commons 3.0 BY-SA
