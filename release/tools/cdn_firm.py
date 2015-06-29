#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys;
import os
from urllib2 import urlopen, URLError, HTTPError

try:
	fileName = "firmware.bin";
	print "Downloading 3DS firmware from the CDN..."
	id = 0x49;
	url="http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000002/00000049"
	f = urlopen(url)
	with open(fileName, "wb") as lf:
		lf.write(f.read())
		
	url="http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000202/0000000B"
	f = urlopen(url)
	with open(fileName, "rb+") as lf:
		lf.seek(0x200000, os.SEEK_SET)
		lf.write(f.read())
		
	url="http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000102/00000016"
	f = urlopen(url)
	with open(fileName, "rb+") as lf:
		lf.seek(0x400000, os.SEEK_SET)
		lf.write(f.read())
	
	print "Downloaded!"
except HTTPError, e:
	print "HTTP Error:", e.code, url
except URLError, e:
	print "URL Error:", e.reason, url