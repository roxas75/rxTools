#####
#ncchinfo.bin format
#
#4 bytes = 0xFFFFFFFF Meant to prevent previous versions of padgen from using these new files
#4 bytes   ncchinfo.bin version or'd with 0xF0000000, to prevent previous versions of padgen from using these new files
#4 bytes   Number of entries
#4 bytes   Reserved
#
#entry (160 bytes in size):
#  16 bytes   Counter
#  16 bytes   KeyY
#   4 bytes   Size in MB(rounded up)
#   8 bytes   Reserved
#   4 bytes   Uses 7x crypto? (0 or 1)
# 112 bytes   Output file name in UTF-16 (format used: "/titleId.partitionName.sectionName.xorpad")
#####
#seedinfo.bin format
#
#  4  bytes   Magic "SEED"
#  4  bytes   Number of Seeds
#  8  bytes*(Number of Seeds)   TitleID
# 16  bytes*(Number of Seeds)   Seeds
#####
#SEEDDB format
#
#    4  bytes   File index
#    1  byte    "!" as the first file name
#   47  bytes   The first file
#    6  bytes   "SEEDDB" as the second file name
#   18  bytes   Unknown_0
#    4  bytes   SEEDDB file data offset (This value minus 1 then multiply by 0x1000 is the offset)
#    4  bytes   SEEDDB file size
# 4012  bytes   Unknown_1 (Padding?)
#    4  bytes   Number of seeds(?) Need more files for analizing.
# 4092  bytes   Unknown_2 (Padding?)
# 2000  dwords  Title IDs (1 dword per ID)
# 4000  dwords  SEEDs (2 dwords per SEED)
#####

import os
import sys
import glob
import struct
import fnmatch
from hashlib import sha256
from ctypes import *
from binascii import hexlify

mediaUnitSize = 0x200

class ncchHdr(Structure):
    _fields_ = [
        ('signature', c_uint8 * 0x100),
        ('magic', c_char * 4),
        ('ncchSize', c_uint32),
        ('titleId', c_uint8 * 0x8),
        ('makerCode', c_uint16),
        ('formatVersion', c_uint8),
        ('formatVersion2', c_uint8),
        ('seedcheck', c_char * 4),
        ('programId', c_uint8 * 0x8),
        ('padding1', c_uint8 * 0x10),
        ('logoHash', c_uint8 * 0x20),
        ('productCode', c_uint8 * 0x10),
        ('exhdrHash', c_uint8 * 0x20),
        ('exhdrSize', c_uint32),
        ('padding2', c_uint32),
        ('flags', c_uint8 * 0x8),
        ('plainRegionOffset', c_uint32),
        ('plainRegionSize', c_uint32),
        ('logoOffset', c_uint32),
        ('logoSize', c_uint32),
        ('exefsOffset', c_uint32),
        ('exefsSize', c_uint32),
        ('exefsHashSize', c_uint32),
        ('padding4', c_uint32),
        ('romfsOffset', c_uint32),
        ('romfsSize', c_uint32),
        ('romfsHashSize', c_uint32),
        ('padding5', c_uint32),
        ('exefsHash', c_uint8 * 0x20),
        ('romfsHash', c_uint8 * 0x20),
    ]

class ncchSection:
    exheader = 1
    exefs = 2
    romfs = 3

class ncch_offsetsize(Structure):
    _fields_ = [
        ('offset', c_uint32),
        ('size', c_uint32),
    ]

class ncsdHdr(Structure):
    _fields_ = [
        ('signature', c_uint8 * 0x100),
        ('magic', c_char * 4),
        ('mediaSize', c_uint32),
        ('titleId', c_uint8 * 0x8),
        ('padding0', c_uint8 * 0x10),
        ('offset_sizeTable', ncch_offsetsize * 0x8),
        ('padding1', c_uint8 * 0x28),
        ('flags', c_uint8 * 0x8),
        ('ncchIdTable', c_uint8 * 0x40),
        ('padding2', c_uint8 * 0x30),
    ]

class SeedError(Exception):
        pass

ncsdPartitions = [b'Main', b'Manual', b'DownloadPlay', b'Partition4', b'Partition5', b'Partition6', b'Partition7', b'UpdateData']

def roundUp(numToRound, multiple):  #From http://stackoverflow.com/a/3407254
    if (multiple == 0):
        return numToRound
    
    remainder = abs(numToRound) % multiple
    if (remainder == 0):
        return numToRound
    if (numToRound < 0):
        return -(abs(numToRound) - remainder)
    return numToRound + multiple - remainder

def reverseCtypeArray(ctypeArray): #Reverses a ctype array and converts it to a hex string.
    return ''.join('%02X' % x for x in ctypeArray[::-1])
    #Is there a better way to do this?

