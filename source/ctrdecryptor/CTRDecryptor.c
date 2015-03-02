#include "CTRDecryptor.h"
#include "common.h"
#include "screenshot.h"
#include "fs.h"
#include "ff.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "ncch.h"
#include "crypto.h"

#define TITLE "CTR Decryptor\n"
#define BUFFER_ADDR ((volatile u8*)0x21000000)
#define BLOCK_SIZE  (8*1024*1024)
static char workspace[1024];

u32 DecryptPartition(PartitionInfo* info){
    size_t bytesWritten;
	if(info->keyY != NULL)
		setup_aeskey(info->keyslot, AES_BIG_INPUT|AES_NORMAL_INPUT, info->keyY);
    use_aeskey(info->keyslot);

    u8 ctr[16] __attribute__((aligned(32)));
    memcpy(ctr, info->ctr, 16);

    u32 size_bytes = info->size;
    for (u32 i = 0; i < size_bytes; i += BLOCK_SIZE) {
        u32 j;
        for (j = 0; (j < BLOCK_SIZE) && (i+j < size_bytes); j+= 16) {
            set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
            aes_decrypt((void*)info->buffer+j, (void*)info->buffer+j, ctr, 1, AES_CTR_MODE);
            add_ctr(ctr, 1);
			TryScreenShot(); //Putting it here allows us to take screenshots at any decryption point, since everyting loops in this
        }
    }
    return 0;
}

void ProcessExeFS(PartitionInfo* info){ //We expect Exefs to take just a block. Why? No exefs right now reached 8MB.
	if(info->keyslot == 0x2C){ 
		DecryptPartition(info);
	}else if(info->keyslot == 0x25){  //The new keyX is a bit tricky, 'couse only .code is encrypted with it
		PartitionInfo myInfo;
		memcpy((void*)&myInfo, (void*)info, sizeof(PartitionInfo));
		u8 OriginalCTR[16]; memcpy(OriginalCTR, info->ctr, 16);    
		myInfo.keyslot = 0x2C; myInfo.size = 0x200;
		DecryptPartition(&myInfo); add_ctr(myInfo.ctr, 0x200 / 16);
		if(myInfo.buffer[0] == '.' && myInfo.buffer[1] == 'c' && myInfo.buffer[2] == 'o' && myInfo.buffer[3] == 'd' && myInfo.buffer[4] == 'e'){
			//The 7.xKey encrypted .code partition
			u32 codeSize = *((unsigned int*)(myInfo.buffer + 0x0C));
			u32 nextSection = *((unsigned int*)(myInfo.buffer + 0x18)) + 0x200;
			myInfo.buffer += 0x200; myInfo.size = codeSize; myInfo.keyslot = 0x25;
			DecryptPartition(&myInfo);
			//The rest is normally encrypted
			memcpy((void*)&myInfo, (void*)info, sizeof(PartitionInfo));
			myInfo.buffer += nextSection; myInfo.size -= nextSection; myInfo.keyslot = 0x2C;
			myInfo.ctr = OriginalCTR;
			add_ctr(myInfo.ctr, nextSection/16);
			DecryptPartition(&myInfo);
		}else{
			myInfo.size = info->size-0x200;
			myInfo.buffer += 0x200;
			DecryptPartition(&myInfo);
		}
	}
}

