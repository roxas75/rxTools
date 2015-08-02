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

#include "configuration.h"

enum {
	STR_LANG_EN,
	STR_LANG_IT,
	STR_LANG_ES,
	STR_LANG_FR,
	STR_LANG_NL,
	STR_LANG_NO,
	STR_LANG_HR,
	STR_LANG_RU,
	STR_LANG_ZH_CN,
	STR_LANG_ZH_TW,

	STR_LANG_NUM
};

enum {
	STR_LANG_NAME,
	STR_SETTINGS,
	STR_FORCE_UI_BOOT,
	STR_SELECTED_THEME,
	STR_SHOW_AGB,
	STR_ENABLE_3D_UI,
	STR_QUICK_BOOT,
	STR_CONSOLE_LANGUAGE,
	STR_CHOOSE_NAND,
	STR_PRESS_X_SYSNAND,
	STR_PRESS_Y_EMUNAND,
	STR_PRESS_B_BACK,
	STR_MISSING_THEME_FILES,
	STR_AUTOBOOT,
	STR_HOLD_R,
	STR_YES,
	STR_NO,

	STR_NUM
};

#define LANG_CODE_LEN 5

#if LANG_CODE_NUM > CFG_STR_MAX_LEN
#error "LANG_CODE_LEN > CFG_STR_MAX_LEN"
#endif

