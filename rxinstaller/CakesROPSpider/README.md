CakesSpiderROP
========

Installs the mset rop using spider.

## NOTE
**On KR/EU/JP/US**

*   Installs **mset 4.x** ROP on firm 4.x~5.x

*   Installs **mset 6.x** ROP on firm 6.0~9.2

**On CN/TW**

 * Installs **mset 4.x** ROP on firm 4.x~9.2

All versions not mentioned above is not supported.  

Usage
========
## Basic usage
Copy code.bin to the root of your sdcard and point your spider to :

  http://dukesrg.no-ip.org/3ds/cakes?code.bin  
  ![qr](https://chart.googleapis.com/chart?cht=qr&chs=220x220&chl=http://dukesrg.no-ip.org/3ds/cakes?code.bin)

The bottom screen should flash with a random pattern. Wait ~10seconds and press home menu to exit spider.

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
* smealum, others for ctrulib
* KARL, OSKA for bootstrap
* Yifan Lu for service patch and Spider3DSTools
* dukesrg for rop3ds, Spider3DSTools, and hosting
