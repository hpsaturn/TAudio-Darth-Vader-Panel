# Darth Vader Frontal Panel

Basic sound effects and LED control of TTGO TAudio ESP32 board for a Darth Vader frontal panel. Includes its box for 3D printing with many improvements from the original box, see the [Credits](#Credits).


## Build and Upload

Please install first [PlatformIO](http://platformio.org/) open source ecosystem for IoT development compatible with **Arduino** IDE and its command line tools (Windows, MacOs and Linux). Also, you may need to install [git](http://git-scm.com/) in your system. After install that you should have the command line tools of PlatformIO. Please test it with `pio --help`. Then please run the next command for build and upload the firmware:

```bash
pio run --target upload
```

## Configuration

Using your USB cable, connect it and run:

```bash
pio device monitor
```

Press enter and type help. You should see the configuration commands for setup your WiFi and panel settings.

## Credits

Original box unfinished from [Thingiverse @alexgeiger](https://www.thingiverse.com/thing:2048281/files)
