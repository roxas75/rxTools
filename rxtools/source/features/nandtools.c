#include "common.h"
#include "menu.h"
#include "nandtools.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "fs.h"
#include "screenshot.h"
#include "padgen.h"
#include "crypto.h"
#include "ncch.h"
#include "NandDumper.h"

#define nCoolFiles sizeof(CoolFiles)/sizeof(CoolFiles[0])

u32 selectedFile;
void SelectFile();

static struct {
    char* name;
    char* path;
} CoolFiles[] = {
    "movable.sed", "private/movable.sed",
    "SecureInfo_A", "rw/sys/SecureInfo_A",
    "LocalFriendCodeSeed_B", "rw/sys/LocalFriendCodeSeed_B",
    "rand_seed", "rw/sys/rand_seed",
    "ticket.db", "dbs/ticket.db",
};

static Menu CoolFilesMenu = {
	"Choose the file to work on",
	{
        " movable.sed", &SelectFile,
        " SecureInfo_A", &SelectFile,
        " LocalFriendCodeSeed_B", &SelectFile,
        " rand_seed", &SelectFile,
        " ticket.db", &SelectFile,
	},
	nCoolFiles,
	0,
	0
};

void SelectFile(){
    selectedFile = CoolFilesMenu.Current;
}

char tmpstr[256];

void dumpCoolFiles()
{
	int nandtype = NandSwitch();
	if (nandtype == UNK_NAND) return;

	selectedFile = -1;
	MenuInit(&CoolFilesMenu);
	MenuShow();

	while (true)
	{
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) MenuNextSelection();
		if (pad_state & BUTTON_UP) MenuPrevSelection();
		if (pad_state & BUTTON_A) { MenuSelect(); break; }
		if (pad_state & BUTTON_B) break;
		TryScreenShot();
		MenuShow();
	}

	if (selectedFile == -1) return;
	ConsoleInit();
	ConsoleSetTitle("File Dumper: %s", CoolFiles[selectedFile].name);

	char dest[256];
	sprintf(dest, "rxTools/%s", CoolFiles[selectedFile].name);
	sprintf(tmpstr, "%d:%s", nandtype, CoolFiles[selectedFile].path);
	print("Dumping...\n"); ConsoleShow();

	char *showres;
	int res = FileCopy(dest, tmpstr);
	if (res < 0 && selectedFile == 1)
	{
		/* Fix for SecureInfo_B */
		print("Error. Trying with SecureInfo_B...\n");
		sprintf(dest, "rxTools/%.11s%c", CoolFiles[selectedFile].name, 'B');
		sprintf(tmpstr, "%d:%.18s%c", nandtype, CoolFiles[selectedFile].path, 'B');
		res = FileCopy(dest, tmpstr);
	} else
	if (res < 0 && selectedFile == 2)
	{
		/* Fix for LocalFriendCodeSeed_A */
		print("Error. Trying with LocalFriendCodeSeed_A...\n");
		sprintf(dest, "rxTools/%.20s%c", CoolFiles[selectedFile].name, 'A');
		sprintf(tmpstr, "%d:%.27s%c", nandtype, CoolFiles[selectedFile].path, 'A');
		res = FileCopy(dest, tmpstr);
	}

	switch(res)
	{
		case 1:
			showres = "Success!";
			break;
		case -1:
			showres = "Cannot write to file!";
			break;
		case -2:
			showres = "Cannot read from file!";
			break;
		default:
			showres = "Failure!";
			break;
	}

	print(showres);
	print("\n\nPress A to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}

void restoreCoolFiles()
{
	int nandtype = NandSwitch();
	if (nandtype == UNK_NAND) return;

	selectedFile = -1;
	MenuInit(&CoolFilesMenu);
	MenuShow();
	while (true)
	{
		u32 pad_state = InputWait();
		if (pad_state & BUTTON_DOWN) MenuNextSelection();
		if (pad_state & BUTTON_UP) MenuPrevSelection();
		if (pad_state & BUTTON_A) { MenuSelect(); break; }
		if (pad_state & BUTTON_B) break;
		TryScreenShot();
		MenuShow();
	}

	if (selectedFile == -1) return;
	ConsoleInit();
	ConsoleSetTitle("File Inject: %s", CoolFiles[selectedFile].name);

	char dest[256];
	sprintf(tmpstr, "rxTools/%s", CoolFiles[selectedFile].name);
	sprintf(dest, "%d:%s", nandtype, CoolFiles[selectedFile].path);
	print("Injecting...\n"); ConsoleShow();

	char *showres;
	int res = FileCopy(dest, tmpstr);
	if (res < 0 && selectedFile == 1)
	{
		/* Fix for SecureInfo_B */
		print("Error. Trying with SecureInfo_B...\n");
		sprintf(tmpstr, "rxTools/%.11s%c", CoolFiles[selectedFile].name, 'B');
		sprintf(dest, "%d:%.18s%c", nandtype, CoolFiles[selectedFile].path, 'B');
		res = FileCopy(dest, tmpstr);
	} else
	if (res < 0 && selectedFile == 2)
	{
		/* Fix for LocalFriendCodeSeed_A */
		print("Error. Trying with LocalFriendCodeSeed_A...\n");
		sprintf(tmpstr, "rxTools/%.20s%c", CoolFiles[selectedFile].name, 'A');
		sprintf(dest, "%d:%.27s%c", nandtype, CoolFiles[selectedFile].path, 'A');
		res = FileCopy(dest, tmpstr);
	}

	switch (res)
	{
		case 1:
			showres = "Success!";
			break;
		case -1:
			showres = "Cannot write to file!";
			break;
		case -2:
			showres = "Cannot read from file!";
			break;
		default:
			showres = "Failure!";
			break;
	}

	print(showres);
	print("\n\nPress A to exit\n");
	ConsoleShow();
	WaitForButton(BUTTON_A);
}
