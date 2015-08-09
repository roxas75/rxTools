import sys
import os
import struct
import hashlib


def round_up(numToRound, multiple):  #From http://stackoverflow.com/a/3407254
    if multiple == 0:
        return numToRound

    remainder = abs(numToRound) % multiple
    if remainder == 0:
        return numToRound
    if numToRound < 0:
        return -(abs(numToRound) - remainder)
    return numToRound + multiple - remainder


# Part of this code from https://stackoverflow.com/questions/120656/directory-listing-in-python
def parse_dir(foldername, ctrlist):
    savedpath = os.getcwd()
    try:
        os.chdir('./' + foldername)
    except Exception as ex:
        return

    for dirname, dirnames, filenames in os.walk('.'):
        for filename in filenames:
            if 'quota.dat' in filename.lower():
                continue

            fsizeMB = round_up(os.stat(os.path.join(dirname, filename)).st_size, 1024*1024) / (1024*1024)
            fname = '/' + foldername + os.path.join(dirname, filename)[1:].replace('\\','/')

            digest = hashlib.sha256(fname.encode('utf-16le') + '\x00\x00').digest()
            x = struct.unpack("<LLLLLLLL", digest)
            ctr = struct.pack("<LLLL", x[0] ^ x[4], x[1] ^ x[5], x[2] ^ x[6], x[3] ^ x[7])

            fname = '/' + fname[1:].replace('/','.') + '.xorpad'
            if len(fname) > 180:
                print "Filename too long. This shouldn't happen."
                print '%s' % fname
                raise SystemExit(0)
            ctrlist.append([ctr, fname, fsizeMB, fname])
    os.chdir(savedpath)


if len(sys.argv) < 2:
    print 'usage: sdinfo_gen.py folderpath'
    print 'folderpath: folder on your SD that contains "dbs", "title", etc.'
    print 'For example, sdinfo_gen.py "X:/Nintendo 3DS/xxxxxxxxx/xxxxxxxx/"'
    raise SystemExit(0)

savedpath = os.getcwd()
try:
    os.chdir(sys.argv[1])
except Exception as ex:
    print str(ex)
    raise SystemExit(0)

ctrlist = []

parse_dir('dbs', ctrlist)
parse_dir('extdata', ctrlist)
parse_dir('title', ctrlist)
# parsedir('backups', ctrlist)  # Untested

os.chdir(savedpath)

if not ctrlist == []:
    fh = open('SDinfo.bin', 'wb')
    listlen = len(ctrlist)
    fh.write(struct.pack('<L', listlen))  # Number of entries
    for x in ctrlist:
        fh.write(x[0])  # Counter
        fh.write(struct.pack('<L', x[2]))  # File size in MB (rounded up)
        fh.write(x[1])  # Output filename
        if len(x[1]) < 180:
            fh.write('\x00'*(180-len(x[1])))  # Pad filename out to 180 bytes
    print 'SDinfo.bin generated.'
else:
    print "Couldn't find any content. Wrong folder?"
