#!/usr/bin/env python2
# -*- coding: utf-8 -*-
import os
import urllib
import shutil

firmdir = "firm"

def f(title, download):
	return urllib.urlretrieve("http://nus.cdn.c.shop.nintendowifi.net/ccs/download/%016X/%08X"
		% (title, download),
		firmdir + "/%016X.bin" % (title))

print "Downloading 3DS firmware from the CDN..."
if os.path.isdir(firmdir):
	shutil.rmtree(firmdir)
os.mkdir(firmdir)
f(0x0004013820000002, 0x00000018)
#f(0x0004013820000202, 0x00000000)
#f(0x0004013820000102, 0x00000000)
print "Downloaded!"