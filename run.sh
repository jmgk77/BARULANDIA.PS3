#!/bin/sh
echo *****************************************BUILD
make
echo *****************************************SEND CODE
ps3load barulandia.ps3.self
echo *****************************************DEBUG
netcat -l 18194
