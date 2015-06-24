import sys
import random
import struct

#crappy obfuscation, but doesn't really cost us anything

magicWord=0xDEADBEEF

def getWord(b, k, n=4):
	return sum(list(map(lambda c: b[k+c]<<(c*8),range(n))))

def putWord(b, k, v):
	b[(k):(k+4)]=struct.pack("I",v)

#todo : remove doubles ?
def findWords(d):
	l=[]
	for k in range(0,len(d)&~0x3,4):
		v=getWord(d,k)
		if v!=magicWord and v!=0x0 and (v>>28)!=0xE:
			l.append(v)
	return l

def obfuscate(d,l):
	for k in range(0,len(d)&~0x3,4):
		v=getWord(d,k)
		if v==magicWord:
			v=random.choice(l)
			putWord(d,k,v)

data=bytearray(open(sys.argv[1],"rb").read())

words=findWords(data)
obfuscate(data,words)
# print("OBFUSCATOR 5000 TEMPORARILY DISABLED")
# print("DO NOT FORGET TO REENABLE")

open(sys.argv[1], "wb").write(data)
