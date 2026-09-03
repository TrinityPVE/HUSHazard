// ============================================================================
// HUSHazard - Hard Universal Search & Hazard Mod (Patched for DayZ 1.29)
// Server Manager Module - FIXED DECENTRALIZED LOOT ECONOMY (PART 1 OF 6)
// ============================================================================

class HUSHazardSubGroupData
{
	string SubGroupName; 
	float Weight;
	ref array<string> Common;
	ref array<string> Uncommon;
	ref array<string> Rare;
	ref array<string> Exclusive;

	void HUSHazardSubGroupData()
	{
		Common = new array<string>();
		Uncommon = new array<string>();
		Rare = new array<string>();
		Exclusive = new array<string>();
	}
}

class HUSHazardCategoryData
{
	string CategoryName; 
	float LootChance;
	bool NeedsGloves;
	float GloveWearHP;
	bool NeedsTool;
	string ToolClass;
	float ToolWearHP;
	ref array<ref HUSHazardSubGroupData> SubGroups; 

	void HUSHazardCategoryData()
	{
		SubGroups = new array<ref HUSHazardSubGroupData>();
	}
}

class HUSHazardLootConfig
{
	ref array<ref HUSHazardCategoryData> Categories; 

	void HUSHazardLootConfig()
	{
		Categories = new array<ref HUSHazardCategoryData>();
	}
}

class HUSHazardServerManager
{
	private static const string m_ConfigPath = "$profile:\\HUSHazard\\HUSHazard_Loot.json";
	private static ref HUSHazardLootConfig m_LootConfig;

	// ИСПРАВЛЕНО: Теперь уличный модуль гарантированно регистрируется при первом запуске
	static void Init()
	{
		if (!GetGame().IsServer()) return;

		m_LootConfig = new HUSHazardLootConfig();

		if (!FileExist(m_ConfigPath))
		{
			Print("[HUSHazard]: JSON-конфиг не найден. Запуск децентрализованной автогенерации...");
			GenerateCivilianConfig();
			GenerateMedicalConfig();
			GeneratePoliceConfig();
			GenerateIndustrialConfig();
			GenerateMilitaryConfig();
			GenerateStreetStaticConfig(); // ГАРАНТИРОВАННЫЙ ЗАПУСК УЛИЧНОЙ СТАТИКИ!
			HH_SaveLootConfig();
		}
		else
		{
			HH_LoadLootConfig();
		}
	}

	private static void HH_LoadLootConfig()
	{
		JsonFileLoader<HUSHazardLootConfig>.JsonLoadFile(m_ConfigPath, m_LootConfig);
		Print("[HUSHazard]: JSON-конфиг экономики успешно загружен из Profiles!");
	}

