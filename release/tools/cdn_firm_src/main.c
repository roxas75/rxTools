#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <malloc.h>
#include <tchar.h>
#include <urlmon.h>
#include <windows.h>

int main()
{
	char FileName[] = "firmware.bin";
	char urlname[256];
	printf("Downloading 3DS firmware from the CDN...\n");
	int id = 0x49;
	sprintf(urlname, "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000002/%08X", id);
	HRESULT hr = URLDownloadToFile( NULL, urlname, FileName, 0, NULL );
	if(hr == S_OK){
		printf("Downloaded!");
	}
	return 0;
}
