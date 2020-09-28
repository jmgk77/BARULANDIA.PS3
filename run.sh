#!/bin/sh
echo *****************************************BUILD
make
echo *****************************************SEND CODE
ps3load BARULANDIA.PS3.self
echo *****************************************DEBUG
netcat -l 18194