	private static void HH_SaveLootConfig()
	{
		if (!FileExist("$profile:\\HUSHazard\\"))
		{
			MakeDirectory("$profile:\\HUSHazard\\");
		}
		JsonFileLoader<HUSHazardLootConfig>.JsonSaveFile(m_ConfigPath, m_LootConfig);
	}
	// ============================================================================
	// ЧАСТЬ 2: ИЗОЛИРОВАННЫЙ БАЗОВЫЙ ГРАЖДАНСКИЙ ЛУТ (МЕТОД 1 ИЗ 5)
	// ============================================================================
	private static void GenerateCivilianConfig()
	{
		HUSHazardCategoryData cat;
		HUSHazardSubGroupData sub;

		// 1. БАЗОВЫЙ ГРАЖДАНСКИЙ ШКАФ (wardrobe)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "wardrobe";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.GloveWearHP = 0.0; cat.NeedsTool = false;
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "clothes"; sub.Weight = 0.50;
		sub.Common.Insert("TShirt_Blue"); sub.Common.Insert("TShirt_Red"); sub.Common.Insert("Hoodie_Black"); sub.Common.Insert("Bandana_Red");
		sub.Uncommon.Insert("Jeans_Blue"); sub.Uncommon.Insert("CargoPants_Green"); sub.Uncommon.Insert("Shirt_BlueCheck"); sub.Uncommon.Insert("BeanieHat_Black");
		sub.Rare.Insert("Raincoat_Green"); sub.Rare.Insert("Sweater_Gray"); sub.Rare.Insert(".380_100RndBox"); sub.Rare.Insert("9mm_100RndBox");
		sub.Exclusive.Insert("LeatherJacket_Black"); sub.Exclusive.Insert("HuntingJacket_Autumn"); sub.Exclusive.Insert("Marmalade");
		cat.SubGroups.Insert(sub);

		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "footwear"; sub.Weight = 0.30;
		sub.Common.Insert("Sneakers_Green"); sub.Common.Insert("AthleticShoes_Black");
		sub.Uncommon.Insert("WorkingBoots_Brown"); sub.Uncommon.Insert("WorkingBoots_Green");
		sub.Rare.Insert("WellingtonBoots_Black"); sub.Rare.Insert("HikingBoots_Brown");
		sub.Exclusive.Insert("HighBoots");
		cat.SubGroups.Insert(sub);

		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "snacks"; sub.Weight = 0.20;
		sub.Common.Insert("SodaCan_Cola"); sub.Common.Insert("SodaCan_Kvass"); sub.Common.Insert("Apple");
		sub.Uncommon.Insert("Chips"); sub.Uncommon.Insert("BakedBeansCan"); sub.Uncommon.Insert("SardinesCan");
		sub.Rare.Insert("Chocolate"); sub.Rare.Insert("TunaCan"); sub.Rare.Insert("Honey");
		sub.Exclusive.Insert("TacticalBaconCan"); sub.Exclusive.Insert("Vodka");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 2. КУХOННЫЙ ХОЛОДИЛЬНИК (refrigerator)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "refrigerator";
		cat.LootChance = 0.35; cat.NeedsGloves = false; cat.NeedsTool = false;
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "food"; sub.Weight = 0.70;
		sub.Common.Insert("Plum"); sub.Common.Insert("Pear"); sub.Common.Insert("Apple");
		sub.Uncommon.Insert("BakedBeansCan"); sub.Uncommon.Insert("SardinesCan");
		sub.Rare.Insert("TunaCan"); sub.Rare.Insert("Honey");
		sub.Exclusive.Insert("TacticalBaconCan"); sub.Exclusive.Insert("Marmalade");
		cat.SubGroups.Insert(sub);
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "drinks"; sub.Weight = 0.30;
		sub.Common.Insert("SodaCan_Cola"); sub.Common.Insert("SodaCan_Pipsi");
		sub.Uncommon.Insert("PowderedMilk");
		sub.Rare.Insert("WaterBottle");
		sub.Exclusive.Insert("Vodka");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 3. КУХОННЫЙ БУФЕТ (kitchen_sideboard)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "kitchen_sideboard";
		cat.LootChance = 0.45; cat.NeedsGloves = false; cat.NeedsTool = false;
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "dry_food"; sub.Weight = 1.00;
		sub.Common.Insert("SodaCan_Cola"); sub.Common.Insert("Crackers");
		sub.Uncommon.Insert("Chips"); sub.Uncommon.Insert("BakedBeansCan"); sub.Uncommon.Insert("SardinesCan");
		sub.Rare.Insert("Chocolate"); sub.Rare.Insert("TunaCan"); sub.Rare.Insert("Honey");
		sub.Exclusive.Insert("TacticalBaconCan"); sub.Exclusive.Insert("Marmalade"); sub.Exclusive.Insert("Vodka");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 4. КНИЖНЫЙ ШКАФ (bookshelf)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "bookshelf";
		cat.LootChance = 0.25; cat.NeedsGloves = false; cat.NeedsTool = false;
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "reading_and_fire"; sub.Weight = 1.00;
		sub.Common.Insert("Matchbox"); sub.Uncommon.Insert("Matchbox"); sub.Rare.Insert("MapChernarus"); sub.Exclusive.Insert("MapChernarus");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 5. РАКОВИНА В ВАННОЙ (bathroom_sink)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "bathroom_sink";
		cat.LootChance = 0.30; cat.NeedsGloves = false; cat.NeedsTool = false;
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "pills"; sub.Weight = 1.00;
		sub.Rare.Insert("CharcoalTablets"); sub.Rare.Insert("VitaminBottle"); sub.Exclusive.Insert("CharcoalTablets"); sub.Exclusive.Insert("VitaminBottle");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);
	}
	// ============================================================================
	// ЧАСТЬ 3: ИЗОЛИРОВАННЫЙ МЕДИЦИНСКИЙ СЕКТОР (МЕТОД 2 ИЗ 5)
	// ============================================================================
	private static void GenerateMedicalConfig()
	{
		HUSHazardCategoryData cat;
		HUSHazardSubGroupData sub;

		// 6. ДОМАШНЯЯ АПТЕЧКА (home_medical_cabinet)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "home_medical_cabinet";
		cat.LootChance = 0.50; cat.NeedsGloves = true; cat.GloveWearHP = 5.0; cat.NeedsTool = true; cat.ToolClass = "Crowbar"; cat.ToolWearHP = 15.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "home_meds"; sub.Weight = 1.00;
		sub.Common.Insert("BandageDressings"); sub.Common.Insert("PurificationTablets");
		sub.Uncommon.Insert("PainkillerTablets"); sub.Uncommon.Insert("CharcoalTablets"); sub.Uncommon.Insert("VitaminBottle");
		sub.Rare.Insert("TetracyclineAntibiotics"); sub.Rare.Insert("AlcoholTincture");
		sub.Exclusive.Insert("Morphine"); sub.Exclusive.Insert("FirstAidKit");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 7. МЕДИЦИНСКИЙ СТЕЛЛАЖ / РАСХОДНИКИ (medical_supplies)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "medical_supplies";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "hospital_gear"; sub.Weight = 1.00;
		sub.Common.Insert("BandageDressings"); sub.Common.Insert("PurificationTablets");
		sub.Uncommon.Insert("BloodBagEmpty"); sub.Uncommon.Insert("SalineBag"); sub.Uncommon.Insert("StartKitIV"); sub.Uncommon.Insert("AlcoholTincture");
		sub.Rare.Insert("Thermometer"); sub.Rare.Insert("SurgicalPlasticBag"); sub.Rare.Insert("BloodTestKit");
		sub.Exclusive.Insert("Defibrillator"); sub.Exclusive.Insert("FirstAidKit");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 8. МЕДИЦИНСКИЙ ШКАФ / ОДЕЖДА (medical_clothing)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "medical_clothing";
		cat.LootChance = 0.45; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "hospital_clothes"; sub.Weight = 1.00;
		sub.Common.Insert("SurgicalGloves_Blue"); sub.Common.Insert("SurgicalGloves_LightBlue"); sub.Common.Insert("N95Mask_White");
		sub.Uncommon.Insert("MedicalScrubsHat_Blue"); sub.Uncommon.Insert("MedicalScrubsHat_White"); sub.Uncommon.Insert("N95Mask_Green");
		sub.Rare.Insert("MedicalScrubsPants_Blue"); sub.Rare.Insert("MedicalScrubsPants_White");
		sub.Exclusive.Insert("MedicalScrubsJacket_Blue"); sub.Exclusive.Insert("MedicalScrubsJacket_White");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 9. ЦЕНТРАЛЬНЫЙ ГОСПИТАЛЬНЫЙ СЕЙФ (hospital_medical_lockbox)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "hospital_medical_lockbox";
		cat.LootChance = 0.60; cat.NeedsGloves = true; cat.GloveWearHP = 8.0; cat.NeedsTool = true; cat.ToolClass = "Crowbar"; cat.ToolWearHP = 20.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "elite_meds"; sub.Weight = 1.00;
		sub.Common.Insert("PainkillerTablets"); sub.Common.Insert("CharcoalTablets"); sub.Common.Insert("VitaminBottle");
		sub.Uncommon.Insert("TetracyclineAntibiotics"); sub.Uncommon.Insert("Epinephrine");
		sub.Rare.Insert("Morphine"); sub.Rare.Insert("SalineBagIV");
		sub.Exclusive.Insert("Epinephrine"); sub.Exclusive.Insert("Morphine");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 10. ЗАПЕРТЫЙ СЕЛЬСКИЙ МЕДПУНКТ (village_medical_lockbox)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "village_medical_lockbox";
		cat.LootChance = 0.55; cat.NeedsGloves = true; cat.GloveWearHP = 6.0; cat.NeedsTool = true; cat.ToolClass = "Crowbar"; cat.ToolWearHP = 18.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "village_meds"; sub.Weight = 0.70;
		sub.Common.Insert("BandageDressings"); sub.Common.Insert("PurificationTablets");
		sub.Uncommon.Insert("PainkillerTablets"); sub.Uncommon.Insert("CharcoalTablets"); sub.Uncommon.Insert("VitaminBottle");
		sub.Rare.Insert("TetracyclineAntibiotics"); sub.Rare.Insert("AlcoholTincture");
		sub.Exclusive.Insert("Morphine"); sub.Exclusive.Insert("FirstAidKit");
		cat.SubGroups.Insert(sub);
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "village_masks"; sub.Weight = 0.30;
		sub.Rare.Insert("GP5GasMask"); sub.Rare.Insert("GasMask_Filter");
		sub.Exclusive.Insert("GasMask"); sub.Exclusive.Insert("GasMask_Filter");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 11. НАСТЕННЫЙ ШКОЛЬНЫЙ МЕДПУНКТ (school_medical_box)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "school_medical_box";
		cat.LootChance = 0.50; cat.NeedsGloves = true; cat.GloveWearHP = 4.0; cat.NeedsTool = true; cat.ToolClass = "Crowbar"; cat.ToolWearHP = 12.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "school_meds"; sub.Weight = 1.00;
		sub.Common.Insert("BandageDressings");
		sub.Uncommon.Insert("PainkillerTablets"); sub.Uncommon.Insert("PurificationTablets");
		sub.Rare.Insert("CharcoalTablets"); sub.Rare.Insert("VitaminBottle");
		sub.Exclusive.Insert("TetracyclineAntibiotics"); sub.Exclusive.Insert("FirstAidKit");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);
	}
	// ============================================================================
	// ЧАСТЬ 4: ИЗОЛИРОВАННЫЙ ПОЛИЦЕЙСКИЙ И ШКОЛЬНЫЙ СЕКТОР (МЕТОД 3 ИЗ 5)
	// ============================================================================
	private static void GeneratePoliceConfig()
	{
		HUSHazardCategoryData cat;
		HUSHazardSubGroupData sub;

		// 12. ПОЛИЦЕЙСКИЙ КАНЦЕЛЯРСКИЙ ШКАФ (police_office)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "police_office";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "police_gear"; sub.Weight = 1.00;
		sub.Common.Insert("PoliceCap"); sub.Common.Insert("Footwear_BootsCargo_Black"); sub.Common.Insert("WorkingGloves_Black");
		sub.Uncommon.Insert("PoliceJacket"); sub.Uncommon.Insert("PolicePants"); sub.Uncommon.Insert("Flashlight"); sub.Uncommon.Insert("Battery9V"); sub.Uncommon.Insert("Handcuffs");
		// ОПЕЧАТКА ИСПРАВЛЕНА: .Radio заменено на .Rare
		sub.Rare.Insert("Baton"); sub.Rare.Insert("Radio"); sub.Rare.Insert("9mm_25RndBox"); sub.Rare.Insert("380_35RndBox");
		sub.Exclusive.Insert("Mag_IJ70_8Rnd"); sub.Exclusive.Insert("Mag_CZ75_15Rnd"); sub.Exclusive.Insert("Mag_Glock_15Rnd");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 13. ПОЛИЦЕЙСКИЙ ОРУЖЕЙНЫЙ СЕЙФ (police_weapon_rack)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "police_weapon_rack";
		cat.LootChance = 0.50; cat.NeedsGloves = true; cat.GloveWearHP = 10.0; cat.NeedsTool = true; cat.ToolClass = "Crowbar"; cat.ToolWearHP = 25.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "police_weapons"; sub.Weight = 1.00;
		sub.Common.Insert("9mm_25RndBox"); sub.Common.Insert("380_35RndBox");
		sub.Uncommon.Insert("Mag_IJ70_8Rnd"); sub.Uncommon.Insert("Mag_CZ75_15Rnd");
		sub.Rare.Insert("Mag_Glock_15Rnd"); sub.Rare.Insert("IJ70"); sub.Rare.Insert("CZ75");
		sub.Exclusive.Insert("Glock19"); sub.Exclusive.Insert("StunBaton");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 14. ШКОЛЬНЫЕ ШКАФЫ И ВИДЖЕТЫ (school_office)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "school_office";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "student_loot"; sub.Weight = 1.00;
		sub.Common.Insert("Paper"); sub.Common.Insert("Matchbox"); sub.Common.Insert("SodaCan_Cola"); sub.Common.Insert("TShirt_Color");
		sub.Uncommon.Insert("SchoolBag_Blue"); sub.Uncommon.Insert("SchoolBag_Red"); sub.Uncommon.Insert("BeanieHat_Black"); sub.Uncommon.Insert("Chips");
		sub.Rare.Insert("MapChernarus"); sub.Rare.Insert("Chocolate"); sub.Rare.Insert("WaterBottle");
		sub.Exclusive.Insert("TaloonBag_Orange"); sub.Exclusive.Insert("Marmalade");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);
	}
	// ============================================================================
	// ЧАСТЬ 5: ИЗОЛИРОВАННЫЙ ИНДУСТРИАЛЬНЫЙ И АВТОМОБИЛЬНЫЙ СЕКТОР (МЕТОД 4 ИЗ 5)
	// ============================================================================
	private static void GenerateIndustrialConfig()
	{
		HUSHazardCategoryData cat;
		HUSHazardSubGroupData sub;

		// 15. ТЕХНИЧЕСКИЙ ИНДУСТРИАЛЬНЫЙ СТЕЛЛАЖ (industrial)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "industrial";
		cat.LootChance = 0.45; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "tools_and_parts"; sub.Weight = 1.00;
		sub.Common.Insert("Screwdriver"); sub.Common.Insert("Wrench"); sub.Common.Insert("LugWrench"); sub.Common.Insert("Rope"); sub.Common.Insert("Nails"); sub.Common.Insert("Sinterpatch");
		sub.Uncommon.Insert("Hacksaw"); sub.Uncommon.Insert("Hammer"); sub.Uncommon.Insert("HeadlightH7"); sub.Uncommon.Insert("Wire"); sub.Uncommon.Insert("Pliers"); sub.Uncommon.Insert("Jerrycan");
		sub.Rare.Insert("PipeWrench"); sub.Rare.Insert("Crowbar"); sub.Rare.Insert("EngineOil"); sub.Rare.Insert("Hatchet"); sub.Rare.Insert("Handsaw");
		sub.Exclusive.Insert("Pickaxe"); sub.Exclusive.Insert("Sledgehammer"); sub.Exclusive.Insert("Blowtorch"); sub.Exclusive.Insert("EpoxyPutty");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 16. ШКАФ ДЛЯ РАБОЧЕЙ ОДЕЖДЫ (industrial_wardrobe)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "industrial_wardrobe";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "worker_clothes"; sub.Weight = 1.00;
		sub.Common.Insert("WorkingGloves_Brown"); sub.Common.Insert("WorkingGloves_Yellow"); sub.Common.Insert("Sneakers_Gray");
		sub.Uncommon.Insert("JumpsuitJacket_Blue"); sub.Uncommon.Insert("JumpsuitPants_Blue"); sub.Uncommon.Insert("JumpsuitJacket_Gray"); sub.Uncommon.Insert("JumpsuitPants_Gray"); sub.Uncommon.Insert("WorkingBoots_Brown");
		// ОПЕЧАТКА ИСПРАВЛЕНА: .SlingBag_Black заменено на стандартный массив .Rare
		sub.Rare.Insert("HardHat_Yellow"); sub.Rare.Insert("HardHat_Orange"); sub.Rare.Insert("SlingBag_Black"); sub.Rare.Insert("Jerrycan");
		sub.Exclusive.Insert("HeavyDutyBag"); sub.Exclusive.Insert("GorkaPants_Summer");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 17. ТЕХНИЧЕСКИЙ КНИЖНЫЙ ШКАФ ЦЕХА (industrial_bookshelf)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "industrial_bookshelf";
		cat.LootChance = 0.35; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "tech_papers"; sub.Weight = 1.00;
		sub.Common.Insert("Paper"); sub.Uncommon.Insert("Paper"); sub.Rare.Insert("Paper"); sub.Exclusive.Insert("MapChernarus");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 18. СТЕЛЛАЖ СЕЛЬСКИХ ИНСТРУМЕНТОВ (village_tools)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "village_tools";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "rural_hardware"; sub.Weight = 1.00;
		sub.Common.Insert("Rope"); sub.Common.Insert("Nails"); sub.Common.Insert("Screwdriver"); sub.Common.Insert("Hammer");
		sub.Uncommon.Insert("Hacksaw"); sub.Uncommon.Insert("Pliers"); sub.Uncommon.Insert("Wire"); sub.Uncommon.Insert("Sickle");
		sub.Rare.Insert("Hatchet"); sub.Rare.Insert("Handsaw"); sub.Rare.Insert("Crowbar"); sub.Rare.Insert("Shovel");
		sub.Exclusive.Insert("Pickaxe"); sub.Exclusive.Insert("Sledgehammer"); sub.Exclusive.Insert("WoodAxe");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 19. ПРИЛАВОК ГРАЖДАНСКОЙ АЗС (fuel_station_display)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "fuel_station_display";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "kiosk_goods"; sub.Weight = 1.00;
		sub.Common.Insert("SodaCan_Cola"); sub.Common.Insert("SodaCan_Kvass"); sub.Common.Insert("Matchbox");
		sub.Uncommon.Insert("Chips"); sub.Uncommon.Insert("BakedBeansCan"); sub.Uncommon.Insert("Battery9V"); sub.Uncommon.Insert("Flashlight");
		sub.Rare.Insert("Chocolate"); sub.Rare.Insert("TunaCan"); sub.Rare.Insert("WaterBottle"); sub.Rare.Insert("CanOpener");
		sub.Exclusive.Insert("TacticalBaconCan"); sub.Exclusive.Insert("Marmalade"); sub.Exclusive.Insert("Vodka");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 20. ОБЫСК БАГАЖНИКА АВТОМОБИЛЯ (wreck_trunk)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "wreck_trunk";
		cat.LootChance = 0.20; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "trunk_fill"; sub.Weight = 1.00;
		sub.Common.Insert("Screwdriver"); sub.Common.Insert("Wrench"); sub.Common.Insert("LugWrench"); sub.Common.Insert("Rope");
		sub.Uncommon.Insert("HeadlightH7"); sub.Uncommon.Insert("Jerrycan"); sub.Uncommon.Insert("Wire"); sub.Uncommon.Insert("Pliers"); sub.Uncommon.Insert("CanOpener");
		sub.Rare.Insert("Crowbar"); sub.Rare.Insert("EngineOil"); sub.Rare.Insert("Heatpack"); sub.Rare.Insert("MapChernarus");
		sub.Exclusive.Insert("HydraulicJack"); sub.Exclusive.Insert("TirePump");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 21. ОБЫСК КАПОТА АВТОМОБИЛЯ (wreck_engine)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "wreck_engine";
		cat.LootChance = 0.30; cat.NeedsGloves = true; cat.GloveWearHP = 10.0; cat.NeedsTool = true; cat.ToolClass = "Wrench"; cat.ToolWearHP = 5.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "engine_parts"; sub.Weight = 1.00;
		sub.Common.Insert("Wire");
		sub.Uncommon.Insert("SparkPlug");
		sub.Rare.Insert("CarBattery"); sub.Rare.Insert("CarRadiator");
		sub.Exclusive.Insert("TruckBattery"); sub.Exclusive.Insert("GlowPlug");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);
	}
	// ============================================================================
	// ЧАСТЬ 6.А: ИЗОЛИРОВАННЫЙ ВОЕННЫЙ СЕКТОР, ПОЖАРКИ И ЛАГЕРЯ (МЕТОД 5 ИЗ 5)
	// ============================================================================
	private static void GenerateMilitaryConfig()
	{
		HUSHazardCategoryData cat;
		HUSHazardSubGroupData sub;

		// 22. ОТКРЫТЫЕ КАЗАРМЕННЫЕ ШКАФЫ (military)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "military";
		cat.LootChance = 0.35; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "army_gear"; sub.Weight = 0.70;
		sub.Common.Insert("TShirt_Green"); sub.Common.Insert("WorkingGloves_Black"); sub.Common.Insert("Socks_Charcoal");
		sub.Uncommon.Insert("GasMask"); sub.Uncommon.Insert("GP5GasMask"); sub.Uncommon.Insert("GasMask_Filter"); sub.Uncommon.Insert("Canteen"); sub.Uncommon.Insert("Flashlight"); sub.Uncommon.Insert("Battery9V"); sub.Uncommon.Insert("HuntingKnife");
		sub.Rare.Insert("CombatKnife"); sub.Rare.Insert("Binoculars"); sub.Rare.Insert("Compass"); sub.Rare.Insert("MilitiaJacket_Gree"); sub.Rare.Insert("MilitiaPants_Gree");
		sub.Exclusive.Insert("NBCJacket_Gray"); sub.Exclusive.Insert("NBCPants_Gray"); sub.Exclusive.Insert("NBCBoots_Gray"); sub.Exclusive.Insert("NBCGloves_Gray"); sub.Exclusive.Insert("TortillaBag");
		cat.SubGroups.Insert(sub);
		
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "army_closet_ammo"; sub.Weight = 0.30;
		sub.Rare.Insert("545x39_20RndBox"); sub.Rare.Insert("762x39_20RndBox"); sub.Rare.Insert("556x45_20RndBox");
		sub.Exclusive.Insert("762x54_20RndBox"); sub.Exclusive.Insert("308Win_20RndBox");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 23. ЗАПЕРТЫЕ ОРУЖЕЙНЫЕ СЕЙФЫ И СТОЛЫ «ТИСЫ» (military_weapon_rack)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "military_weapon_rack";
		cat.LootChance = 0.45; cat.NeedsGloves = true; cat.GloveWearHP = 12.0; cat.NeedsTool = true; cat.ToolClass = "Crowbar"; cat.ToolWearHP = 30.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "army_munitions"; sub.Weight = 1.00;
		sub.Uncommon.Insert("Mag_AK74_30Rnd"); sub.Uncommon.Insert("Mag_STANAG_30Rnd");
		sub.Rare.Insert("545x39_20RndBox"); sub.Rare.Insert("762x39_20RndBox"); sub.Rare.Insert("556x45_20RndBox"); sub.Rare.Insert("Mag_AKM_30Rnd");
		sub.Exclusive.Insert("762x54_20RndBox"); sub.Exclusive.Insert("308Win_20RndBox"); sub.Exclusive.Insert("Mag_SVD_10Rnd"); sub.Exclusive.Insert("Mag_FAL_20Rnd");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 24. ОТКРЫТЫЕ АММО-БОКСЫ В АНГAРАХ (military_ammo_box)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "military_ammo_box";
		cat.LootChance = 0.50; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "basic_ammo"; sub.Weight = 1.00;
		sub.Common.Insert("9mm_25RndBox"); sub.Common.Insert("380_35RndBox");
		sub.Uncommon.Insert("545x39_20RndBox"); sub.Uncommon.Insert("762x39_20RndBox"); sub.Uncommon.Insert("556x45_20RndBox");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 25. ОБЫСК ВОЕННЫХ ПАЛАТОК (military_tent)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "military_tent";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "tent_drop"; sub.Weight = 1.00;
		sub.Common.Insert("TShirt_Green"); sub.Common.Insert("Socks_Charcoal"); sub.Common.Insert("9mm_25RndBox");
		sub.Uncommon.Insert("AssaultBag_Green"); sub.Uncommon.Insert("Canteen"); sub.Uncommon.Insert("GasMask_Filter"); sub.Uncommon.Insert("545x39_20RndBox");
		sub.Rare.Insert("MilitiaJacket_Gree"); sub.Rare.Insert("MilitiaPants_Gree"); sub.Rare.Insert("Compass"); sub.Rare.Insert("Binoculars");
		sub.Exclusive.Insert("CoyoteBag_Khaki"); sub.Exclusive.Insert("TortillaBag");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 26. ТЕХНИЧЕСКИЙ ВОЕННЫЙ ШКАФ ШТАБА (military_bookshelf)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "military_bookshelf";
		cat.LootChance = 0.30; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "hq_papers"; sub.Weight = 1.00;
		sub.Common.Insert("Paper"); sub.Uncommon.Insert("Paper"); sub.Rare.Insert("MapChernarus"); sub.Exclusive.Insert("MapChernarus");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 27. ВОЕННЫЙ ХОЛОДИЛЬНИК РАДАРОВ (military_refrigerator)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "military_refrigerator";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "army_rations"; sub.Weight = 1.00;
		sub.Common.Insert("WaterBottle"); sub.Uncommon.Insert("Canteen"); sub.Uncommon.Insert("Crackers"); sub.Rare.Insert("TacticalBaconCan"); sub.Exclusive.Insert("TacticalBaconCan"); sub.Exclusive.Insert("Vodka");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 28. УЛИЧНЫЙ СЕКТОР: ТУАЛЕТЫ (toilet_dry)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "toilet_dry";
		cat.LootChance = 0.35; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "outhouse_finds"; sub.Weight = 1.00;
		sub.Common.Insert("Paper"); sub.Common.Insert("Matchbox"); sub.Common.Insert("Bandana_Black");
		sub.Uncommon.Insert("WorkingGloves_Brown"); sub.Uncommon.Insert("SodaCan_Kvass");
		sub.Rare.Insert("KitchenKnife"); sub.Rare.Insert("380_35RndBox"); sub.Rare.Insert("9mm_25RndBox");
		sub.Exclusive.Insert("AlcoholTincture"); sub.Exclusive.Insert("CharcoalTablets");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 29. ТУРИСТИЧЕСКИЕ ЛАГЕРИ (camp_house_wardrobe)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "camp_house_wardrobe";
		cat.LootChance = 0.45; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "scout_gear"; sub.Weight = 1.00;
		sub.Common.Insert("Matchbox"); sub.Common.Insert("SodaCan_Kvass"); sub.Common.Insert("Bandana_Red");
		sub.Uncommon.Insert("TaloonBag_Blue"); sub.Uncommon.Insert("TaloonBag_Orange"); sub.Uncommon.Insert("HuntingKnife"); sub.Uncommon.Insert("Compass");
		sub.Rare.Insert("HuntingBag"); sub.Rare.Insert("HuntingJacket_Autumn"); sub.Rare.Insert("HuntingPants_Autumn"); sub.Rare.Insert("Heatpack");
		sub.Exclusive.Insert("Marmalade"); sub.Exclusive.Insert("MilitiaJacket_Gree"); sub.Exclusive.Insert("HighBoots");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 30. ОТКРЫТЫЙ СТЕЛЛАЖ ПОЖАРНОЙ ЧАСТИ (fire_station_supplies)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "fire_station_supplies";
		cat.LootChance = 0.40; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "rescue_gear"; sub.Weight = 1.00;
		sub.Common.Insert("Wire"); sub.Common.Insert("Rope"); sub.Common.Insert("Screwdriver");
		sub.Uncommon.Insert("Radio"); sub.Uncommon.Insert("Battery9V"); sub.Uncommon.Insert("Flashlight");
		sub.Rare.Insert("Crowbar"); sub.Rare.Insert("Pliers");
		sub.Exclusive.Insert("Blowtorch");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 31. ЗАПЕРТЫЙ ШКАФ РАЗДЕВАЛКИ ПОЖАРНЫХ (fire_station_wardrobe)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "fire_station_wardrobe";
		cat.LootChance = 0.50; cat.NeedsGloves = true; cat.GloveWearHP = 8.0; cat.NeedsTool = true; cat.ToolClass = "Crowbar"; cat.ToolWearHP = 22.0;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "fireman_outfit"; sub.Weight = 1.00;
		sub.Common.Insert("WorkingGloves_Yellow");
		sub.Uncommon.Insert("FirefighterHelmet_Red"); sub.Uncommon.Insert("FirefighterHelmet_Yellow");
		sub.Rare.Insert("FirefighterJacket_Red"); sub.Rare.Insert("FirefighterPants_Red"); sub.Rare.Insert("FirefighterAxe");
		sub.Exclusive.Insert("FirefighterAxe"); sub.Exclusive.Insert("GorkaPants_Summer");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);

		// 32. АВТОМАТЫ С НАПИТКАМИ ГOРOДА (vending_drinks)
		cat = new HUSHazardCategoryData();
		cat.CategoryName = "vending_drinks";
		cat.LootChance = 0.60; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "soda"; sub.Weight = 1.00;
		sub.Common.Insert("SodaCan_Cola"); sub.Common.Insert("SodaCan_Pipsi");
		sub.Uncommon.Insert("SodaCan_Spite"); sub.Uncommon.Insert("SodaCan_Kvass");
		sub.Rare.Insert("WaterBottle"); sub.Exclusive.Insert("WaterBottle");
		cat.SubGroups.Insert(sub);
		m_LootConfig.Categories.Insert(cat);
		
	}

	// ============================================================================
	// СЛУЖЕБНЫЕ МЕТОДЫ ПОИСКА ДАННЫХ В ДИНАМИЧЕСКИХ С++ МАССИВАХ (ФИКС МАР)
	// ============================================================================
	private static HUSHazardCategoryData GetCategoryDataByName(string name)
	{
		if (!m_LootConfig || !m_LootConfig.Categories) return null;
		
		for (int i = 0; i < m_LootConfig.Categories.Count(); i++)
		{
			HUSHazardCategoryData cat = m_LootConfig.Categories.Get(i);
			if (cat && cat.CategoryName == name) return cat;
		}
		return null;
	}

		// ИСПРАВЛЕНО: Базовые пулы для уличных объектов, которых не было в вашей сборке
	private static void GenerateStreetStaticConfig()
	{
		HUSHazardCategoryData cat; HUSHazardSubGroupData sub;

		// СОБАЧЬЯ БУДКА (dog_kennel)
		cat = new HUSHazardCategoryData(); cat.CategoryName = "dog_kennel"; cat.LootChance = 0.50; cat.NeedsGloves = true; cat.GloveWearHP = 3.0; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "kennel_finds"; sub.Weight = 1.00;
		sub.Common.Insert("Bone"); sub.Common.Insert("Bone"); sub.Uncommon.Insert("Rope"); sub.Uncommon.Insert("Pelt_Wolf"); sub.Rare.Insert("Worm"); sub.Exclusive.Insert("HuntingKnife");
		cat.SubGroups.Insert(sub); m_LootConfig.Categories.Insert(cat);

		// КУРЯТНИК (chicken_coop)
		cat = new HUSHazardCategoryData(); cat.CategoryName = "chicken_coop"; cat.LootChance = 0.45; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "coop_finds"; sub.Weight = 1.00;
		sub.Common.Insert("ChickenFeather"); sub.Common.Insert("ChickenFeather"); sub.Uncommon.Insert("Worm"); sub.Rare.Insert("Matchbox"); cat.SubGroups.Insert(sub); 
		m_LootConfig.Categories.Insert(cat);

		// СЕЛЬСКИЙ ТУАЛЕТ (toilet_dry)
		cat = new HUSHazardCategoryData(); cat.CategoryName = "toilet_dry"; cat.LootChance = 0.35; cat.NeedsGloves = false; cat.NeedsTool = false;
		sub = new HUSHazardSubGroupData(); sub.SubGroupName = "outhouse_finds"; sub.Weight = 1.00;
		sub.Common.Insert("Paper"); sub.Common.Insert("Matchbox"); sub.Uncommon.Insert("WorkingGloves_Brown"); sub.Rare.Insert("KitchenKnife"); cat.SubGroups.Insert(sub); 
		m_LootConfig.Categories.Insert(cat);
	}


	// ============================================================================
	// ТРЕХСТУПЕНЧАТЫЙ МАТЕМАТИЧЕСКИЙ КАЛЬКУЛЯТОР ШАНСOВ ВЫДАЧИ ПРЕДМЕТOВ
	// ============================================================================
	static void ProcessSearch(PlayerBase player, Object target, string proxyName)
	{
		Print("[HUSHazard_DEBUG]: Метод ProcessSearch запущен! Цель: " + target.GetType() + " | Переданный маркер: '" + proxyName + "'");

		if (!GetGame().IsServer() || !player || proxyName == string.Empty || !target) 
		{
			return;
		}

		string lowerProxyName = proxyName;
		lowerProxyName.ToLower();

		// ============================================================================
		// ИСПРАВЛЕННЫЙ ШЛЮЗ КУЛДАУНОВ: ИСКЛЮЧИТЕЛЬНО ДЛЯ МАШИН И УЛИЧНЫХ ОБЪЕКТОВ
		// ============================================================================
		string targetID = target.GetID().ToString();
		string checkKey = "";

		// Маппим ключи строго для автомобилей и уличной статики
		if (lowerProxyName == "wreck_trunk")       checkKey = targetID + "_trunk";
		else if (lowerProxyName == "wreck_engine") checkKey = targetID + "_engine";
		else if (lowerProxyName == "toilet_dry" || lowerProxyName == "dog_kennel" || lowerProxyName == "chicken_coop") checkKey = targetID + "_street";

		// Если это машина или уличный объект, и ключ кулдауна найден в единой базе
		if (checkKey != string.Empty && ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(checkKey))
		{
			int cooldownEndTime = ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(checkKey);
			if (GetGame().GetTime() < cooldownEndTime)
			{
				// Изнашиваем перчатки на полные 2.0 единицы за спам по облутанным машинам/улице
				EntityAI glovesOnCool = player.GetInventory().FindAttachment(InventorySlots.GLOVES);
				if (glovesOnCool && !glovesOnCool.IsRuined())
				{
					glovesOnCool.DecreaseHealth("", "", 1.0);
					Print("[HUSHazard_Gloves]: Машина/Улица на кулдауне! Перчатки игрока изношены на 2 единицы.");
				}

				player.MessageAction("[HUSHazard]: Всё украдено до нас... Здесь уже абсолютно пусто.");
				return; // Прерываем генерацию предметов
			}
		}
		
		// МЕБЕЛЬ ПРОПУСКАЕТСЯ ДАЛЬШЕ И ЗДЕСЬ НЕ ПРОВЕРЯЕТСЯ (КУЛДАУН ШКАФОВ В ACTIONCONDITION)

		if (!m_LootConfig || !m_LootConfig.Categories || m_LootConfig.Categories.Count() == 0)
		{
			Init();
		}

		HUSHazardCategoryData jsonCat = GetCategoryDataByName(lowerProxyName);
		if (!jsonCat)
		{
			Print("[HUSHazard_DEBUG]: Отмена! Категория '" + lowerProxyName + "' отсутствует в JSON-файле!");
			ref Param1<string> mParam = new Param1<string>("[HUSHazard]: Ошибка: Категория '" + lowerProxyName + "' отсутствует в JSON!");
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, mParam, true, player.GetIdentity());
			return;
		}

		Print("[HUSHazard_DEBUG]: Категория найдена в JSON! Глобальный шанс обыска: " + jsonCat.LootChance.ToString());

		EntityAI tool = player.GetHumanInventory().GetEntityInHands();
		EntityAI gloves = player.GetInventory().FindAttachment(InventorySlots.GLOVES);

		// --- ХАРДКОРНЫЙ ШЛЮЗ №1: ПРОВЕРКА И ИЗНOС ИНСТРУМЕНТА ДЛЯ КАПOТA ---
		if (lowerProxyName == "wreck_engine")
		{
			if (!tool || (tool.GetType() != "PipeWrench" && !tool.GetType().Contains("Wrench")))
			{
				ref Param1<string> errParam = new Param1<string>("[HUSHazard]: Ошибка: Для разбора капота необходим Гаечный или Разводной ключ!");
				GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, errParam, true, player.GetIdentity());
				return;
			}
			float maxToolHP = tool.GetMaxHealth("", "Health");
			tool.AddHealth("", "Health", -(maxToolHP * 0.05));
			
			ref Param1<string> msgWrench = new Param1<string>("[HUSHazard]: Разводной ключ изнашивается при откручивании ржавых болтов!");
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msgWrench, true, player.GetIdentity());
			
			if (tool.GetHealth("", "Health") <= 0)
			{
				ref Param1<string> breakWrench = new Param1<string>("[HUSHazard]: Внимание! Ваш рабочий ключ полностью СЛОМАЛСЯ!");
				GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, breakWrench, true, player.GetIdentity());
			}

			if (!gloves || gloves.IsDamageDestroyed())
			{
				if (player.GetBleedingManagerServer())
				{
					// ИСПРАВЛЕНО ДЛЯ DAYZ 1.29: Используем нативный ID слота инвентаря для рук
					player.GetBleedingManagerServer().AttemptAddBleedingSource(InventorySlots.GLOVES);
					ref Param1<string> bleedMsg = new Param1<string>("[HUSHazard]: Вы глубоко распороли голые руки о ржавый болт!");
					GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, bleedMsg, true, player.GetIdentity());
				}
			}
			else 
			{
				// ИСПРАВЛЕНО ДЛЯ DAYZ 1.29: Заданы правильные 3 параметра для DecreaseHealth
				gloves.DecreaseHealth("", "Health", 10.0);
			}
		}
		// --- ОБЩАЯ ПРОВЕРКА ИНСТРУМЕНТOВ ДЛЯ ЗАПЕРТЫХ СЕЙФOВ И ШКАФOВ ---
		else if (jsonCat.NeedsTool)
		{
			if (!tool || (!tool.GetType().Contains("Crowbar") && !tool.GetType().Contains("Axe") && !tool.GetType().Contains("Sledgehammer") && !tool.GetType().Contains("Pickaxe")))
			{
				ref Param1<string> lockParam = new Param1<string>("[HUSHazard]: Ошибка: Этот шкаф заперт! Требуется Лом, Топор, Кувалда или Кирка!");
				GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, lockParam, true, player.GetIdentity());
				return;
			}
			// ИСПРАВЛЕНО ДЛЯ DAYZ 1.29: Заданы правильные 3 параметра для DecreaseHealth
			tool.DecreaseHealth("", "Health", jsonCat.ToolWearHP);
			ref Param1<string> toolMsg = new Param1<string>("[HUSHazard]: Вы взломали замок! Ваш инструмент износился.");
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, toolMsg, true, player.GetIdentity());
		}

		// Автоматический износ перчаток в обычных точках (если настроено в JSON)
		if (lowerProxyName != "wreck_engine" && jsonCat.NeedsGloves && jsonCat.GloveWearHP > 0.0 && gloves && !gloves.IsDamageDestroyed())
		{
			gloves.DecreaseHealth("", "Health", jsonCat.GloveWearHP);
		}

		// --- СТУПЕНЬ 1: БРOСOК НА ГЛOБAЛЬНЫЙ ШАНС ОБЫСКА (LootChance) ---
		float roll = Math.RandomFloat01();
		if (roll > jsonCat.LootChance)
		{
			Print("[HUSHazard_DEBUG]: Кубик выбросил " + roll.ToString() + ". Шанс провален!");
			ref Param1<string> failParam = new Param1<string>("[HUSHazard]: Обыск завершен: ничего полезного не найдено.");
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, failParam, true, player.GetIdentity());
			return;
		}

		if (!jsonCat.SubGroups || jsonCat.SubGroups.Count() == 0) return;

		// --- СТУПЕНЬ 2: РOЗЫГРЫШ ВНУТРЕННЕЙ ПОДГРУППЫ ТOВAРOВ ПО ВЕСAМ (Weight) ---
		float subGroupRoll = Math.RandomFloat(0.0, 1.0);
		float currentWeightSum = 0.0;
		HUSHazardSubGroupData rolledGroup = null;

		for (int i = 0; i < jsonCat.SubGroups.Count(); i++)
		{
			HUSHazardSubGroupData subData = jsonCat.SubGroups.Get(i);
			if (!subData) continue;
			
			currentWeightSum += subData.Weight;
			if (subGroupRoll <= currentWeightSum)
			{
				rolledGroup = subData;
				break;
			}
		}

		if (!rolledGroup) rolledGroup = jsonCat.SubGroups.Get(0);

		// --- СТУПЕНЬ 3: ВЫБOР ТИРA РЕДКOСТИ ВНУТРИ ВЫПАВШЕЙ ГРУППЫ ---
		ref array<string> targetItemPool = rolledGroup.Common; 
		float rarityRoll = Math.RandomFloat01();

		bool isTruck = target && (target.GetType().Contains("m3s") || target.GetType().Contains("praga") || target.GetType().Contains("truck"));
		bool isHummer = target && target.GetType().Contains("hummer");

		if (rarityRoll <= 0.03) targetItemPool = rolledGroup.Exclusive;      
		else if (rarityRoll <= 0.15) targetItemPool = rolledGroup.Rare;       
		else if (rarityRoll <= 0.40) targetItemPool = rolledGroup.Uncommon;   
		else targetItemPool = rolledGroup.Common;                             

		if (!targetItemPool || targetItemPool.Count() == 0) targetItemPool = rolledGroup.Common;
		if (!targetItemPool || targetItemPool.Count() == 0)
		{
			ref Param1<string> emptyParam = new Param1<string>("[HUSHazard]: Обыск завершен: ничего полезного не найдено.");
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, emptyParam, true, player.GetIdentity());
			return;
		}

		int finalRandomIndex = Math.RandomInt(0, targetItemPool.Count());
		string spawnedItemClass = targetItemPool.Get(finalRandomIndex);

		if (proxyName == "wreck_engine")
		{
			if (spawnedItemClass == "TruckBattery" && !isTruck) spawnedItemClass = "CarBattery";
			if (spawnedItemClass == "CarBattery" && isTruck) spawnedItemClass = "TruckBattery";
			if (spawnedItemClass == "GlowPlug" && !isHummer) spawnedItemClass = "SparkPlug";
			if (spawnedItemClass == "SparkPlug" && isHummer) spawnedItemClass = "GlowPlug";
		}

		// --- СПAВН ПРЕДМЕТA И ОЧИСТКA ЗЕМЛИ ЗA 10 МИНУТ ---
		vector spawnPos = player.GetPosition() + "0 0.2 0";
		Object spawnedObj = GetGame().CreateObjectEx(spawnedItemClass, spawnPos, ECE_NONE);
		
		if (spawnedObj)
		{
			ItemBase itemNode = ItemBase.Cast(spawnedObj);
			if (itemNode)
			{
				itemNode.SetLifetime(600.0); 

				// ============================================================================
				// ТЗ: ФИЛЬТР СОСТОЯНИЯ (КАЧЕСТВА) ПРЕДМЕТА
				// Шансы: Сильно повреждено 40%, Повреждено 30%, Поношено 20%, Нетронуто 10%
				// ============================================================================
				float rollQuality = Math.RandomFloat01();
				float maxHP = itemNode.GetMaxHealth("", "Health");

				if (rollQuality <= 0.40)      itemNode.SetHealth("", "Health", maxHP * 0.25); // Сильно повреждено (Badly Damaged)
				else if (rollQuality <= 0.70) itemNode.SetHealth("", "Health", maxHP * 0.50); // Повреждено (Damaged)
				else if (rollQuality <= 0.90) itemNode.SetHealth("", "Health", maxHP * 0.70); // Поношено (Worn)
				else                          itemNode.SetHealth("", "Health", maxHP);        // Нетронуто (Pristine)

				// ============================================================================
				// ТЗ: ФИЛЬТР НАПОЛНЕННОСТИ (КОЛИЧЕСТВА) ДЛЯ ПРЕДМЕТОВ
				// ============================================================================
				string lowerClass = spawnedItemClass; lowerClass.ToLower();

				if (lowerClass == "matchbox") 
				{
					itemNode.SetQuantity(Math.RandomIntInclusive(2, 5));
				}
				else if (lowerClass == "petrollighter") 
				{
					itemNode.SetQuantity(Math.RandomFloat(0.02, 0.05) * itemNode.GetQuantityMax());
				}
				else if (lowerClass.Contains("box") && (lowerClass.Contains("rnd") || lowerClass.Contains("ammo")))
				{
					itemNode.SetQuantity(Math.RandomIntInclusive(1, 5));
				}
				else if (lowerClass.Contains("tablets") || lowerClass == "vitaminbottle")
				{
					itemNode.SetQuantity(Math.RandomIntInclusive(1, 3));
				}
				else if (lowerClass == "bone")
				{
					itemNode.SetQuantity(Math.RandomIntInclusive(2, 4));
				}

				Print("[HUSHazard_DEBUG]: УСПЕХ! Спавним предмет: " + spawnedItemClass + " с кастомным качеством/количеством.");
				
				// ИСПРАВЛЕНО: Заменили капризный RPC на нативный MessageAction, чтобы надпись гарантированно зажглась
				player.MessageAction("[HUSHazard]: Кажется, что-то нашлось... Заберите вещь с земли.");
			}
		}
		else
		{
			// ИСПРАВЛЕНО: Добавлен блок провала. Теперь если кубик выдал неудачу под капотом или в багажнике,
			// персонаж мгновенно увидит эту надпись на экране!
			player.MessageAction("[HUSHazard]: Обыск завершен: ничего полезного не найдено.");
			Print("[HUSHazard_DEBUG]: Бросок кубика провален для категории: " + lowerProxyName);
		}

		// Сетевой RPC-импульс на клиент для старта локального кулдауна
		string uniqueCooldownKey = target.GetID().ToString() + "_" + lowerProxyName;
		ref Param1<string> rpcKeyParam = new Param1<string>(uniqueCooldownKey);
		GetGame().RPCSingleParam(player, 95202, rpcKeyParam, true, player.GetIdentity());
	}

	// ============================================================================
