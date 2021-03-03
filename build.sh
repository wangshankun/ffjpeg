#!/bin/sh
make clean && make -j16

./src/ffjpeg -db ~/face_sdk/jz/test_video/test_jpg/0001.jpg

