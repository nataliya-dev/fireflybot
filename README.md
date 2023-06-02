# Fireflybot

Fireflybot mimics the synchonization patterns of fireflies with electronic components.

## Hardware

- Raspberry Pi 3 Model B (v1.2)
- Sandisk Ultra 32Gb Micro SD Card
- Intel® RealSense™ Depth Camera Model D435

## Software Requirements

- Ubuntu MATE Version 20.04.4 LTS Focal Fossa arm64 system
- Intel® RealSense™ SDK 2.0 (v2.50.0)
- OpenCV (Open Source Computer Vision Library) (v4.2.0)
- WiringPi (https://github.com/wbeebe/WiringPi)

## Installation tips

- [Intel® RealSense™ D400 cameras with Raspberry Pi](https://github.com/IntelRealSense/librealsense/blob/master/doc/RaspberryPi3.md)
- Build with DBUILD_GRAPHICAL_EXAMPLES=false
- Specify the cores when running make (`` make -j `nproc`  ``)
- In some cases the RAM capacity is not sufficient to compile the SDK, so if the compilation process crashes or exits with an error-code try to create a swap file and then recompile the SDK.

## Usage

Clone the project into your desired directory then build the project.

```
cd fireflybot
mkdir build && cd build
cmake ..
make
```

Connect the LED to WiringPi GPIO 29 and the camera via USB to the Raspberry Pi. Test each component individually before running the system to make sure the hardware is connected properly.

To test the blink module:

```
sudo ./fireflybot -tb
```

To test the camera module:

```
./fireflybot -tc
```

To test the hardware-free synchronization module:

```
./fireflybot -sim
```

Once all the components are connected and tested separately, you can run the synchonization module.

```
sudo ./fireflybot
```