// ГЛОБАЛЬНАЯ КАТЕГОРИЯ БЕЛОГО СПИСКА ИНСТРУМЕНТОВ ВЗЛОМА TRINITYPVE
// ============================================================================
static ref array<string> m_ValidTools;

static bool PlayerHasValidToolForCategory(string categoryName, ItemBase itemInHands)
{
	bool bIsLockedCabinet = false;
	if (categoryName == "medical" || categoryName.Contains("safe") || categoryName.Contains("lock") || categoryName.Contains("cabinet")) bIsLockedCabinet = true;

	// ----------------============================================================
	// СЦЕНАРИЙ 1: ШКАФ ОТКРЫТЫЙ (ИЛИ БАГАЖНИК МАШИНЫ) — ТРЕБУЮТСЯ СТРОГО ГОЛЫЕ РУКИ!
	// ----------------============================================================
	if (!bIsLockedCabinet)
	{
		if (!itemInHands) return true;
		return false;
	}

	// ----------------============================================================
	// СЦЕНАРИЙ 2: ШКАФ ЗАПЕРТ (СЕЙФ/АПТЕЧКА) — ТРЕБУЕТСЯ ИНСТРУМЕНТ ИЗ БЕЛОГО СПИСКА
	// ----------------============================================================
	if (!itemInHands) return false; 

	if (!m_ValidTools)
	{
		m_ValidTools = new array<string>;
		m_ValidTools.Insert("Crowbar");
		m_ValidTools.Insert("FirefighterAxe"); // Строгое фабричное имя тяжелого топора
		m_ValidTools.Insert("WoodAxe");        // Строгое фабричное имя тяжелого топора
		m_ValidTools.Insert("SledgeHammer");
		m_ValidTools.Insert("Pickaxe");
		m_ValidTools.Insert("PipeWrench");
	}

	string currentToolType = itemInHands.GetType();

	// Проверяем инструмент по нашей категории белого списка взлома
	for (int i = 0; i < m_ValidTools.Count(); i++)
	{
		// НАМЕРТВО ЗАФИКСИРОВАНО: Строгое побуквенное равенство типов '==' 
		// Это полностью исключает ложный пропуск маленького Hatchet в запертые сейфы!
		if (currentToolType == m_ValidTools.Get(i))
		{
			return true; 
		}
	}

	return false;
}

	// ============================================================================
	// ДОБАВИТЬ В САМЫЙ КОНЕЦ КЛАССА HUSHazardServerManager (ПЕРЕД ПОСЛЕДНЕЙ СКОБКОЙ)
	// ============================================================================
	
	static bool IsHospitalTable(string typeName)
	{
		if (typeName.Contains("medical_table") || typeName.Contains("lab_bench") || typeName.Contains("hospital_transport")) return true;
		return false;
	}

	static string GetFurnitureAudioTag(Object targetObj)
	{
		if (!targetObj) return "HH_Trunk_Search_SoundSet";
		
		string typeName = targetObj.GetType();
		typeName.ToLower();

		// а. Холодильники
		if (typeName.Contains("fridge"))
		{
			return "HH_Fridge_Search_SoundSet";
		}
		
		// б. Книжные шкафы и офисные стеллажи
		if (typeName.Contains("library") || typeName.Contains("school") || typeName.Contains("shelf"))
		{
			return "HH_Books_Search_SoundSet";
		}

		// в. Шкафы для одежды и гардеробы
		if (typeName.Contains("almara") || typeName.Contains("clothing") || typeName.Contains("case_bedroom"))
		{
			return "HH_Clothes_Search_SoundSet";
		}

		// г. Металлические шкафы, сейфы и аптечки
		if (typeName.Contains("locker") || typeName.Contains("lekarnicka") || typeName.Contains("safe") || typeName.Contains("crate") || typeName.Contains("metalcase"))
		{
			return "HH_MetalCabinet_Search_SoundSet";
		}

		// д. Деревянные столы, доступные для взлома в больницах
		if (IsHospitalTable(typeName) || typeName.Contains("table") || typeName.Contains("desk"))
		{
			return "HH_Table_Search_SoundSet";
		}

		// Откат по умолчанию для багажников машин
		return "HH_Trunk_Search_SoundSet";
	}





};


