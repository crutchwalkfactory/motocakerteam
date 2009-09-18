#! /bin/bash

source ezxenv.sh
./configure --enable-video-motoezx=yes --enable-audio-ezx=yes --disable-video-dummy --disable-video-fbcon --disable-video-tga --disable-arts --disable-esd --disable-alsa --enable-cdrom --disable-video-x11 --disable-nasm --host=arm-linux --host=arm-linux-gnueabi --prefix=/arm-eabi --enable-video-directfb=no --enable-video-opengl --enable-osmesa-shared --enable-osmesa-shared --x-libraries=/install/moto/xc/xc/exports/lib --x-includes=/install/moto/xc/xc/exports/include