def getNcchAesCounter(header, type): #Function based on code from ctrtool's source: https://github.com/Relys/Project_CTR
    counter = bytearray(b'\x00' * 16)
    if header.formatVersion == 2 or header.formatVersion == 0:
        counter[:8] = bytearray(header.titleId[::-1])
        counter[8:9] = chr(type)
    elif header.formatVersion == 1:
        x = 0
        if type == ncchSection.exheader:
            x = 0x200 #ExHeader is always 0x200 bytes into the NCCH
        if type == ncchSection.exefs:
            x = header.exefsOffset * mediaUnitSize
        if type == ncchSection.romfs:
            x = header.romfsOffset * mediaUnitSize
        counter[:8] = bytearray(header.titleId)
        for i in xrange(4):
            counter[12+i] = chr((x>>((3-i)*8)) & 0xFF)
    
    return bytes(counter)

def getNewkeyY(keyY,header,titleId):
    tids = []
    seeds = []
    seedif = os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), 'seedinfo.bin')
    if os.path.exists(seedif):
        with open(seedif,'rb')as seedinfo:
            if not seedinfo.read(4) == 'SEED':
                raise SeedError("Not as seedinfo!")
            seedcount = struct.unpack('<I',seedinfo.read(4))[0]

            for i in range(seedcount):
                tids.append(seedinfo.read(8))
            for i in range(seedcount):
                seeds.append(bytearray(seedinfo.read(16)))
    else:
        #Read seeds directly from savedata.You should dump savedata files from "nand:/data/<console-unique>/sysdata/0001000f/" and rename as *.sav
        filenames = os.listdir(os.path.dirname(os.path.realpath(sys.argv[0])))
        x = 0
        for fn in filenames:
            fn = os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), fn)
            if fnmatch.fnmatch(fn,'*.sav'):
                with open(fn,'rb') as savefile:
                    savedata = savefile.read()
                    while savedata.find('SEEDDB')>=0:
                        SBoffset = savedata.find('SEEDDB')
                        a = savedata[SBoffset+24:SBoffset+28]
                        tidoffset = (struct.unpack('<I',savedata[SBoffset+24:SBoffset+28])[0] - 1) * 4096 + SBoffset - 52 + 4096
                        for i in range(2000):
                            tid = savedata[tidoffset:tidoffset+8]
                            tids.append(tid)
                            tidoffset += 8
                        for i in range(2000):            #Seeds upper limit is 2000, hard coding
                            seed = savedata[tidoffset:tidoffset+16]
                            seeds.append(bytearray(seed))
                            tidoffset += 16
                        savedata = savedata[tidoffset:]
            else:
                x += 1
        if not x < len(filenames):
            raise SeedError("Can't find SEEDDB file!")
    if not len(tids) == len(seeds):
        raise SeedError('Seed info incomplete!')
    for i in range(len(seeds)):
        if tids[i] == titleId:
            seedcheck = struct.unpack('>I',header.seedcheck)[0]
            if int(sha256(seeds[i] + tids[i]).hexdigest()[:8],16) == seedcheck:
                keystr = sha256(keyY + seeds[i]).hexdigest()[:32]
                v = []
                for j in range(0,32,8):
                    v.append(int(keystr[j:j+8],16))
                w = []
                for j in v:
                    w.append(struct.pack('>I',j))
                newkeyY = ''
                for j in w:
                    newkeyY += j
                return bytearray(newkeyY)
            else:
                raise SeedError('Seed check fail, wrong seed?')
    raise SeedError("Can't find SEEDDB file!\nPlease dump savedata files from \n(nand:/data/<console-unique>/sysdata/0001000f/) and rename as *.sav")

def parseNCSD(fh):
    print 'Parsing NCSD in file "%s":' % os.path.basename(fh.name)
    entries = 0
    data = ''
    
    fh.seek(0)
    header = ncsdHdr()
    fh.readinto(header) #Reads header into structure
    
    for i in xrange(len(header.offset_sizeTable)):
        if header.offset_sizeTable[i].offset:
            result = parseNCCH(fh, header.offset_sizeTable[i].offset * mediaUnitSize, i, reverseCtypeArray(header.titleId), 0)
            entries += result[0]
            data = data + result[1]
    return [entries, data]

