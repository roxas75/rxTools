#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <malloc.h>
#include <tchar.h>
#include <urlmon.h>
#include <windows.h>

int main()
{
	char FileName[] = "msetdg.bin";
	char urlname[256];
	unsigned int tid[] = { 0x00020000, 0x00021000, 0x00022000}; //JPN, USA, EUR
	unsigned int tver[] = { 0x1A, 0x1F, 0x19};
	int choice = 0;
	do{
		system("cls");
		printf("Select your 3DS region :\n    [1] JPN\n    [2] USA\n    [3] EUR\n\nSelection : ");
		scanf("%d", &choice);
	}while(!choice || choice > 3);

	printf("Downloading MSET 4.x from the CDN...\n");
	sprintf(urlname, "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/00040010%08X/%08X", tid[choice-1], tver[choice-1]);
	HRESULT hr = URLDownloadToFile( NULL, urlname, FileName, 0, NULL );
	if(hr == S_OK){
		printf("Downloaded!");
	}
	return 0;
}
