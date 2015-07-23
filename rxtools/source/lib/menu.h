#ifndef MENU_H
#define MENU_H

#include "common.h"
#include "console.h"

typedef struct{
	char* Str;
	void(* Func)();
	char* gfx_splash;
}MenuEntry;

typedef struct{
	char* Name;
	MenuEntry* Option;
	int nEntryes;
	int Current;    //The current selected option
	bool Showed;    //Useful, to not refresh everything everytime
} Menu;

void MenuInit(Menu* menu);
void MenuShow();
void MenuNextSelection();
void MenuPrevSelection();
void MenuSelect();
void MenuClose();
void MenuRefresh();

extern bool bootGUI;
extern char Theme;
extern bool agb_bios;
extern bool theme_3d;
extern bool silent_boot;
extern char language;
extern Menu* MyMenu;


// Translations and corrections by: nastys, Crusard, Pikawil, DjoeN, VinsCool, R4ndom, guitarheroknight, Asia81.
static int N_LANG=6;

static char* STR_LANGUAGES[]={"Eng", "Ita", "Esp", "Fra", "NLD", "Nor", "Hrv"};
static char* STR_FORCE_UI_BOOT[]={"Force UI boot:      < %s > ", "Forza avvio UI:     < %s > ", "Iniciar en UI:      < %s > ", "Forcer demarrage UI < %s > ", "Start in UI:        < %s >", "Tving oppstart av UI< %s >", "Forsiraj ulazak u UI< %s >"};
static char* STR_SELECTED_THEME[]={"Selected theme:     <  %c  > ", "Tema selezionato:   <  %c  > ", "Tema seleccionado:  <  %c  > ", "Theme choisi :      <  %c  > ", "Selecteer thema:    <  %c  >", "Valgt tema:         <  %c  > ", "Izabrana tema:      <  %c  > "};
static char* STR_SHOW_AGB[]={"Show AGB_FIRM BIOS: < %s > ", "Mostra BIOS AGB_FIRM< %s > ", "Ver AGB_FIRM BIOS:  < %s > ", "Activer AGB_FIRM :  < %s > ", "Toon AGB_FIRM BIOS: < %s > ", "Vis AGB_FIRM BIOS:  < %s > ", "Prikazi AGB_FIRM:   < %s > "};
static char* STR_ENABLE_3D_UI[]={"Enable 3D UI:       < %s > ", "Attiva UI 3D:       < %s > ", "Habilitar 3D UI:    < %s > ", "Activer UI 3D :     < %s > ", "Gebruik 3D UI:      < %s > ", "Aktiver UI i 3D:    < %s > ", "Ukljuci 3D UI:      < %s > "};
static char* STR_QUICK_BOOT[]={"Quick boot:         < %s > ", "Avvio rapido:       < %s > ", "Inicio rapido:      < %s >", "Demarrage rapide :  < %s >", "Snel starten:       < %s > ", "Rask oppstart:      < %s > ", "Brzo Paljenje:      < %s > "};
static char* STR_CONSOLE_LANGUAGE[]={"Language:           < %s > ", "Lingua:             < %s > ", "Lenguaje:           < %s > ", "Langage :           < %s > ", "Taal:               < %s > ", "Sprak:              < %s > ", "Jezik:              < %s > "};

static char* STR_YES[]={"Yes", "Si ", "Si ", "Oui", "Ja ", "Ja ", "Da "};
static char* STR_NO[]={"No ", "No ", "No ", "Non", "Nee", "Nei", "Ne "};
//static char* STR_SETTINGS[]={"           SETTINGS", "          IMPOSTAZIONI", "            AJUSTES", "            OPTIONS", "          INSTELLINGEN", "         INNSTILLINGER", "           Postavke"};
static char* STR_AUTOBOOT[]={"           AUTOBOOT", "       AVVIO AUTOMATICO", "      INICIO AUTOMATICO", "    DÉMARRAGE AUTOMATIQUE", "          AUTOSTART", "    AUTOMATISK OPPSTART", "   Automatsko pokretanje"};
static char* STR_HOLD_R[]={"Hold R to go to the menu", "Tieni premuto R per andare al menu", "Manten pulsado R para ir al menu", "Maintenir le bouton R pour accéder au menu", "R ingedrukt houden voor menu", "Hold R for å gå til menyen", "Pridrzite tipku R da bi ste usli u meni"};
//static char* STR_MISSING_THEME_FILES={" MISSING THEME FILES!   ", " FILE DEL TEMA MANCANTI!   ", " NO SE ENCUENTRAN LOS ARCHIVOS DEL TEMA!   ", " FICHIERS DE THÈME MANQUANTS !   ", " THEMA BESTANDEN ONTBREKEN!   ", " MANGLER TEMAFILER!   ", " NEDOSTAJU DATOTEKE TEME!   "};
//static char* STR_CHOOSE_NAND={"Choose the NAND you want to use", "Seleziona la NAND da usare", "Selecciona una NAND para usar:", "Choissisez la NAND que vous voulez utiliser :", "Kies een NAND voor gebruik:", "Velg hvilken NAND du vil bruke:", "Izaberi NAND datoteku koju zelite koristiti:"};
//static char* STR_PRESS_X_SYSNAND={"Press X: sysNAND\n", "Premi X: sysNAND\n", "Pulsa X: sysNAND\n", "Bouton X : sysNAND\n", "Druk X: sysNAND\n", "Trykk X: sysNAND\n", "Stisni X: sysNAND\n"};
//static char* STR_PRESS_Y_EMUNAND={"Press Y: emuNAND\n", "Press Y: emuNAND\n", "Pulsa Y: emuNAND\n", "Bouton Y : emuNAND\n", "Druk Y: emuNAND\n", "Trykk Y: emuNAND\n", "Stisni Y: emuNAND\n"};
//static char* STR_PRESS_B_BACK={"Press B: Back\n", "Premi B: Indietro", "Pulsa B: Volver al menu", "Bouton B : Retour\n", "Druk B: Terug naar menu\n", "Trykk B: Tilbake\n", "Stisni B: Povratak\n"};

#endif
