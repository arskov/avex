# Simple Web Camera Player

Simple web camera player implemented with FFmpeg(libav*) and SDL2

### Prerequisites

Install required libraries
```
sudo apt update
sudo apt install \
    build-essential \
    ffmpeg \
    libsdl2 \
    libavcodec-dev \
    libavformat-dev \
    libavfilter-dev \
    libavutil-dev \
    libavdevice-dev \
    libsdl2-dev
```

If you are using Visual Studio Code install following extentions: `ms-vscode.cpptools`, `twxs.cmake`, `vector-of-bool.cmake-tools`.

Create `build` folder inside (sibling to `src` folder). From Visual Studio Code command pallete (`Ctrl+Shif+P`) invoke `CMake: Configure` and then `CMake: Debug Target`.

Since this code has been created for learning purposes, many things are not implemented as they should be, and in particular the device name is hardcoded :). So investigate supported formats of your webcam first. You can do this with following commands:

```
v4l2-ctl --list-devices

Integrated Camera: Integrated C (usb-0000:00:14.0-8):
	/dev/video0
```

```
v4l2-ctl -i /dev/video0 --list-formats

Video input set to 0 (Camera 1: Camera, ok)
ioctl: VIDIOC_ENUM_FMT
	Index       : 0
	Type        : Video Capture
	Pixel Format: 'YUYV'
	Name        : YUYV 4:2:2

	Index       : 1
	Type        : Video Capture
	Pixel Format: 'MJPG' (compressed)
	Name        : Motion-JPEG
```

```
ffmpeg -f v4l2 -list_formats all -i /dev/video0

...
[video4linux2,v4l2 @ 0x55dd373c18c0] Raw       :     yuyv422 :           YUYV 4:2:2 : 640x480 320x180 320x240 352x288 424x240 640x360 848x480 960x540 1280x720
[video4linux2,v4l2 @ 0x55dd373c18c0] Compressed:       mjpeg :          Motion-JPEG : 640x480 320x180 320x240 352x288 424x240 640x360 848x480 960x540 1280x720
```

Hit `F5` to run. Enjoy :)