// ============================================================================
// МОДУЛЬНЫЙ МОСТ 4_WORLD: СТАБИЛЬНЫЙ ЗАПУСК ЭКОНОМИКИ И ПОЛНЫЙ РЕЕСТР ЭКШЕНОВ
// ============================================================================
modded class PlayerBase
{
	// 1. ВАШ ВАНИЛЬНЫЙ МЕТОД ИНИЦИАЛИЗАЦИИ ЭКОНОМИКИ И ОЧИСТКИ РУК ПРИ КРАШАХ
	override void EEInit()
	{
		super.EEInit();
		
		if (GetGame().IsServer())
		{
			// Запуск автогенерации и загрузки вашей экономики лута
			HUSHazardServerManager.Init();

			// АБСОЛЮТНЫЙ ФИЛЬТР БЕЗОПАСНОСТИ: Защита от краша сервера во время обыска!
			if (GetHumanInventory())
			{
				EntityAI itemInHands = GetHumanInventory().GetEntityInHands();
				if (itemInHands && itemInHands.GetType() == "HH_InvisibleProxy_Item")
				{
					GetGame().ObjectDelete(itemInHands);
				}
			}
		}
	}

	// 2. ХАРДКОРНАЯ ПРОВЕРКА УСЛОВИЙ ТАКТИЧЕСКОГО КРИКА TRINITYPVE
	bool HH_CanScream()
	{
		if (!IsAlive()) return false;
		
		// Крик доступен только если НЕТ перчаток
		EntityAI gloves = GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (gloves && !gloves.IsDamageDestroyed()) return false;
		
		// Крик доступен только если НЕТ маски
		EntityAI mask = GetInventory().FindAttachment(InventorySlots.MASK);
		if (mask && !mask.IsDamageDestroyed()) return false;
		
		return true;
	}

	// 3. ВАШ ИСПРАВЛЕННЫЙ РЕЕСТР ОРИГИНАЛЬНЫХ ЭКШЕНОВ ОБЫСКА НА КЛАВИШУ "F" И КРИКА НА ЛКМ
	override void SetActions()
	{
		super.SetActions();
		
		// Обыски объектов по кнопке F
		AddAction(ActionSearchHazard);
		AddAction(ActionSearchZombie);
		AddAction(ActionSearchTrunkWreck);   
		AddAction(ActionSearchEngineWreck);  
		AddAction(ActionSearchFurniture);

		// Регистрация кастомного экшена свиста на ЛКМ кулаками
		AddAction(ActionHH_Scream);
	}
}
