#!/usr/bin/python
# -*- coding: utf-8 -*-

from urllib2 import urlopen, URLError, HTTPError
import platform
import os

try:
	fileName = "msetdg.bin";
	tid = [0x00020000, 0x00021000, 0x00022000] #JPN, USA, EUR
	tver = [0x1A, 0x1F, 0x19]
	choice = 0
	while choice not in (1, 2, 3):
		os.system('cls') if platform.system() == "Windows" else os.system('clear')
		print "Select your 3DS region :\n    [1] JPN\n    [2] USA\n    [3] EUR\n"
		choice = int(raw_input("Selection : "))
	url="http://nus.cdn.c.shop.nintendowifi.net/ccs/download/00040010%08X/%08X"%(tid[choice-1], tver[choice-1])
	print "Downloading MSET 4.x from the CDN..."
	f = urlopen(url)
	with open(fileName, "wb") as lf:
		lf.write(f.read())
	print "Downloaded!"
except HTTPError, e:
	print "HTTP Error:", e.code, url
except URLError, e:
	print "URL Error:", e.reason, url