// Translations and corrections by: nastys, Crusard, Pikawil, DjoeN, VinsCool, R4ndom, guitarheroknight, Asia81, Syphurith, soarqin.
static const struct {
	wchar_t code[LANG_CODE_LEN];
	const wchar_t *strings[STR_NUM];
} mlStrings[STR_LANG_NUM] = {
	[STR_LANG_EN] = {
		.code = L"en",
		.strings = {
			[STR_LANG_NAME] = L"English",
			[STR_SETTINGS] = L"Settings",
			[STR_FORCE_UI_BOOT] = L"Force UI boot          %ls",
			[STR_SELECTED_THEME] = L"Selected theme      <%3c  >",
			[STR_SHOW_AGB] = L"Show AGB_FIRM BIOS     %ls",
			[STR_ENABLE_3D_UI] = L"Enable 3D UI           %ls",
			[STR_QUICK_BOOT] = L"Quick boot             %ls",
			[STR_CONSOLE_LANGUAGE] = L"Language  %16ls",
			[STR_CHOOSE_NAND] = L"Choose the NAND you want to use",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Back",
			[STR_MISSING_THEME_FILES] = L"MISSING THEME FILES!",
			[STR_AUTOBOOT] = L"AUTOBOOT",
			[STR_HOLD_R] = L"Hold R to go to the menu",
			[STR_YES] = L"Yes",
			[STR_NO] = L"No"
		}
	}, [STR_LANG_IT] = {
		.code = L"it",
		.strings = {
			[STR_LANG_NAME] = L"Italian",
			[STR_SETTINGS] = L"Impostazioni",
			[STR_FORCE_UI_BOOT] = L"Forza avvio UI         %ls",
			[STR_SELECTED_THEME] = L"Tema selezionato    <%3c  >",
			[STR_SHOW_AGB] = L"Mostra BIOS AGB_FIRM   %ls",
			[STR_ENABLE_3D_UI] = L"Attiva UI 3D           %ls",
			[STR_QUICK_BOOT] = L"Avvio rapido           %ls",
			[STR_CONSOLE_LANGUAGE] = L"Lingua    %16ls",
			[STR_CHOOSE_NAND] = L"Seleziona la NAND da usare",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Indietro",
			[STR_MISSING_THEME_FILES] = L"FILE DEL TEMA MANCANTI!",
			[STR_AUTOBOOT] = L"AVVIO AUTOMATICO",
			[STR_HOLD_R] = L"Tieni premuto R per andare al menu",
			[STR_YES] = L"Si",
			[STR_NO] = L"No"
		}
	}, [STR_LANG_ES] = {
		.code = L"es",
		.strings = {
			[STR_LANG_NAME] = L"Spanish",
			[STR_SETTINGS] = L"Ajustes",
			[STR_FORCE_UI_BOOT] = L"Iniciar en UI          %ls",
			[STR_SELECTED_THEME] = L"Tema seleccionado   <%3c  >",
			[STR_SHOW_AGB] = L"Ver AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"Habilitar 3D UI        %ls",
			[STR_QUICK_BOOT] = L"Inicio rapido          %ls",
			[STR_CONSOLE_LANGUAGE] = L"Lenguaje  %16ls",
			[STR_CHOOSE_NAND] = L"Selecciona una NAND para usar",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Volver al menu",
			[STR_MISSING_THEME_FILES] = L"NO SE ENCUENTRAN LOS ARCHIVOS DEL TEMA!",
			[STR_AUTOBOOT] = L"INICIO AUTOMATICO",
			[STR_HOLD_R] = L"Manten pulsado R para ir al menu",
			[STR_YES] = L"Si",
			[STR_NO] = L"No"
		}
	}, [STR_LANG_FR] = {
		.code = L"fr",
		.strings = {
			[STR_LANG_NAME] = L"French",
			[STR_SETTINGS] = L"Options",
			[STR_FORCE_UI_BOOT] = L"Forcer demarrage UI    %ls",
			[STR_SELECTED_THEME] = L"Theme choisi        <%3c  >",
			[STR_SHOW_AGB] = L"Activer AGB_FIRM       %ls",
			[STR_ENABLE_3D_UI] = L"Activer UI 3D          %ls",
			[STR_QUICK_BOOT] = L"Demarrage rapide       %ls",
			[STR_CONSOLE_LANGUAGE] = L"Langage   %16ls",
			[STR_CHOOSE_NAND] = L"Choissisez la NAND que vous voulez utiliser",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Retour",
			[STR_MISSING_THEME_FILES] = L"FICHIERS DE THEME MANQUANTS!",
			[STR_AUTOBOOT] = L"DÉMARRAGE AUTOMATIQUE",
			[STR_HOLD_R] = L"Maintenir le bouton R pour accéder au menu",
			[STR_YES] = L"Oui",
			[STR_NO] = L"Non"
		}
	}, [STR_LANG_NL] = {
		.code = L"nl",
		.strings = {
			[STR_LANG_NAME] = L"Dutch",
			[STR_SETTINGS] = L"Instellingen",
			[STR_FORCE_UI_BOOT] = L"Start in UI            %ls",
			[STR_SELECTED_THEME] = L"Selecteer thema     <%3c  >",
			[STR_SHOW_AGB] = L"Toon AGB_FIRM BIOS     %ls",
			[STR_ENABLE_3D_UI] = L"Gebruik 3D UI          %ls",
			[STR_QUICK_BOOT] = L"Snel starten           %ls",
			[STR_CONSOLE_LANGUAGE] = L"Taal      %16ls",
			[STR_CHOOSE_NAND] = L"Kies een NAND voor gebruik",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Terug naar menu",
			[STR_MISSING_THEME_FILES] = L"THEMA BESTANDEN ONTBREKEN!", 
			[STR_AUTOBOOT] = L"AUTOSTART",
			[STR_HOLD_R] = L"R ingedrukt houden voor menu",
			[STR_YES] = L"Ja",
			[STR_NO] = L"Nee"
		}
	}, [STR_LANG_NO] = {
		.code = L"no",
		.strings = {
			[STR_LANG_NAME] = L"Norwegian",
			[STR_SETTINGS] = L"Innstillinger",
			[STR_FORCE_UI_BOOT] = L"Tving oppstart av UI   %ls",
			[STR_SELECTED_THEME] = L"Valgt tema          <%3c  >",
			[STR_SHOW_AGB] = L"Vis AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"Aktiver UI i 3D        %ls",
			[STR_QUICK_BOOT] = L"Rask oppstart          %ls",
			[STR_CONSOLE_LANGUAGE] = L"Sprak     %16ls",
			[STR_CHOOSE_NAND] = L"Velg hvilken NAND du vil bruke",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Tilbake",
			[STR_MISSING_THEME_FILES] = L"MANGLER TEMAFILER!",
			[STR_AUTOBOOT] = L"AUTOMATISK OPPSTART",
			[STR_HOLD_R] = L"Hold R for å gå til menyen",
			[STR_YES] = L"Ja",
			[STR_NO] = L"Nei"
		}
	}, [STR_LANG_HR] = {
		.code = L"hr",
		.strings = {
			[STR_LANG_NAME] = L"Croatian",
			[STR_SETTINGS] = L"Postavke",
			[STR_FORCE_UI_BOOT] = L"Forsiraj ulazak u UI   %ls",
			[STR_SELECTED_THEME] = L"Izabrana tema       <%3c  >",
			[STR_SHOW_AGB] = L"Prikazi AGB_FIRM       %ls",
			[STR_ENABLE_3D_UI] = L"Ukljuci 3D UI          %ls",
			[STR_QUICK_BOOT] = L"Brzo Paljenje          %ls",
			[STR_CONSOLE_LANGUAGE] = L"Jezik     %16ls",
			[STR_CHOOSE_NAND] = L"Izaberi NAND datoteku koju zelite koristiti", 
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Povratak",
			[STR_MISSING_THEME_FILES] = L"NEDOSTAJU DATOTEKE TEME!",
			[STR_AUTOBOOT] = L"Automatsko pokretanje",
			[STR_HOLD_R] = L"Pridrzite tipku R da bi ste usli u meni",
			[STR_YES] = L"Da",
			[STR_NO] = L"Ne"
		}
	}, [STR_LANG_RU] = {
		.code = L"ru",
		.strings = {
			[STR_LANG_NAME] = L"Russian",
			[STR_SETTINGS] = L"Настройки",
			[STR_FORCE_UI_BOOT] = L"Загружать UI           %ls",
			[STR_SELECTED_THEME] = L"Выбранная тема      <%3c  >",
			[STR_SHOW_AGB] = L"Показывать AGB_FIRM    %ls",
			[STR_ENABLE_3D_UI] = L"Включить 3D UI         %ls",
			[STR_QUICK_BOOT] = L"Быстрая загрузка       %ls",
			[STR_CONSOLE_LANGUAGE] = L"Язык      %16ls",
			[STR_CHOOSE_NAND] = L"Выберите NAND",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ Назад",
			[STR_MISSING_THEME_FILES] = L"ОТСУТСТВУЮТ ФАЙЛЫ ТЕМЫ!",
			[STR_AUTOBOOT] = L"Автозагрузка",
			[STR_HOLD_R] = L"Удерживайте R для перехода в меню",
			[STR_YES] = L"Да",
			[STR_NO] = L"Нет"
		}
	}, [STR_LANG_ZH_CN] = {
		.code = L"zh-CN",
		.strings = {
			[STR_LANG_NAME] = L"Chinese simp.",
			[STR_SETTINGS] = L"设置选项",
			[STR_FORCE_UI_BOOT] = L"启动时强制进菜单       %ls",
			[STR_SELECTED_THEME] = L"选择主题            <%3c  >",
			[STR_SHOW_AGB] = L"显示AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"启用3D界面             %ls",
			[STR_QUICK_BOOT] = L"快速启动               %ls",
			[STR_CONSOLE_LANGUAGE] = L"界面语言  %16ls",
			[STR_CHOOSE_NAND] = L"请选择要使用的NAND",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ 返回",
			[STR_MISSING_THEME_FILES] = L"找不到主题文件!",
			[STR_AUTOBOOT] = L"自动启动",
			[STR_HOLD_R] = L"按住R键进入菜单",
			[STR_YES] = L"是",
			[STR_NO] = L"否"
		}
	}, [STR_LANG_ZH_TW] = {
		.code = L"zh-TW",
		.strings = {
			[STR_LANG_NAME] = L"Chinese trad.",
			[STR_SETTINGS] = L"設定選項",
			[STR_FORCE_UI_BOOT] = L"引導時強制進菜單       %ls",
			[STR_SELECTED_THEME] = L"選擇主題            <%3c  >",
			[STR_SHOW_AGB] = L"顯示AGB_FIRM BIOS      %ls",
			[STR_ENABLE_3D_UI] = L"啟用3D介面             %ls",
			[STR_QUICK_BOOT] = L"快速引導               %ls",
			[STR_CONSOLE_LANGUAGE] = L"介面語言  %16ls",
			[STR_CHOOSE_NAND] = L"請選擇要使用的NAND",
			[STR_PRESS_X_SYSNAND] = L"Ⓧ sysNAND",
			[STR_PRESS_Y_EMUNAND] = L"Ⓨ emuNAND",
			[STR_PRESS_B_BACK] = L"Ⓑ 返回",
			[STR_MISSING_THEME_FILES] = L"主題檔案缺失!",
			[STR_AUTOBOOT] = L"自動引導",
			[STR_HOLD_R] = L"按住R鈕進入菜單",
			[STR_YES] = L"是",
			[STR_NO] = L"否"
		}
	}
};
