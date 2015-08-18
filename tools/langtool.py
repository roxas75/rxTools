#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import json
import codecs
from collections import OrderedDict

if len(sys.argv) < 3:
    print "Usage: langtool.py <template file> <language file>"
    exit(0)

try:
    ftemp = codecs.open(sys.argv[1], "r", "utf-8")
    orig = json.load(ftemp, object_pairs_hook=OrderedDict)
    ftemp.close()
    ftemp = codecs.open(sys.argv[2], "r", "utf-8")
    lang = json.load(ftemp, object_pairs_hook=OrderedDict)
    ftemp.close()

    dest = OrderedDict()
    for k, v in orig.iteritems():
        if k in lang:
            dest[k] = lang[k]
        else:
            dest[k] = "TODO:" + v
    for k, v in lang.iteritems():
        if not k in orig:
            print "Warning:", k, "is not in original language template."
            dest[k] = v

    ftemp = codecs.open(sys.argv[2], "w", "utf-8")
    ftemp.write(json.dumps(dest, indent=4, ensure_ascii=False))
    ftemp.close()
except Exception, e:
	print "Error:", e