def parseNCCH(fh, offs=0, idx=0, titleId='', standAlone=1):
    tab = '    ' if not standAlone else '  '
    if not standAlone:
        print '  Parsing %s NCCH' % ncsdPartitions[idx]
    else:
        print 'Parsing NCCH in file "%s":' % os.path.basename(fh.name)
    entries = 0
    data = ''
    
    fh.seek(offs)
    header = ncchHdr()
    fh.readinto(header) #Reads header into structure
    
    if titleId == '':
        titleId = reverseCtypeArray(header.programId)   #Use ProgramID instead, is it OK?
    
    keyY = bytearray(header.signature[:16])
    
    if not standAlone:
        print tab + 'NCCH Offset: %08X' % offs
    print tab + 'Product code: ' + str(bytearray(header.productCode)).rstrip('\x00')
    if not standAlone:
        print tab + 'Partition number: %d' % idx
    print tab + 'KeyY: %s' % hexlify(keyY).upper()
    print tab + 'Title ID: %s' % reverseCtypeArray(header.titleId)
    print tab + 'Format version: %d' % header.formatVersion
    
    uses7xCrypto = bytearray(header.flags)[3]
    if uses7xCrypto:
        print tab + 'Uses 7.x NCCH crypto'
    a = header.flags[7]
    useSeedCrypto = header.flags[7] == 32
    if useSeedCrypto:
        keyY = getNewkeyY(keyY,header,struct.pack('I',int(titleId[8:],16))+struct.pack('I',int(titleId[:8],16)))
        print tab + 'Use Seed NCCH crypto'
        print tab + 'Seed KeyY: %s' % hexlify(keyY).upper()
    
    print ''
    
    if header.exhdrSize:
        data = data + parseNCCHSection(header, ncchSection.exheader, 0, 0, 1, tab)
        data = data + genOutName(titleId, ncsdPartitions[idx], b'exheader')
        entries += 1
        print ''
    if header.exefsSize: #We need generate two xorpads for exefs if it uses 7.x crypto, since only a part of it uses the new crypto.
        data = data + parseNCCHSection(header, ncchSection.exefs, 0, 0, 1, tab)
        data = data + genOutName(titleId, ncsdPartitions[idx], b'exefs_norm')
        entries += 1
        if uses7xCrypto:
            data = data + parseNCCHSection(header, ncchSection.exefs, uses7xCrypto, useSeedCrypto, 0, tab)
            data = data + genOutName(titleId, ncsdPartitions[idx], b'exefs_7x')
            entries += 1
        print ''
    if header.romfsSize:
        data = data + parseNCCHSection(header, ncchSection.romfs, uses7xCrypto, useSeedCrypto, 1, tab)
        data = data + genOutName(titleId, ncsdPartitions[idx], b'romfs')
        entries += 1
        print ''
    
    print ''
    
    return [entries, data]

def parseNCCHSection(header, type, uses7xCrypto, useSeedCrypto, doPrint, tab):
    if type == ncchSection.exheader:
        sectionName = 'ExHeader'
        offset = 0x200 #Always 0x200
        sectionSize = header.exhdrSize * mediaUnitSize
    elif type == ncchSection.exefs:
        sectionName = 'ExeFS'
        offset = header.exefsOffset * mediaUnitSize
        sectionSize = header.exefsSize * mediaUnitSize
    elif type == ncchSection.romfs:
        sectionName = 'RomFS'
        offset = header.romfsOffset * mediaUnitSize
        sectionSize = header.romfsSize * mediaUnitSize
    else:
        print 'Invalid NCCH section type was somehow passed in. :/'
        sys.exit()
    
    counter = getNcchAesCounter(header, type)
    keyY = bytearray(header.signature[:16])
    
    titleId = reverseCtypeArray(header.programId)
    if useSeedCrypto:
        keyY = getNewkeyY(keyY,header,struct.pack('I',int(titleId[8:],16))+struct.pack('I',int(titleId[:8],16)))
    
    sectionMb = roundUp(sectionSize, 1024*1024) / (1024*1024)
    if sectionMb == 0:
        sectionMb = 1 #Should never happen, but meh.
    
    if doPrint:
        print tab + '%s offset:  %08X' % (sectionName, offset)
        print tab + '%s counter: %s' % (sectionName, hexlify(counter))
        print tab + '%s Megabytes(rounded up): %d' % (sectionName, sectionMb)
    
    return struct.pack('<16s16sIIII', str(counter), str(keyY), sectionMb, 0, 0, uses7xCrypto)

def genOutName(titleId, partitionName, sectionName):
    outName = b'/%s.%s.%s.xorpad' % (titleId, partitionName, sectionName)
    if len(outName) > 112:
        print "Output file name too large. This shouldn't happen."
        sys.exit()
    
    return outName + (b'\x00'*(112-len(outName))) #Pad out so whole entry is 160 bytes (48 bytes are set before filename)



if __name__ == "__main__":
    sys.argv.append('00000001.dec')
    if len(sys.argv) < 2:
        print 'usage: ctrKeyGen.py files..'
        print '  Supports parsing both CCI(.3ds) and NCCH files.'
        print '  Wildcards are supported'
        print '  Example: ctrKeyGen.py *.ncch SM3DL.3ds'
        sys.exit()
    
    inpFiles = []
    existFiles = []
    
    for i in xrange(len(sys.argv)-1):
        inpFiles = inpFiles + glob.glob(sys.argv[i+1].replace('[','[[]')) #Needed for wildcard support on Windows
    
    for i in xrange(len(inpFiles)):
        if os.path.isfile(inpFiles[i]):
            existFiles.append(inpFiles[i])
    
    if existFiles == []:
        print "Input files don't exist"
        sys.exit()
    
    print ''
    
    entries = 0
    data = ''
    
    for file in existFiles:
        result = []
        
        with open(file,'rb') as fh:
            fh.seek(0x100)
            magic = fh.read(4)
            if magic == b'NCSD':
                result = parseNCSD(fh)
                print ''
            elif magic == b'NCCH':
                result = parseNCCH(fh)
                print ''
        
        if result:
            entries += result[0]
            data = data + result[1]
    
    dndFix = os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), 'ncchinfo.bin') #Fix drag'n'drop
    with open(dndFix, 'wb') as fh:
        fh.write(struct.pack('<IIII',0xFFFFFFFF, 0xF0000003, entries, 0))
        fh.write(data)
    
    print 'Done!'