int ProcessCTR(char* path){
	PartitionInfo myInfo;
	File myFile;
	char myString[256];  //In case it is needed...
	if(FileOpen(&myFile, path, 0)){
		ConsoleInit();
		ConsoleAddText(TITLE); ConsoleShow();
		unsigned int ncch_base = 0x100;
		unsigned char magic[] = { 0, 0, 0, 0, 0};
		FileRead(&myFile, magic, 4, ncch_base);
		if(magic[0] == 'N' && magic[1] == 'C' && magic[2] == 'S' && magic[3] == 'D'){
			ncch_base = 0x4000;
			FileRead(&myFile, magic, 4, ncch_base+0x100);
			if(!(magic[0] == 'N' && magic[1] == 'C' && magic[2] == 'C' && magic[3] == 'H')){
				FileClose(&myFile);
				return 2;
			}
		}else if(magic[0] == 'N' && magic[1] == 'C' && magic[2] == 'C' && magic[3] == 'H'){
			ncch_base = 0x0;
		}else{
			FileClose(&myFile);
			return 2;
		}
		ctr_ncchheader NCCH; unsigned int mediaunitsize = 0x200;
		FileRead(&myFile, &NCCH, 0x200, ncch_base);
		
		ConsoleAddText(path);
		ConsoleAddText(NCCH.productcode);
		unsigned int NEWCRYPTO = 0, CRYPTO = 1;
		if(NCCH.flags[3] != 0) NEWCRYPTO = 1;
		if(NCCH.flags[7] & 4) CRYPTO = 0;
		if(NEWCRYPTO){
			ConsoleAddText("\nCryptoType : 7.X Key security");
		}else if(CRYPTO){
			ConsoleAddText("\nCryptoType : Secure");
		}else{
			ConsoleAddText("\nCryptoType : None");
			ConsoleAddText("Decryption completed!");
			FileClose(&myFile);
			ConsoleShow();
			return 3;
		}
		
		ConsoleShow();
		u8 CTR[16];
		if(getle32(NCCH.extendedheadersize) > 0){
			ConsoleAddText("Decrypting ExHeader..."); ConsoleShow(); 
			ncch_get_counter(NCCH, CTR, 1); 
			FileRead(&myFile, BUFFER_ADDR, 0x800, ncch_base + 0x200);
			myInfo.buffer = BUFFER_ADDR; 
			myInfo.size = 0x800; 
			myInfo.keyslot = 0x2C; 
			myInfo.ctr = CTR;
			myInfo.keyY = NCCH.signature;
			DecryptPartition(&myInfo);
			FileWrite(&myFile, BUFFER_ADDR, 0x800, ncch_base + 0x200);
		}
		if(getle32(NCCH.exefssize) > 0){
			ConsoleAddText("Decrypting ExeFS..."); ConsoleShow(); 
			ncch_get_counter(NCCH, CTR, 2); 
			myInfo.buffer = BUFFER_ADDR; 
			myInfo.keyslot = NEWCRYPTO ? 0x25 : 0x2C; 
			myInfo.ctr = CTR;
			myInfo.keyY = NCCH.signature;
			
			size_t bytesRead = FileRead(&myFile, BUFFER_ADDR, getle32(NCCH.exefssize) * mediaunitsize, ncch_base + getle32(NCCH.exefsoffset) * mediaunitsize);
			myInfo.size = bytesRead;
			ProcessExeFS(&myInfo); //Explanation at function definition
			FileWrite(&myFile, BUFFER_ADDR, getle32(NCCH.exefssize) * mediaunitsize, ncch_base + getle32(NCCH.exefsoffset) * mediaunitsize);
		}
		if(getle32(NCCH.romfssize) > 0){
			ConsoleAddText("Decrypting RomFS..."); ConsoleShow(); 
			ncch_get_counter(NCCH, CTR, 3); 
			myInfo.buffer = BUFFER_ADDR; 
			myInfo.keyslot = NEWCRYPTO ? 0x25 : 0x2C; 
			myInfo.ctr = CTR;
			myInfo.keyY = NCCH.signature;
			for(int i = 0; i < getle32(NCCH.romfssize) * mediaunitsize / BLOCK_SIZE; i++){
				sprintf(myString, "%i%%", (int)((i*BLOCK_SIZE)/(getle32(NCCH.romfssize) * mediaunitsize/ 100)));
				int x, y; ConsoleGetXY(&x, &y); y += CHAR_WIDTH * 4; x += CHAR_WIDTH*22; 
				DrawString(TOP_SCREEN, myString, x, y, ConsoleGetTextColor(),  ConsoleGetBackgroundColor());
				size_t bytesRead = FileRead(&myFile, BUFFER_ADDR, BLOCK_SIZE, ncch_base + getle32(NCCH.romfsoffset) * mediaunitsize + i*BLOCK_SIZE);
				myInfo.size = bytesRead;
				DecryptPartition(&myInfo);
				add_ctr(myInfo.ctr, bytesRead/16);
				FileWrite(&myFile, BUFFER_ADDR, BLOCK_SIZE, ncch_base + getle32(NCCH.romfsoffset) * mediaunitsize + i*BLOCK_SIZE);
			}
		}
		NCCH.flags[7] |= 4; //Disable encryption
		NCCH.flags[3] = 0;  //Disable 7.XKey usage
		FileWrite(&myFile, &NCCH, 0x200, ncch_base);
		if(ncch_base == 0x4000) FileWrite(&myFile, ((u8*)&NCCH) + 0x100, 0x100, 0x1100);   //Only for NCSD
		FileClose(&myFile);
		ConsoleAddText("Decryption completed!"); ConsoleShow(); 
		return 0;
	}else return 1;
}

void CTRDecryptor(){
	ConsoleInit();
	ConsoleAddText(TITLE); //ConsoleShow();
	int nFiles = 0;
	DIR myDir;
	FILINFO *myInfo = &workspace;  //Explanation : Since i noticed that the filename overflew for the most of the times, i reserved it a bigger space
	
	myInfo->fname[0] = 'A';
	while(f_opendir(&myDir, "") != FR_OK);
	for(int i = 0; myInfo->fname[0] != 0; i++){ 
		f_readdir(&myDir, myInfo);
		if(ProcessCTR(myInfo->fname) == 0){
			nFiles++;
		}
	}
	
	ConsoleInit();
	ConsoleAddText(TITLE);
	sprintf(workspace, "Decrypted %d files\n", nFiles);
	ConsoleAddText(workspace); 
	ConsoleAddText("Press A to exit"); 
	ConsoleShow();
	f_closedir(&myDir);
	WaitForButton(BUTTON_A);
}