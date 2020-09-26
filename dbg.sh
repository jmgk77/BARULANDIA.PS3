#!/bin/sh
make
ps3load barulandia.ps3.self
netcat -l 18194
