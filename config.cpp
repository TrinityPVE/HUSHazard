class CfgPatches
{
	class HUSHazard
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Vehicles_Wheeled",
			"DZ_Sounds_Effects",
			"DZ_Weapons_Melee",      
			"DZ_Characters_Zombies",
			"DZ_Gear_Tools"
		};
	};
};

class CfgSoundShaders
{
	class HH_Zombie_Search_SoundShader
	{
		samples[] = { { "HUSHazard\sounds\hh_zombie_search", 1 } };
		volume = 1.8;
		range = 15;
	};
	// ДОБАВЛЕНО: Шейдер для разбора капота ключом
	class HH_Wrench_Loop_SoundShader
	{
		samples[] = { { "HUSHazard\sounds\hh_wrench_loop", 1 } };
		volume = 1.5;
		range = 10;
	};
};

class CfgSoundSets
{
	class HH_Zombie_Search_SoundSet
	{
		soundShaders[] = { "HH_Zombie_Search_SoundShader" };
		soundDistType = 1;
		volumeFactor = 1.0;
		spatial = 1;
		loop = 1;
	};
	// ДОБАВЛЕНО: Саундсет для экшена с гаечным ключом
	class HH_Wrench_Loop_SoundSet
	{
		soundShaders[] = { "HH_Wrench_Loop_SoundShader" };
		soundDistType = 1;
		volumeFactor = 1.0;
		spatial = 1;
		loop = 1;
	};
};

// ============================================================================
// МОДИФИКАЦИЯ ПАТРОНОВ (CfgAmmo — ФИНАЛЬНАЯ НАСТРОЙКА БАЛАНСА 2.0 И 3.0)
// ============================================================================
class CfgAmmo
{
	class DefaultAmmo;
	class MeleeDamage: DefaultAmmo {};

	class MeleeFists: MeleeDamage
	{
		class DamageApplied
		{
			type = "Melee";
			class Health { damage = 0.0; }; 
			class Blood  { damage = 0; };
			class Shock  { damage = 0; }; 
		};
	};
	class MeleeFistsHeavy: MeleeDamage
	{
		class DamageApplied
		{
			type = "Melee";
			class Health { damage = 0.0; }; 
			class Blood  { damage = 0; };
			class Shock  { damage = 0; }; 
		};
	};

	class HH_Melee_Wipeout: MeleeDamage
	{
		class DamageApplied
		{
			type = "Melee";
			class Health { damage = 200.0; }; 
			class Blood  { damage = 150.0; };
			class Shock  { damage = 150.0; };
		};
	};

	// ФИКСАЦИЯ ТЗ: Легкие удары хлама — 2.0 ХП урона, тяжелые — 3.0 ХП урона!
	class MeleeLightBlunt: MeleeDamage
	{
		class DamageApplied { type = "Melee"; class Health { damage = 2.0; }; class Blood { damage = 0; }; class Shock { damage = 10.0; }; };
	};
	class MeleeLightBlunt_Heavy: MeleeDamage
	{
		class DamageApplied { type = "Melee"; class Health { damage = 3.0; }; class Blood { damage = 0; }; class Shock { damage = 15.0; }; };
	};
	class MeleeBluntLight_3: MeleeDamage
	{
		class DamageApplied { type = "Melee"; class Health { damage = 2.0; }; class Blood { damage = 0; }; class Shock { damage = 10.0; }; };
	};
	class MeleeBluntHeavy_3: MeleeDamage
	{
		class DamageApplied { type = "Melee"; class Health { damage = 3.0; }; class Blood { damage = 0; }; class Shock { damage = 15.0; }; };
	};
};

class CfgVehicles
{
	class DayZInfected;
	class ZombieBase: DayZInfected
	{
		class MeleeCombat
		{
			class Hits
			{
				class HitLight { disabledBlock = 1; blockMitigation = "NONE"; };
				class HitHeavy { disabledBlock = 1; blockMitigation = "NONE"; };
			};
		};

		class DamageSystem
		{
			class DamageZones
			{
				class Head
				{
					class Health { hitpoints = 100; transferToGlobalCoef = 1; }; 
					fatalInjuryCoef = 0.1;
				};
				class Torso { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; };
				class LeftArm { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; };
				class RightArm: LeftArm { fatalInjuryCoef = -1; };
				class LeftLeg { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; };
				class RightLeg: LeftLeg { fatalInjuryCoef = -1; }; 
				class LeftFoot { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; };
				class RightFoot: LeftFoot { fatalInjuryCoef = -1; };
			};
		};
	};

	class Inventory_Base;

	// Лопата: Возвращен заводской пул прочности (200 ХП), вес 4кг
	class Shovel: Inventory_Base
	{
		weight = 4000;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health { hitpoints = 200; }; 
			};
		};
	};

	class FirefighterAxe: Inventory_Base
	{
		class MeleeModes
		{
			class Default { ammo = "HH_Melee_Wipeout"; range = 1.4; };
			class Heavy   { ammo = "HH_Melee_Wipeout"; range = 1.4; };
			class Sprint  { ammo = "HH_Melee_Wipeout"; range = 3.3; };
		};
	};
	class WoodAxe: Inventory_Base
	{
		class MeleeModes
		{
			class Default { ammo = "HH_Melee_Wipeout"; range = 1.4; };
			class Heavy   { ammo = "HH_Melee_Wipeout"; range = 1.4; };
			class Sprint  { ammo = "HH_Melee_Wipeout"; range = 3.3; };
		};
	};
	class SledgeHammer: Inventory_Base
	{
		class MeleeModes
		{
			class Default { ammo = "HH_Melee_Wipeout"; range = 1.4; };
			class Heavy   { ammo = "HH_Melee_Wipeout"; range = 1.4; };
			class Sprint  { ammo = "HH_Melee_Wipeout"; range = 3.3; };
		};
	};
	class Crowbar: Inventory_Base
	{
		class MeleeModes
		{
			class Default { ammo = "HH_Melee_Wipeout"; range = 1.3; };
			class Heavy   { ammo = "HH_Melee_Wipeout"; range = 1.3; };
			class Sprint  { ammo = "HH_Melee_Wipeout"; range = 2.8; };
		};
	};
	class PipeWrench: Inventory_Base
	{
		class MeleeModes
		{
			class Default { ammo = "HH_Melee_Wipeout"; range = 1.3; };
			class Heavy   { ammo = "HH_Melee_Wipeout"; range = 1.3; };
			class Sprint  { ammo = "HH_Melee_Wipeout"; range = 2.8; };
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