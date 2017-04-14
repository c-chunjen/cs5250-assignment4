#!/bin/sh

echo abc > /dev/fourMegaBytes 
cat /dev/fourMegaBytes 
echo defg > /dev/fourMegaBytes 
cat /dev/fourMegaBytes
