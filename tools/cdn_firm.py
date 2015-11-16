#!/usr/bin/env python2
# -*- coding: utf-8 -*-
import os
import urllib
import shutil

firmdir = "firm"
ctrfirmdir = "O3DS_firm_files"
ktrfirmdir = "N3DS_firm_files"

def f(title, download,type):

	if(type=="ctr"):
		urllib.urlretrieve("http://nus.cdn.c.shop.nintendowifi.net/ccs/download/%016X/cetk"
			% (title),
			ctrfirmdir+"/"+firmdir + "/%016X_cetk.bin" % (title))
		return urllib.urlretrieve("http://nus.cdn.c.shop.nintendowifi.net/ccs/download/%016X/%08X"
			% (title, download),
			ctrfirmdir+"/"+firmdir + "/%016X.bin" % (title))
	else:
		urllib.urlretrieve("http://nus.cdn.c.shop.nintendowifi.net/ccs/download/%016X/cetk"
			% (title),
			ktrfirmdir+"/"+firmdir + "/%016X_cetk.bin" % (title))
		return urllib.urlretrieve("http://nus.cdn.c.shop.nintendowifi.net/ccs/download/%016X/%08X"
			% (title, download),
			ktrfirmdir+ "/"+firmdir + "/%016X.bin" % (title))

	
print "Downloading 3DS firmware from the CDN..."
if os.path.isdir(ctrfirmdir):
	shutil.rmtree(ctrfirmdir)
if os.path.isdir(ktrfirmdir):
	shutil.rmtree(ktrfirmdir)
os.mkdir(ctrfirmdir)
os.mkdir(ctrfirmdir+"/"+firmdir)
os.mkdir(ktrfirmdir)
os.mkdir(ktrfirmdir+"/"+firmdir)
f(0x0004013800000002, 0x00000049, "ctr")
f(0x0004013800000102, 0x00000016, "ctr")
f(0x0004013800000202, 0x0000000B, "ctr")
#f(0x0004013820000002, 0xXXXXXXXX, "ktr") the relevant file is no longer available on NUS servers, still here for legacy reasons.
f(0x0004013820000102, 0x00000000, "ktr")
f(0x0004013820000202, 0x00000000, "ktr")
print "Downloaded!"