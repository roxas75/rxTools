CakesSpiderROP
========

Installs the mset rop using spider.

## NOTE
Installs **mset 4.x** ROP on firm 4.x  
Installs **mset 6.x** ROP on firm 7.1~9.2  
All versions not mentioned above is not supported.  

Usage
========
## Basic usage
Copy code.bin to the root of your sdcard and point your spider to :

* On 4.x firms :  
  http://dukesrg.no-ip.org/3ds/rop/?LoadCode4.dat  
  ![qr](https://chart.googleapis.com/chart?cht=qr&chs=220x220&chl=http://dukesrg.no-ip.org/3ds/rop/?LoadCode4.dat)

* On 7.1~9.2 firms :  
http://dukesrg.no-ip.org/3ds/rop/?LoadCode.dat  
![qr](https://chart.googleapis.com/chart?cht=qr&chs=220x220&chl=http://dukesrg.no-ip.org/3ds/rop/?LoadCode.dat)

The bottom screen should flash with a random pattern. Press home menu to exit spider.

This will install the rop for for loading Cakes.dat by default.

## Custom rop
To install a rop for a custom file, put ropCustom.txt in the root of the sdcard. The first line inside the file will be used as the custom dat filename.
The custom filename is limited to 12 characters.
See example/ropCustom.txt for an example.

Building
========
Requires devkitPro. Run make to compile.

Credits
========
* Anonymous contributor for the code
* zoogie for the patch format