class CfgPatches
{
	class HUSHazard_Scripts_Core
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {
			"DZ_Data",
			"DZ_Scripts",
			"JM_CF_Scripts",         // Подключаем Community Framework
			"HUSHazard_AI",          // ЖЕСТКАЯ С++ ОЧЕРЕДЬ: Ждем загрузки суб-модуля мозгов ИИ!
			"HUSHazard_Vehicles"     // ЖЕСТКАЯ С++ ОЧЕРЕДЬ: Ждем загрузки суб-модуля физических тел и звуков!
		};
	};
};

class CfgMods
{
	class HUSHazard
	{
		dir = "HUSHazard";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "HUSHazard Mod";
		credits = "Reticent";
		author = "Reticent";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "Game", "World", "Mission" };
		class defs
		{
			class gameScriptModule { value = ""; files[] = { "HUSHazard/scripts/3_Game" }; };
			class worldScriptModule { value = ""; files[] = { "HUSHazard/scripts/4_World" }; };
			class missionScriptModule { value = ""; files[] = { "HUSHazard/scripts/5_Mission" }; };
		};
	};
};
