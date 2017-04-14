#!/bin/sh

sudo rm -rf /dev/fourMegaBytes
sudo mknod /dev/fourMegaBytes c 61 1
sudo chmod 666 /dev/fourMegaBytes
