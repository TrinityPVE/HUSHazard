class CfgPatches
{
	class HUSHazard_Vehicles
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {
			"DZ_Data",
			"DZ_Scripts",
			"HUSHazard_AI", // Выдерживаем строгую модульную очередь загрузки
			"DZ_AI",
			"DZ_Vehicles_Wheeled",
			"DZ_Sounds_Effects",
			"DZ_Weapons_Melee",      
			"DZ_Characters",
			"DZ_Characters_Zombies", // Базовый патч игры, содержащий структуры DayZInfected
			"DZ_Gear_Tools"
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

// ============================================================================
// ЗВУКОВЫЕ ШЕЙДЕРЫ И НАБОРЫ TRINITYPVE
// ============================================================================
class CfgSoundShaders
{
	class HH_Zombie_Search_SoundShader { samples[] = { { "HUSHazard\sounds\hh_zombie_search", 1 } }; volume = 1.8; range = 15; };
	class HH_Wrench_Loop_SoundShader { samples[] = { { "HUSHazard\sounds\hh_wrench_loop", 1 } }; volume = 1.5; range = 10; };
	class action_interact_SoundShader { samples[] = { { "HUSHazard\sounds\hh_trunk", 1 } }; volume = 0.7; range = 15; };
	class HH_Fridge_Search_SoundShader { samples[] = { { "HUSHazard\sounds\hh_fridge", 1 } }; volume = 1.2; range = 12; };
	class HH_Metal_Search_SoundShader { samples[] = { { "HUSHazard\sounds\hh_metal_cabinet", 1 } }; volume = 1.2; range = 12; };
	class HH_Books_Search_SoundShader { samples[] = { { "HUSHazard\sounds\hh_books", 1 } }; volume = 1.2; range = 12; };
	class HH_WoodLock_Search_SoundShader { samples[] = { { "HUSHazard\sounds\hh_wood_cabinet", 1 } }; volume = 1.2; range = 12; };
	class HH_MetalWardrobe_Search_SoundShader { samples[] = { { "HUSHazard\sounds\hh_metal_wardrobe", 1 } }; volume = 1.2; range = 12; };

	class HH_Scream_SoundShader 
	{ 
		samples[] = { { "HUSHazard\sounds\hh_svist", 1 } }; 
		volume = 0.5; 
		range = 150; 
		volumeCurve[] = {
			{0.0, 1.0},
			{5.0, 0.3},
			{10.0, 0.05},
			{15.0, 0.0}
		};
	};
};

class CfgSoundSets
{
	class HH_Zombie_Search_SoundSet { soundShaders[] = { "HH_Zombie_Search_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };
	class HH_Wrench_Loop_SoundSet { soundShaders[] = { "HH_Wrench_Loop_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; }; 
	class action_interact_SoundSet { soundShaders[] = { "action_interact_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };
	class HH_Fridge_Search_SoundSet { soundShaders[] = { "HH_Fridge_Search_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };
	class HH_Books_Search_SoundSet { soundShaders[] = { "HH_Books_Search_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };
	class HH_Clothes_Search_SoundSet { soundShaders[] = { "HH_WoodLock_Search_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };
	class HH_MetalCabinet_Search_SoundSet { soundShaders[] = { "HH_Metal_Search_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };
	class HH_Table_Search_SoundSet { soundShaders[] = { "HH_WoodLock_Search_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };
	class HH_Trunk_Search_SoundSet { soundShaders[] = { "action_interact_SoundShader" }; volumeFactor = 1.0; spatial = 1; loop = 1; };

	class HH_Scream_SoundSet 
	{ 
		soundShaders[] = { "HH_Scream_SoundShader" }; 
		spatial = 1;
		sound3DProcessingType = "Human3DProcessingType";
		distanceFilter = "regularSilenceFilter"; 
	};
};

class CfgAmmo
{
	class DefaultAmmo;
	class MeleeDamage: DefaultAmmo {};
	class MeleeFists: MeleeDamage { class DamageApplied { type = "Melee"; class Health { damage = 0.0; }; class Blood { damage = 0; }; class Shock { damage = 0; }; }; };
	class MeleeFistsHeavy: MeleeDamage { class DamageApplied { type = "Melee"; class Health { damage = 0.0; }; class Blood { damage = 0; }; class Shock { damage = 0; }; }; };
	class HH_Melee_Wipeout: MeleeDamage { class DamageApplied { type = "Melee"; class Health { damage = 200.0; }; class Blood { damage = 150.0; }; class Shock { damage = 150.0; }; }; };
	class MeleeLightBlunt: MeleeDamage { class DamageApplied { type = "Melee"; class Health { damage = 2.0; }; class Blood { damage = 0; }; class Shock { damage = 10.0; }; }; };
	class MeleeLightBlunt_Heavy: MeleeDamage { class DamageApplied { type = "Melee"; class Health { damage = 3.0; }; class Blood { damage = 0; }; class Shock { damage = 15.0; }; }; };
	class MeleeBluntLight_3: MeleeDamage { class DamageApplied { type = "Melee"; class Health { damage = 2.0; }; class Blood { damage = 0; }; class Shock { damage = 10.0; }; }; };
	class MeleeBluntHeavy_3: MeleeDamage { class DamageApplied { type = "Melee"; class Health { damage = 3.0; }; class Blood { damage = 0; class Shock { damage = 15.0; }; }; }; };
};

// ============================================================================
// ГЕОМЕТРИЯ СУЩЕСТВ И ПРЕДМЕТОВ (CfgVehicles С ПОЛНЫМ НАСЛЕДОВАНИЕМ КОСТЕЙ)
// ============================================================================
class CfgVehicles
{
	class Man;
	class SurvivorBase: Man
	{
		class NoiseActionDefault 
		{
			strength = 15; 	
			type = "shot"; 
		};
	};

	// НАГЛУХО СТЫКУЕМ ВЫЗОВЫ VTABLE С СИСТЕМНЫМИ ИЕРАРХИЯМИ ДЛЯ УСТРАНЕНИЯ КРАША ACCESS VIOLATION
	class DZ_LightAI;
	class DayZInfected: DZ_LightAI {};

	class ZombieBase: DayZInfected
	{
		class MeleeCombat { class Hits { class HitLight { disabledBlock = 1; blockMitigation = "NONE"; }; class HitHeavy { disabledBlock = 1; blockMitigation = "NONE"; }; }; };
		class DamageSystem { class DamageZones { class Head { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = 0.1; }; class Torso { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; }; class LeftArm { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; }; class RightArm: LeftArm { fatalInjuryCoef = -1; }; class LeftLeg { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; }; class RightLeg: LeftLeg { fatalInjuryCoef = -1; }; class LeftFoot { class Health { hitpoints = 100; transferToGlobalCoef = 1; }; fatalInjuryCoef = -1; }; class RightFoot: LeftFoot { fatalInjuryCoef = -1; }; }; };
	};

	class ZombieMaleBase: ZombieBase {};
	class ZombieFemaleBase: ZombieBase {};

	class Inventory_Base;
	class Shovel: Inventory_Base { weight = 4000; class DamageSystem { class GlobalHealth { class Health { hitpoints = 200; }; }; }; };
	class FirefighterAxe: Inventory_Base { class MeleeModes { class Default { ammo = "HH_Melee_Wipeout"; range = 1.4; }; class Heavy { ammo = "HH_Melee_Wipeout"; range = 1.4; }; class Sprint { ammo = "HH_Melee_Wipeout"; range = 3.3; }; }; };
	class WoodAxe: Inventory_Base { class MeleeModes { class Default { ammo = "HH_Melee_Wipeout"; range = 1.4; }; class Health { hitpoints = 100; }; }; };
	class SledgeHammer: Inventory_Base { class MeleeModes { class Default { ammo = "HH_Melee_Wipeout"; range = 1.4; }; class Heavy { ammo = "HH_Melee_Wipeout"; range = 1.4; }; class Sprint { ammo = "HH_Melee_Wipeout"; range = 3.3; }; }; };
	class Crowbar: Inventory_Base { class MeleeModes { class Default { ammo = "HH_Melee_Wipeout"; range = 1.3; }; class Heavy { ammo = "HH_Melee_Wipeout"; range = 1.3; }; class Sprint { ammo = "HH_Melee_Wipeout"; range = 2.8; }; }; };
	class PipeWrench: Inventory_Base { class MeleeModes { class Default { ammo = "HH_Melee_Wipeout"; range = 1.3; }; class Heavy { ammo = "HH_Melee_Wipeout"; range = 1.3; }; class Sprint { ammo = "HH_Melee_Wipeout"; range = 2.8; }; }; };

	class HH_InvisibleProxy_Item: Inventory_Base
	{
		scope = 2;
		displayName = "";
		model = "HUSHazard\HH_invisibleproxy.p3d";
		weight = 0;
		inventorySlot[] = {"Melee"};
		showActions = 0;              
		showItemInSimulation = 0;     
		hiddenSelections[] = {"all"}; 
		class DamageSystem { class GlobalHealth { class Health { hitpoints = 100; }; }; };
	};
};
