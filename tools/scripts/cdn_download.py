# This script is old and shitty

import os
import errno
import sys
import urllib2
from struct import unpack
from binascii import hexlify
from hashlib import sha256
from Crypto.Cipher import AES


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


# ######## # From https://stackoverflow.com/questions/5783517/downloading-progress-bar-urllib2-python
def chunk_report(bytes_so_far, chunk_size, total_size):
    percent = float(bytes_so_far) / total_size
    percent = round(percent*100, 2)
    sys.stdout.write("Downloaded %d of %d bytes (%0.2f%%)\r" % (bytes_so_far, total_size, percent))

    if bytes_so_far >= total_size:
        sys.stdout.write('\n')


def chunk_read(response, outfname, chunk_size=2*1024*1024, report_hook=None):
    fh = open(outfname, 'wb')
    total_size = response.info().getheader('Content-Length').strip()
    total_size = int(total_size)
    bytes_so_far = 0

    while 1:
        if report_hook:
            report_hook(bytes_so_far, chunk_size, total_size)

        chunk = response.read(chunk_size)
        bytes_so_far += len(chunk)
        if not chunk:
            break

        fh.write(chunk)
    fh.close()
# ######## #


def display_usage_info():
    print 'Usage: cdn_download.py TitleID TitleKey [-redown -redec]'
    print '-redown : redownload content'
    print '-redec  : re-attempt content decryption'
    raise SystemExit(0)

if len(sys.argv) < 3:
    display_usage_info()

titleid = sys.argv[1]
titlekey = sys.argv[2]
forceDownload = 0
forceDecrypt = 0

for i in xrange(len(sys.argv)):
    if sys.argv[i] == '-redown':
        forceDownload = 1
    elif sys.argv[i] == '-redec':
        forceDecrypt = 1

if len(titleid) != 16 or len(titlekey) != 32:
    print 'Invalid arguments'
    raise SystemExit(0)

baseurl = 'http://nus.cdn.c.shop.nintendowifi.net/ccs/download/' + titleid

print 'Downloading TMD...'

try:
    tmd = urllib2.urlopen(baseurl + '/tmd')
except urllib2.URLError, e:
    print 'ERROR: Bad title ID?'
    raise SystemExit(0)

tmd = tmd.read()

mkdir_p(titleid)
open(titleid + '/tmd', 'wb').write(tmd)
print 'Done\n'

if tmd[:4] != '\x00\x01\x00\x04':
    print 'Unexpected signature type.'
    raise SystemExit(0)
    
# Set Proper Version
version = unpack('>H', tmd[0x1dc:0x1de])[0]
# Set Save Size
saveSize = (unpack('<I', tmd[0x19a:0x19e])[0])/1024
    
contentCount = unpack('>H', tmd[0x206:0x208])[0]

print 'Content count: ' + str(contentCount) + '\n'

    
# Download Contents
fSize = 16*1024
for i in xrange(contentCount):
    cOffs = 0xB04+(0x30*i)
    cID = format(unpack('>I', tmd[cOffs:cOffs+4])[0], '08x')
    cIDX = format(unpack('>H', tmd[cOffs+4:cOffs+6])[0], '04x')
    cSIZE = format(unpack('>Q', tmd[cOffs+8:cOffs+16])[0], 'd')
    cHASH = format(unpack('>32s', tmd[cOffs+16:cOffs+48])[0])
    
    print 'Content ID:    ' + cID
    print 'Content Index: ' + cIDX
    print 'Content Size:  ' + cSIZE
    print 'Content Hash:  ' + hexlify(cHASH)

    aes_obj = AES.new(titlekey.decode("hex"), AES.MODE_CBC, (cIDX + '0000000000000000000000000000').decode("hex"))

    outfname = titleid + '/' + cID
    if os.path.exists(outfname) == 0 or forceDownload == 1\
            or os.path.getsize(outfname) != unpack('>Q', tmd[cOffs+8:cOffs+16])[0]:
        response = urllib2.urlopen(baseurl + '/' + cID)
        chunk_read(response, outfname, report_hook=chunk_report)

        # If we redownloaded the content, then decrypting it is implied.
        with open(outfname, 'rb') as fo:
            ciphertext = fo.read()
        dec = aes_obj.decrypt(ciphertext)
        with open(outfname + '.dec', 'wb') as fo:
            fo.write(dec)
    
    elif os.path.exists(outfname + '.dec') == 0 or forceDecrypt == 1 \
            or os.path.getsize(outfname + '.dec') != unpack('>Q', tmd[cOffs+8:cOffs+16])[0]:
        with open(outfname, 'rb') as fo:
            ciphertext = fo.read()
        dec = aes_obj.decrypt(ciphertext)
        with open(outfname + '.dec', 'wb') as fo:
            fo.write(dec)

    with open(outfname + '.dec', 'rb') as fh:
        fh.seek(0, os.SEEK_END)
        fhSize = fh.tell()
        if fh.tell() != unpack('>Q', tmd[cOffs+8:cOffs+16])[0]:
            print 'Title size mismatch.  Download likely incomplete'
            print 'Downloaded: ' + format(fh.tell(), 'd')
            raise SystemExit(0)
        fh.seek(0)
        hash = sha256()
        
        while fh.tell() != fhSize:
            hash.update(fh.read(0x1000000))
            print 'checking hash: ' + format(float(fh.tell()*100)/fhSize, '.1f') + '% done\r',
            
        sha256file = hash.hexdigest()
        if sha256file != hexlify(cHASH):
            print 'hash mismatched, Decryption likely failed, wrong key?'
            print 'got hash: ' + sha256file
            raise SystemExit(0)
        fh.seek(0x100)
        if fh.read(4) != 'NCCH':
            fh.seek(0x60)
            if fh.read(4) != 'WfA\0':
                print 'Not NCCH, nor DSiWare, file likely corrupted'
                raise SystemExit(0)
            else:
                print 'Not an NCCH container, likely DSiWare'
        fh.seek(0, os.SEEK_END)
        fSize += fh.tell()
        
    print '\n'
    
print 'Done!'
