/*
 * Copyright (C) 2015 The PASTA Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "lang.h"

// Translations and corrections by: nastys, Crusard, Pikawil, DjoeN, VinsCool, R4ndom, guitarheroknight, Asia81, Syphurith, soarqin.
static const struct {
	char code[LANG_CODE_LEN];
	const wchar_t *strings[STR_NUM];
} mlStrings[STR_LANG_NUM] = {
	[STR_LANG_EN] = {
		.code = "en",
		.strings = {
			[STR_LANG_NAME] = L"English",

			[STR_FORCE_UI_BOOT] = L"Force UI boot          %ls",
			[STR_SELECTED_THEME] = L"Selected theme      <%3c  >",
			[STR_SHOW_AGB] = L"Show AGB_FIRM BIOS     %ls",
			[STR_ENABLE_3D_UI] = L"Enable 3D UI           %ls",
			[STR_QUICK_BOOT] = L"Quick boot             %ls",
			[STR_CONSOLE_LANGUAGE] = L"Language  %16ls",
			[STR_AUTOBOOT] = L"AUTOBOOT",
			[STR_HOLD_R] = L"Hold R to go to the menu"
		}
	}, [STR_LANG_IT] = {
		.code = "it",
		.strings = {
			[STR_LANG_NAME] = L"Italian",

			[STR_FORCE_UI_BOOT] = L"Forza avvio UI         %ls",
			[STR_SELECTED_THEME] = L"Tema selezionato    <%3c  >",
			[STR_SHOW_AGB] = L"Mostra BIOS AGB_FIRM   %ls",
			[STR_ENABLE_3D_UI] = L"Attiva UI 3D           %ls",
			[STR_QUICK_BOOT] = L"Avvio rapido           %ls",
			[STR_CONSOLE_LANGUAGE] = L"Lingua    %16ls",
			[STR_AUTOBOOT] = L"AVVIO AUTOMATICO",
			[STR_HOLD_R] = L"Tieni premuto R per andare al menu"
		}
	}, [STR_LANG_ES] = {
		.code = "es",
		.strings = {
			[STR_LANG_NAME] = L"Spanish",

			[STR_FORCE_UI_BOOT] = L"Iniciar en UI          %ls",
			[STR_SELECTED_THEME] = L"Tema seleccionado   <%3c  >",
			[STR_SHOW_AGB] = L"Ver AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"Habilitar 3D UI        %ls",
			[STR_QUICK_BOOT] = L"Inicio rapido          %ls",
			[STR_CONSOLE_LANGUAGE] = L"Lenguaje  %16ls",
			[STR_AUTOBOOT] = L"INICIO AUTOMATICO",
			[STR_HOLD_R] = L"Manten pulsado R para ir al menu"
		}
	}, [STR_LANG_FR] = {
		.code = "fr",
		.strings = {
			[STR_LANG_NAME] = L"French",

			[STR_FORCE_UI_BOOT] = L"Forcer demarrage UI    %ls",
			[STR_SELECTED_THEME] = L"Theme choisi        <%3c  >",
			[STR_SHOW_AGB] = L"Activer AGB_FIRM       %ls",
			[STR_ENABLE_3D_UI] = L"Activer UI 3D          %ls",
			[STR_QUICK_BOOT] = L"Demarrage rapide       %ls",
			[STR_CONSOLE_LANGUAGE] = L"Langage   %16ls",
			[STR_AUTOBOOT] = L"DÉMARRAGE AUTOMATIQUE",
			[STR_HOLD_R] = L"Maintenir le bouton R pour accéder au menu"
		}
	}, [STR_LANG_NL] = {
		.code = "nl",
		.strings = {
			[STR_LANG_NAME] = L"Dutch",

			[STR_FORCE_UI_BOOT] = L"Start in UI            %ls",
			[STR_SELECTED_THEME] = L"Selecteer thema     <%3c  >",
			[STR_SHOW_AGB] = L"Toon AGB_FIRM BIOS     %ls",
			[STR_ENABLE_3D_UI] = L"Gebruik 3D UI          %ls",
			[STR_QUICK_BOOT] = L"Snel starten           %ls",
			[STR_CONSOLE_LANGUAGE] = L"Taal      %16ls",
			[STR_AUTOBOOT] = L"AUTOSTART",
			[STR_HOLD_R] = L"R ingedrukt houden voor menu"
		}
	}, [STR_LANG_NO] = {
		.code = "no",
		.strings = {
			[STR_LANG_NAME] = L"Norwegian",

			[STR_FORCE_UI_BOOT] = L"Tving oppstart av UI   %ls",
			[STR_SELECTED_THEME] = L"Valgt tema          <%3c  >",
			[STR_SHOW_AGB] = L"Vis AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"Aktiver UI i 3D        %ls",
			[STR_QUICK_BOOT] = L"Rask oppstart          %ls",
			[STR_CONSOLE_LANGUAGE] = L"Sprak     %16ls",
			[STR_AUTOBOOT] = L"AUTOMATISK OPPSTART",
			[STR_HOLD_R] = L"Hold R for å gå til menyen"
		}
	}, [STR_LANG_HR] = {
		.code = "hr",
		.strings = {
			[STR_LANG_NAME] = L"Croatian",

			[STR_FORCE_UI_BOOT] = L"Forsiraj ulazak u UI   %ls",
			[STR_SELECTED_THEME] = L"Izabrana tema       <%3c  >",
			[STR_SHOW_AGB] = L"Prikazi AGB_FIRM       %ls",
			[STR_ENABLE_3D_UI] = L"Ukljuci 3D UI          %ls",
			[STR_QUICK_BOOT] = L"Brzo Paljenje          %ls",
			[STR_CONSOLE_LANGUAGE] = L"Jezik     %16ls",
			[STR_AUTOBOOT] = L"Automatsko pokretanje",
			[STR_HOLD_R] = L"Pridrzite tipku R da bi ste usli u meni"
		}
	}, [STR_LANG_RU] = {
		.code = "ru",
		.strings = {
			[STR_LANG_NAME] = L"Russian",

			[STR_FORCE_UI_BOOT] = L"Загружать UI           %ls",
			[STR_SELECTED_THEME] = L"Выбранная тема      <%3c  >",
			[STR_SHOW_AGB] = L"Показывать AGB_FIRM    %ls",
			[STR_ENABLE_3D_UI] = L"Включить 3D UI         %ls",
			[STR_QUICK_BOOT] = L"Быстрая загрузка       %ls",
			[STR_CONSOLE_LANGUAGE] = L"Язык      %16ls",
			[STR_AUTOBOOT] = L"АВТОЗАГРУЗКА",
			[STR_HOLD_R] = L"Удерживайте R для перехода в меню"
		}
	}, [STR_LANG_ZH_CN] = {
		.code = "zh-CN",
		.strings = {
			[STR_LANG_NAME] = L"Chinese simp.",

			[STR_FORCE_UI_BOOT] = L"启动时强制进菜单       %ls",
			[STR_SELECTED_THEME] = L"选择主题            <%3c  >",
			[STR_SHOW_AGB] = L"显示AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"启用3D界面             %ls",
			[STR_QUICK_BOOT] = L"快速启动               %ls",
			[STR_CONSOLE_LANGUAGE] = L"界面语言  %16ls",
			[STR_AUTOBOOT] = L"自动启动",
			[STR_HOLD_R] = L"按住R键进入菜单"
		}
	}, [STR_LANG_ZH_TW] = {
		.code = "zh-TW",
		.strings = {
			[STR_LANG_NAME] = L"Chinese trad.",

			[STR_FORCE_UI_BOOT] = L"引導時強制進菜單       %ls",
			[STR_SELECTED_THEME] = L"選擇主題            <%3c  >",
			[STR_SHOW_AGB] = L"顯示AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"啟用3D介面             %ls",
			[STR_QUICK_BOOT] = L"快速引導               %ls",
			[STR_CONSOLE_LANGUAGE] = L"介面語言  %16ls",
			[STR_AUTOBOOT] = L"自動引導",
			[STR_HOLD_R] = L"按住R鈕進入菜單"
		}
	}, [STR_LANG_PT] = {
		.code = "pt",
		.strings = {
			[STR_LANG_NAME] = L"Português",

			[STR_FORCE_UI_BOOT] = L"Iniciar UI             %ls",
			[STR_SELECTED_THEME] = L"Tema selecionado    <%3c  >",
			[STR_SHOW_AGB] = L"Exibir BIOS AGB_FIRM   %ls",
			[STR_ENABLE_3D_UI] = L"Habilitar 3D UI        %ls",
			[STR_QUICK_BOOT] = L"Inicio rapido          %ls",
			[STR_CONSOLE_LANGUAGE] = L"Idioma    %16ls",
			[STR_AUTOBOOT] = L"INICIO AUTOMATICO",
			[STR_HOLD_R] = L"Precione R para ir para menu"
		}
	}
};

//static wchar_t* STR_YES[]={L"Yes", L"Si", L"Si", L"Oui", L"Ja", L"Ja", L"Da", L"Да", L"是", L"是", L"Sim"};
//static wchar_t* STR_NO[]={L"No", L"No", L"No", L"Non", L"Nee", L"Nei", L"Ne", L"Нет", L"否", L"否", L"Não"};
//static wchar_t* STR_SETTINGS[]={L"           SETTINGS", L"          IMPOSTAZIONI", L"            AJUSTES", L"            OPTIONS", L"          INSTELLINGEN", L"         INNSTILLINGER", L"           Postavke", L"           НАСТРОЙКИ", L"            设置选项", L"            設定選項", L"            OPÇÕES"};
//static wchar_t* STR_MISSING_THEME_FILES={L" MISSING THEME FILES!   ", L" FILE DEL TEMA MANCANTI!   ", L" NO SE ENCUENTRAN LOS ARCHIVOS DEL TEMA!   ", L" FICHIERS DE THEME MANQUANTS !   ", L" THEMA BESTANDEN ONTBREKEN!   ", L" MANGLER TEMAFILER!   ", L" NEDOSTAJU DATOTEKE TEME!   ", L" ОТСУТСТВУЮТ ФАЙЛЫ ТЕМЫ!   ", L" 找不到主题文件!  ", L" 主題檔案缺失!  ", L" ARQUIVO DE TEMA FALTANDO!   "};
//static wchar_t* STR_CHOOSE_NAND={L"Choose the NAND you want to use", L"Seleziona la NAND da usare", L"Selecciona una NAND para usar:", L"Choissisez la NAND que vous voulez utiliser :", L"Kies een NAND voor gebruik:", L"Velg hvilken NAND du vil bruke:", L"Izaberi NAND datoteku koju zelite koristiti:", L"Выберите NAND, который хотите использовать", L"请选择要使用的NAND", L"請選擇要使用的NAND", L"Selecione a NAND para iniciar"};
//static wchar_t* STR_PRESS_X_SYSNAND={L"Press Ⓧ: sysNAND\n", L"Premi Ⓧ: sysNAND\n", L"Pulsa Ⓧ: sysNAND\n", L"Bouton X : sysNAND\n", L"Druk Ⓧ: sysNAND\n", L"Trykk Ⓧ: sysNAND\n", L"Stisni Ⓧ: sysNAND\n", L"Нажмите Ⓧ: sysNAND\n", L"按 Ⓧ: sysNAND\n", L"按 Ⓧ: sysNAND\n", L"Precione Ⓧ: sysNAND\n"};
//static wchar_t* STR_PRESS_Y_EMUNAND={L"Press Ⓨ: emuNAND\n", L"Press Ⓨ: emuNAND\n", L"Pulsa Ⓨ: emuNAND\n", L"Bouton Y : emuNAND\n", L"Druk Ⓨ: emuNAND\n", L"Trykk Ⓨ: emuNAND\n", L"Stisni Ⓨ: emuNAND\n", L"Нажмите Ⓨ: emuNAND\n", L"按 Ⓨ: emuNAND\n", L"按 Ⓨ: emuNAND\n", L"Precione Ⓨ: emuNAND\n"};
//static wchar_t* STR_PRESS_B_BACK={L"Press Ⓑ: Back\n", L"Premi Ⓑ: Indietro", L"Pulsa Ⓑ: Volver al menu", L"Bouton B : Retour\n", L"Druk Ⓑ: Terug naar menu\n", L"Trykk Ⓑ: Tilbake\n", L"Stisni Ⓑ: Povratak\n", L"Нажмите Ⓑ: Назад\n", L"按 Ⓑ: 返回\n", L"按 Ⓑ: 返回\n", L"Precione Ⓑ: Voltar"};

const wchar_t * const *strings;
static unsigned int langIndex;

void setLang(unsigned int i){
	langIndex = i;
	strings = mlStrings[langIndex].strings;
}

void setLangByCode(const char *code){
	unsigned int i;

	for (i = 0; i < STR_LANG_NUM; i++)
		if (!strcmp(code, mlStrings[i].code)) {
			setLang(i);
			return;
		}

	setLang(STR_LANG_EN);
}

unsigned int getLang(){
	return langIndex;
}

const char *getLangCode(){
	return mlStrings[langIndex].code;
}
