#!/usr/bin/python
# -*- coding: utf-8 -*-

from urllib2 import urlopen, URLError, HTTPError
import platform
import os

try:
	fileName = "msetdg.bin";
	tid = [0x00020000, 0x00020000, 0x00021000, 0x00021000, 0x00022000, 0x00022000, 0x00027000, 0x00027000, 0x00026000, 0x00028000] #JPN 4/6, USA 4/6, EUR 4/6, KOR 4/6, CHN 4, TWN 4
	tver = [0x1A, 0x22, 0x1F, 0x2D, 0x19, 0x23,  0x04, 0x0D, 0x06, 0x05]
	choice = 0
	while choice not in (1, 2, 3, 4, 5, 6, 7, 8, 9, 10):
		os.system('cls') if platform.system() == "Windows" else os.system('clear')
		print "Select your 3DS region :\n\n    [1] JPN (4.x MSET)\n    [2] JPN (6.x MSET)\n\n    [3] USA (4.x MSET)\n    [4] USA (6.x MSET)\n\n    [5] EUR (4.x MSET)\n    [6] EUR (6.x MSET)\n\n    [7] KOR (4.x MSET)\n    [8] KOR (6.x MSET)\n\n    [9] CHN (4.x MSET)\n\n   [10] TWN (4.x MSET)\n"
		choice = int(raw_input("Selection : "))
	url="http://nus.cdn.c.shop.nintendowifi.net/ccs/download/00040010%08X/%08X"%(tid[choice-1], tver[choice-1])
	print "Downloading MSET from the CDN..."
	f = urlopen(url)
	with open(fileName, "wb") as lf:
		lf.write(f.read())
	print "Downloaded!"
except HTTPError, e:
	print "HTTP Error:", e.code, url
except URLError, e:
	print "URL Error:", e.reason, url