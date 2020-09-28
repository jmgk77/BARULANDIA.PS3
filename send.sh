
#!/bin/sh
echo *****************************************BUILD PKG
make pkg
echo *****************************************SEND DATA
ftp -n 192.168.0.4 <<END_SCRIPT
quote user anonymous
quote PASS password
cd /dev_hdd0/packages
del barulandia.ps3.pkg
put barulandia.ps3.pkg
quit
END_SCRIPT