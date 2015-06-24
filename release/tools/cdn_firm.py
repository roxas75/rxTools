#!/usr/bin/python
# -*- coding: utf-8 -*-

from urllib2 import urlopen, URLError, HTTPError

try:
	fileName = "firmware.bin";
	print "Downloading 3DS firmware from the CDN..."
	id = 0x49;
	url="http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000002/%08X"%id
	f = urlopen(url)
	with open(fileName, "wb") as lf:
		lf.write(f.read())
	print "Downloaded!"
except HTTPError, e:
	print "HTTP Error:", e.code, url
except URLError, e:
	print "URL Error:", e.reason, url