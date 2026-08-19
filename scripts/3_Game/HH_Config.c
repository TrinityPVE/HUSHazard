// ============================================================================
// HUSHazard - Hard Universal Search & Hazard Mod (Patched for DayZ 1.29)
// Holder Config Module - Multi-Tier Loot Matrices & Coordinates (PART 1 OF 5)
// ============================================================================

class HUSHazardSubCategory
{
	string SubName;
	float DropWeight;
	ref array<string> Common;
	ref array<string> Uncommon;
	ref array<string> Rare;
	ref array<string> Exclusive;

	void HUSHazardSubCategory()
	{
		Common = new array<string>();
		Uncommon = new array<string>();
		Rare = new array<string>();
		Exclusive = new array<string>();
	}
}

class HUSHazardCategory
{
	string ProxyName;
	float LootChance;
	bool NeedsGloves;
	float GloveWearHP;
	bool NeedsTool;
	string ToolClass;
	float ToolWearHP;
	
	ref array<ref HUSHazardSubCategory> SubGroups;

	void HUSHazardCategory()
	{
		SubGroups = new array<ref HUSHazardSubCategory>();
	}
}

class HUSHazardConfig
{
	ref array<ref HUSHazardCategory> Categories;

	void HUSHazardConfig()
	{
		Categories = new array<ref HUSHazardCategory>();
	}
}

class HUSHazardConfigHolder
{
	private static ref HUSHazardConfig m_Instance;
	private static ref TStringArray m_AllowedModels;

	static void SetConfig(HUSHazardConfig config) { m_Instance = config; }
	static HUSHazardConfig GetConfig() { return m_Instance; }

	static void InitAllowedModels()
	{
		if (!m_AllowedModels) m_AllowedModels = new TStringArray();
		m_AllowedModels.Clear();
		m_AllowedModels.Insert("kennel");
		m_AllowedModels.Insert("dog");
		m_AllowedModels.Insert("coop");
		m_AllowedModels.Insert("chicken");
		m_AllowedModels.Insert("toilet");
	}

	static bool IsModelAllowed(string modelName)
	{
		if (!m_AllowedModels) InitAllowedModels();
		string lowerName = modelName; lowerName.ToLower();
		for (int i = 0; i < m_AllowedModels.Count(); i++)
		{
			if (lowerName.Contains(m_AllowedModels.Get(i))) return true;
		}
		return false;
	}

	static HUSHazardCategory GetCategoryByProxy(string proxyName)
	{
		if (!m_Instance) return null;
		string lowerProxy = proxyName; lowerProxy.ToLower();
		
		if (lowerProxy.Contains("toilet")) lowerProxy = "toilet_dry";
		else if (lowerProxy.Contains("kennel") || lowerProxy.Contains("dog")) lowerProxy = "dog_kennel";
		else if (lowerProxy.Contains("coop") || lowerProxy.Contains("chicken")) lowerProxy = "chicken_coop";

		for (int i = 0; i < m_Instance.Categories.Count(); i++)
		{
			string catName = m_Instance.Categories.Get(i).ProxyName;
			catName.ToLower();
			if (catName == lowerProxy) return m_Instance.Categories.Get(i);
		}
		return null;
	}

	static string GetFurnitureCategoryByVector(string houseType, vector localPos)
	{
		string type = houseType;
		type.ToLower();

		float x = localPos[0];
		float y = localPos[1];
		float z = localPos[2];

		// ============================================================================
		// СЕКТОР ЖИЛЫХ ДОМОВ (ТОЧКИ 1–20)
		// ============================================================================
		if (type.Contains("house_1w05_yellow"))
		{
			if (x >= 0.54 && x <= 0.84 && y >= -0.57 && y <= -0.27 && z >= 1.75 && z <= 2.05) return "refrigerator";
			if (x >= -1.94 && x <= -1.64 && y >= -1.17 && y <= -0.87 && z >= -2.51 && z <= -2.21) return "wardrobe";
			if (x >= -2.52 && x <= -2.22 && y >= -0.32 && y <= -0.02 && z >= -0.06 && z <= 0.23) return "bookshelf";
		}

		if (type.Contains("house_1w09"))
		{
			if (x >= -0.30 && x <= -0.00 && y >= -1.82 && y <= -1.52 && z >= 4.34 && z <= 4.64) return "refrigerator";
			if (x >= -6.76 && x <= -6.46 && y >= -2.10 && y <= -1.80 && z >= -0.54 && z <= -0.24) return "wardrobe";
			if (x >= -6.81 && x <= -6.51 && y >= -2.10 && y <= -1.80 && z >= -2.63 && z <= -2.33) return "wardrobe";
			if (x >= -0.06 && x <= 0.23 && y >= -1.09 && y <= -0.79 && z >= -4.05 && z <= -3.75) return "bookshelf";
		}

		if (type.Contains("house_1w06"))
		{
			if (x >= -2.31 && x <= -2.01 && y >= -0.34 && y <= -0.04 && z >= -2.32 && z <= -2.02) return "refrigerator";
			if (x >= 3.33 && x <= 3.63 && y >= -0.50 && y <= -0.20 && z >= 1.92 && z <= 2.22) return "wardrobe";
		}

		if (type.Contains("house_1w03"))
		{
			if (x >= -4.19 && x <= -3.89 && y >= -2.24 && y <= -1.94 && z >= -7.60 && z <= -7.30) return "wardrobe";
		}

		if (type.Contains("house_1w02"))
		{
			if (x >= -0.24 && x <= 0.05 && y >= -2.50 && y <= -2.20 && z >= 3.43 && z <= 3.73) return "wardrobe";
		}

		if (type.Contains("house_1w04"))
		{
			if (x >= -4.27 && x <= -3.97 && y >= -2.38 && y <= -2.08 && z >= -5.74 && z <= -5.44) return "wardrobe";
		}

		if (type.Contains("city_stand_grocery"))
		{
			if (x >= 2.33 && x <= 2.63 && y >= -1.83 && y <= -1.53 && z >= -1.24 && z <= -0.94) return "grocery_display";
		}

		if (type.Contains("house_2w01"))
		{
			if (x >= -3.09 && x <= -2.79 && y >= -2.82 && y <= -2.52 && z >= -1.30 && z <= -1.00) return "wardrobe";
		}

		if (type.Contains("house_1w08") && !type.Contains("brown"))
		{
			if (x >= 0.15 && x <= 0.45 && y >= -1.37 && y <= -1.07 && z >= -2.05 && z <= -1.75) return "refrigerator";
			if (x >= 1.88 && x <= 2.18 && y >= -1.30 && y <= -1.00 && z >= -0.00 && z <= 0.29) return "bookshelf";
		}

		if (type.Contains("house_1w11"))
		{
			if (x >= -4.79 && x <= -4.49 && y >= -2.14 && y <= -1.84 && z >= -3.58 && z <= -3.28) return "refrigerator";
			if (x >= 1.50 && x <= 1.80 && y >= -2.11 && y <= -1.81 && z >= -0.35 && z <= -0.05) return "wardrobe";
		}

		if (type.Contains("house_2b02"))
		{
			if (x >= 8.86 && x <= 9.16 && y >= -5.51 && y <= -5.21 && z >= -2.67 && z <= -2.37) return "refrigerator";
		}

		if (type.Contains("house_1w07"))
		{
			if (x >= -6.80 && x <= -6.50 && y >= -2.93 && y <= -2.63 && z >= 3.52 && z <= 3.82) return "wardrobe";
			if (x >= -3.86 && x <= -3.56 && y >= -2.67 && y <= -2.37 && z >= -3.61 && z <= -3.31) return "refrigerator";
			if (x >= 5.84 && x <= 6.14 && y >= -2.71 && y <= -2.41 && z >= 3.52 && z <= 3.82) return "wardrobe";
		}

		if (type.Contains("house_1b01_pub"))
		{
			if (x >= 4.98 && x <= 5.28 && y >= -2.35 && y <= -2.05 && z >= -1.49 && z <= -1.19) return "pub_bar";
			if (x >= -3.20 && x <= -2.90 && y >= -2.47 && y <= -2.17 && z >= 8.44 && z <= 8.74) return "wardrobe";
		}

		if (type.Contains("house_1w01"))
		{
			if (x >= 4.63 && x <= 4.93 && y >= -2.48 && y <= -2.18 && z >= 5.27 && z <= 5.57) return "refrigerator";
		}

		if (type.Contains("house_1w08_brown"))
		{
			if (x >= 1.21 && x <= 1.51 && y >= -1.50 && y <= -1.20 && z >= -0.30 && z <= 0.00) return "refrigerator";
			if (x >= 6.37 && x <= 6.67 && y >= -1.80 && y <= -1.50 && z >= 4.22 && z <= 4.52) return "wardrobe";
		}
		// ============================================================================
		// ЧАСТЬ 2: ГОРОДСКИЕ И СЕЛЬСКИЕ ЖИЛЫЕ ДОМА (ТОЧКИ 21–45)
		// ============================================================================
		if (type.Contains("house_1w10_brown"))
		{
			if (x >= 2.36 && x <= 2.66 && y >= -1.01 && y <= -0.71 && z >= -1.30 && z <= -1.00) return "refrigerator";
			if (x >= -2.89 && x <= -2.59 && y >= -1.17 && y <= -0.87 && z >= -4.30 && z <= -4.00) return "wardrobe";
		}

		if (type.Contains("house_2b01"))
		{
			if (x >= -2.13 && x <= -1.83 && y >= -0.85 && y <= -0.55 && z >= 4.88 && z <= 5.18) return "refrigerator";
		}

		if (type.Contains("house_2w02"))
		{
			if (x >= -1.61 && x <= -1.31 && y >= -2.37 && y <= -2.07 && z >= 3.35 && z <= 3.65) return "home_medical_cabinet";
			if (x >= 6.71 && x <= 7.01 && y >= -3.62 && y <= -3.32 && z >= -3.01 && z <= -2.71) return "refrigerator";
			if (x >= 5.29 && x <= 5.59 && y >= -3.82 && y <= -3.52 && z >= 2.25 && z <= 2.55) return "wardrobe";
		}

		if (type.Contains("houseblock_1f1"))
		{
			if (x >= 5.38 && x <= 5.68 && y >= 0.45 && y <= 0.75 && z >= 2.61 && z <= 2.91) return "wardrobe";
			if (x >= 3.62 && x <= 3.92 && y >= -4.16 && y <= -3.86 && z >= 4.63 && z <= 4.93) return "wardrobe";
		}

		if (type.Contains("houseblock_1f3"))
		{
			if (x >= 1.04 && x <= 1.34 && y >= -3.21 && y <= -2.91 && z >= -0.20 && z <= 0.09) return "kitchen_sideboard";
		}

		if (type.Contains("houseblock_1f2"))
		{
			if (x >= 2.94 && x <= 3.24 && y >= -3.96 && y <= -3.66 && z >= -1.54 && z <= -1.24) return "wardrobe";
		}

		if (type.Contains("house_2w03_brown"))
		{
			if (x >= -8.87 && x <= -8.57 && y >= -0.11 && y <= 0.18 && z >= 4.54 && z <= 4.84) return "wardrobe";
			if (x >= -1.46 && x <= -1.16 && y >= 0.84 && y <= 1.14 && z >= 1.22 && z <= 1.52) return "kitchen_sideboard";
		}

		if (type.Contains("houseblock_2f9"))
		{
			if (x >= 6.22 && x <= 6.52 && y >= -6.40 && y <= -6.10 && z >= -1.08 && z <= -0.78) return "wardrobe";
			if (x >= 0.80 && x <= 1.10 && y >= 2.24 && y <= 2.54 && z >= -0.27 && z <= 0.02) return "kitchen_sideboard";
		}

		if (type.Contains("houseblock_2f6"))
		{
			if (x >= -2.57 && x <= -2.27 && y >= -2.17 && y <= -1.87 && z >= -1.59 && z <= -1.29) return "wardrobe";
		}

		if (type.Contains("houseblock_2f_corner"))
		{
			if (x >= -4.09 && x <= -3.79 && y >= -6.02 && y <= -5.72 && z >= 8.46 && z <= 8.76) return "wardrobe";
		}

		if (type.Contains("houseblock_2f8"))
		{
			if (x >= -4.70 && x <= -4.40 && y >= -5.90 && y <= -5.60 && z >= 1.05 && z <= 1.35) return "wardrobe";
			if (x >= -8.66 && x <= -8.36 && y >= 2.74 && y <= 3.04 && z >= 0.63 && z <= 0.93) return "kitchen_sideboard";
		}

		if (type.Contains("houseblock_1f_corner"))
		{
			if (x >= 2.41 && x <= 2.71 && y >= -4.90 && y <= -4.60 && z >= -2.39 && z <= -2.09) return "kitchen_sideboard";
			if (x >= 1.28 && x <= 1.58 && y >= -0.48 && y <= -0.18 && z >= 1.04 && z <= 1.34) return "wardrobe";
		}

		if (type.Contains("city_store"))
		{
			if (x >= -10.57 && x <= -10.27 && y >= -0.27 && y <= 0.02 && z >= -5.30 && z <= -5.00) return "city_store";
			if (x >= 8.88 && x <= 9.18 && y >= -0.35 && y <= -0.05 && z >= -2.53 && z <= -2.23) return "city_store";
			if (x >= 11.74 && x <= 12.04 && y >= -0.54 && y <= -0.24 && z >= -2.53 && z <= -2.23) return "city_store";
		}

		if (type.Contains("office1"))
		{
			if (x >= 16.14 && x <= 16.44 && y >= -4.71 && y <= -4.41 && z >= -0.96 && z <= -0.66) return "office";
			if (x >= 0.93 && x <= 1.23 && y >= 0.92 && y <= 1.22 && z >= 2.90 && z <= 3.20) return "office";
			if (x >= -3.84 && x <= -3.54 && y >= 0.96 && y <= 1.26 && z >= 4.18 && z <= 4.48) return "office";
		}

		if (type.Contains("houseblock_1f4"))
		{
			if (x >= -2.10 && x <= -1.80 && y >= -3.40 && y <= -3.10 && z >= -0.40 && z <= -0.10) return "kitchen_sideboard";
		}
		// ============================================================================
		// ЧАСТЬ 3: КИОСКИ, МНОГОЭТАЖКИ, ЗАВОДСКИЕ ЦЕХА И МАСТЕРСКИЕ (ТОЧКИ 46–65)
		// ============================================================================
		if (type.Contains("city_stand_news1"))
		{
			if (x >= 2.47 && x <= 2.77 && y >= -1.83 && y <= -1.53 && z >= -1.37 && z <= -1.07) return "news_stand";
		}

		if (type.Contains("house_2w03") && !type.Contains("brown"))
		{
			if (x >= -5.22 && x <= -4.92 && y >= 0.16 && y <= 0.46 && z >= 3.04 && z <= 3.34) return "wardrobe";
			if (x >= -1.48 && x <= -1.18 && y >= 0.08 && y <= 0.38 && z >= -0.59 && z <= -0.29) return "bookshelf";
		}

		if (type.Contains("house_2w04") && !type.Contains("yellow"))
		{
			if (x >= 4.47 && x <= 4.77 && y >= -0.20 && y <= 0.09 && z >= 1.02 && z <= 1.32) return "wardrobe";
			if (x >= 3.72 && x <= 4.02 && y >= -0.25 && y <= 0.04 && z >= 3.18 && z <= 3.48) return "wardrobe";
		}

		if (type.Contains("house_2w04_yellow"))
		{
			if (x >= -0.14 && x <= 0.15 && y >= 0.20 && y <= 0.50 && z >= 2.97 && z <= 3.27) return "wardrobe";
			if (x >= 4.31 && x <= 4.61 && y >= -0.75 && y <= -0.45 && z >= 0.25 && z <= 0.55) return "wardrobe";
		}

		if (type.Contains("houseblock_2f5"))
		{
			if (x >= 1.95 && x <= 2.25 && y >= -6.60 && y <= -6.30 && z >= 2.27 && z <= 2.57) return "city_store";
		}

		if (type.Contains("houseblock_3f1"))
		{
			if (x >= 3.64 && x <= 3.94 && y >= -7.29 && y <= -6.99 && z >= -0.03 && z <= 0.26) return "wardrobe";
			if (x >= -1.36 && x <= -1.06 && y >= 3.61 && y <= 3.91 && z >= 3.31 && z <= 3.61) return "wardrobe";
			if (x >= -2.13 && x <= -1.83 && y >= 0.50 && y <= 0.80 && z >= 3.31 && z <= 3.61) return "wardrobe";
		}

		if (type.Contains("factory_lathes"))
		{
			if (x >= 16.63 && x <= 16.93 && y >= -4.75 && y <= -4.45 && z >= 5.97 && z <= 6.27) return "industrial_bookshelf";
			if (x >= 1.94 && x <= 2.24 && y >= -4.72 && y <= -4.42 && z >= 8.80 && z <= 9.10) return "industrial_bookshelf";
			if (x >= 6.85 && x <= 7.15 && y >= -4.64 && y <= -4.34 && z >= -7.67 && z <= -7.37) return "industrial_bookshelf";
			if (x >= 6.75 && x <= 7.05 && y >= -5.08 && y <= -4.78 && z >= 4.91 && z <= 5.21) return "industrial";
			if (x >= 7.81 && x <= 8.11 && y >= -5.39 && y <= -5.09 && z >= -4.54 && z <= -4.24) return "industrial";
			if (x >= 1.54 && x <= 1.84 && y >= -5.03 && y <= -4.73 && z >= -0.81 && z <= -0.51) return "industrial";
		}

		if (type.Contains("workshop4"))
		{
			if (x >= -0.27 && x <= 0.02 && y >= -0.63 && y <= -0.33 && z >= 1.74 && z <= 2.04) return "industrial";
			if (x >= -1.62 && x <= -1.32 && y >= -1.05 && y <= -0.75 && z >= -2.98 && z <= -2.68) return "industrial";
		}

		if (type.Contains("houseblock_2f7"))
		{
			if (x >= 4.15 && x <= 4.45 && y >= -6.89 && y <= -6.59 && z >= 0.82 && z <= 1.12) return "city_store";
			if (x >= 9.56 && x <= 9.86 && y >= -6.89 && y <= -6.59 && z >= 1.95 && z <= 2.25) return "city_store";
		}

		if (type.Contains("city_policestation"))
		{
			if (x >= 2.07 && x <= 2.37 && y >= -1.63 && y <= -1.33 && z >= 2.90 && z <= 3.20) return "police_weapon_rack";
			if (x >= 2.07 && x <= 2.37 && y >= -1.93 && y <= -1.63 && z >= 4.58 && z <= 4.88) return "police_weapon_rack";
			if (x >= 2.07 && x <= 2.37 && y >= -2.00 && y <= -1.70 && z >= 6.12 && z <= 6.42) return "police_weapon_rack";
			if (x >= 3.69 && x <= 3.99 && y >= -1.55 && y <= -1.25 && z >= -4.32 && z <= -4.02) return "police_office";
			if (x >= -6.43 && x <= -6.13 && y >= -1.96 && y <= -1.66 && z >= -3.11 && z <= -2.81) return "police_office";
			if (x >= -6.42 && x <= -6.12 && y >= -1.67 && y <= -1.37 && z >= 2.58 && z <= 2.88) return "police_office";
		}
		// ============================================================================
		// ЧАСТЬ 4: КИОСКИ, ГОСПИТАЛИ, ПОЖАРНЫЕ ЧАСТИ, МЭРИЯ И ПЯТИЭТАЖКИ (ТОЧКИ 66–85)
		// ============================================================================
		if (type.Contains("city_stand_news2"))
		{
			if (x >= 0.82 && x <= 1.12 && y >= -0.41 && y <= -0.11 && z >= -2.65 && z <= -2.35) return "news_stand";
		}

		if (type.Contains("mil_tent_big2_5"))
		{
			if (x >= -0.09 && x <= 0.20 && y >= -2.14 && y <= -1.84 && z >= 0.52 && z <= 0.82) return "military_tent";
		}

		if (type.Contains("mil_guardhouse1"))
		{
			if (x >= 4.77 && x <= 5.07 && y >= -0.93 && y <= -0.63 && z >= 2.67 && z <= 2.97) return "police_office";
		}

		if (type.Contains("repair_center"))
		{
			if (x >= 0.74 && x <= 1.04 && y >= -1.00 && y <= -0.70 && z >= 4.68 && z <= 4.98) return "industrial";
		}

		if (type.Contains("workshop5"))
		{
			if (x >= -0.02 && x <= 0.27 && y >= -0.36 && y <= -0.06 && z >= 5.17 && z <= 5.47) return "industrial";
			if (x >= 0.56 && x <= 0.86 && y >= -0.73 && y <= -0.43 && z >= -1.35 && z <= -1.05) return "industrial";
		}

		if (type.Contains("city_firestation"))
		{
			if (x >= 1.99 && x <= 2.29 && y >= 5.59 && y <= 5.89 && z >= 3.45 && z <= 3.75) return "fire_station_supplies";
			if (x >= -2.92 && x <= -2.62 && y >= 5.60 && y <= 5.90 && z >= 0.14 && z <= 0.44) return "fire_station_supplies";
			if (x >= -14.38 && x <= -14.08 && y >= 6.46 && y <= 6.76 && z >= 8.40 && z <= 8.70) return "fire_station_wardrobe";
			if (x >= -14.36 && x <= -14.06 && y >= 6.27 && y <= 6.57 && z >= 6.47 && z <= 6.77) return "fire_station_wardrobe";
			if (x >= -14.37 && x <= -14.07 && y >= 6.29 && y <= 6.59 && z >= 4.81 && z <= 5.11) return "fire_station_wardrobe";
			if (x >= -11.79 && x <= -11.49 && y >= 6.14 && y <= 6.44 && z >= 12.61 && z <= 12.91) return "refrigerator";
			if (x >= 5.94 && x <= 6.24 && y >= 0.61 && y <= 0.91 && z >= 7.05 && z <= 7.35) return "fire_station_wardrobe";
			if (x >= 8.15 && x <= 8.45 && y >= 0.70 && y <= 1.00 && z >= 7.05 && z <= 7.35) return "fire_station_wardrobe";
		}

		if (type == "land_guardhouse" || type.Contains("guardhouse"))
		{
			if (x >= 0.91 && x <= 1.21 && y >= -0.93 && y <= -0.63 && z >= 1.58 && z <= 1.88) return "police_office";
		}

		if (type.Contains("garage_small") && !type.Contains("row"))
		{
			if (x >= 1.30 && x <= 1.60 && y >= -0.69 && y <= -0.39 && z >= 3.61 && z <= 3.91) return "industrial";
		}

		if (type.Contains("cementworks_hall2_grey"))
		{
			if (x >= -13.61 && x <= -13.31 && y >= -4.56 && y <= -4.26 && z >= -17.87 && z <= -17.57) return "industrial";
			if (x >= -9.89 && x <= -9.59 && y >= -4.05 && y <= -3.75 && z >= -12.35 && z <= -12.05) return "industrial";
		}

		if (type.Contains("cementworks_expeditiona"))
		{
			if (x >= -13.42 && x <= -13.12 && y >= -11.91 && y <= -11.61 && z >= -8.26 && z <= -7.96) return "industrial";
			if (x >= -13.42 && x <= -13.12 && y >= -12.07 && y <= -11.77 && z >= -15.46 && z <= -15.16) return "industrial";
		}

		if (type.Contains("cementworks_hall1"))
		{
			if (x >= 0.46 && x <= 0.76 && y >= -0.51 && y <= -0.21 && z >= -19.63 && z <= -19.33) return "industrial";
		}

		if (type.Contains("house_2b03"))
		{
			if (x >= 2.13 && x <= 2.43 && y >= -4.61 && y <= -4.31 && z >= 6.15 && z <= 6.45) return "wardrobe";
			if (x >= -3.00 && x <= -2.70 && y >= -1.91 && y <= -1.61 && z >= 2.47 && z <= 2.77) return "wardrobe";
			if (x >= -3.00 && x <= -2.70 && y >= -1.31 && y <= -1.01 && z >= -3.82 && z <= -3.52) return "wardrobe";
		}

		if (type.Contains("city_hospital"))
		{
			if (x >= 8.91 && x <= 9.21 && y >= -6.30 && y <= -6.00 && z >= 0.26 && z <= 0.56) return "vending_drinks";
			if (x >= -15.62 && x <= -15.32 && y >= -6.83 && y <= -6.53 && z >= 0.26 && z <= 0.56) return "vending_drinks";
			if (x >= -4.13 && x <= -3.83 && y >= -7.50 && y <= -7.20 && z >= 8.05 && z <= 8.35) return "hospital_medical_lockbox";
			if (x >= -9.39 && x <= -9.09 && y >= -7.50 && y <= -7.20 && z >= 7.84 && z <= 8.14) return "hospital_medical_lockbox";
			if (x >= -15.68 && x <= -15.38 && y >= -7.50 && y <= -7.20 && z >= 6.07 && z <= 6.37) return "hospital_medical_lockbox";
			if (x >= -20.62 && x <= -20.32 && y >= -7.50 && y <= -7.20 && z >= 1.34 && z <= 1.64) return "hospital_medical_lockbox";
			if (x >= -20.76 && x <= -20.46 && y >= -7.50 && y <= -7.20 && z >= 5.92 && z <= 6.22) return "hospital_medical_lockbox";
			if (x >= 14.89 && x <= 15.19 && y >= -6.88 && y <= -6.58 && z >= 6.65 && z <= 6.95) return "medical_clothing";
			if (x >= 16.63 && x <= 16.93 && y >= -6.84 && y <= -6.54 && z >= 6.65 && z <= 6.95) return "medical_clothing";
		}

		if (type.Contains("tenement_small"))
		{
			if (x >= 2.18 && x <= 2.48 && y >= -6.87 && y <= -6.57 && z >= 3.76 && z <= 4.06) return "bathroom_sink";
			if (x >= -4.39 && x <= -4.09 && y >= 6.74 && y <= 7.04 && z >= -2.61 && z <= -2.31) return "bathroom_sink";
			if (x >= 2.60 && x <= 2.90 && y >= -5.79 && y <= -5.49 && z >= 3.84 && z <= 4.14) return "kitchen_sideboard";
			if (x >= 1.76 && x <= 2.06 && y >= -6.76 && y <= -6.46 && z >= -3.43 && z <= -3.13) return "wardrobe";
			if (x >= 10.46 && x <= 10.76 && y >= 2.84 && y <= 3.14 && z >= 2.16 && z <= 2.46) return "wardrobe";
			if (x >= 10.46 && x <= 10.76 && y >= 3.75 && y <= 4.05 && z >= 4.64 && z <= 4.94) return "wardrobe";
			if (x >= 1.76 && x <= 2.06 && y >= -7.49 && y <= -7.19 && z >= 5.14 && z <= 5.44) return "refrigerator";
		}

		if (type.Contains("village_healthcare"))
		{
			if (x >= 3.15 && x <= 3.45 && y >= -3.11 && y <= -2.81 && z >= 3.35 && z <= 3.65) return "village_medical_lockbox";
			if (x >= -1.63 && x <= -1.33 && y >= -2.97 && y <= -2.67 && z >= -4.84 && z <= -4.54) return "village_medical_lockbox";
			if (x >= -5.48 && x <= -5.18 && y >= -2.81 && y <= -2.51 && z >= -2.09 && z <= -1.79) return "village_medical_lockbox";
			if (x >= -9.94 && x <= -9.64 && y >= -2.95 && y <= -2.65 && z >= -2.09 && z <= -1.79) return "village_medical_lockbox";
			if (x >= -4.13 && x <= -3.83 && y >= -1.97 && y <= -1.67 && z >= 1.03 && z <= 1.33) return "medical_clothing";
			if (x >= -4.96 && x <= -4.66 && y >= -2.03 && y <= -1.73 && z >= 1.03 && z <= 1.33) return "medical_clothing";
			if (x >= -5.86 && x <= -5.56 && y >= -2.00 && y <= -1.70 && z >= 1.03 && z <= 1.33) return "medical_clothing";
			if (x >= -3.76 && x <= -3.46 && y >= -1.98 && y <= -1.68 && z >= 3.85 && z <= 4.15) return "medical_supplies";
			if (x >= -1.27 && x <= -0.97 && y >= -1.93 && y <= -1.63 && z >= 3.72 && z <= 4.02) return "medical_supplies";
			if (x >= -1.27 && x <= -0.97 && y >= -1.97 && y <= -1.67 && z >= 2.14 && z <= 2.44) return "medical_supplies";
			if (x >= -3.76 && x <= -3.46 && y >= -2.02 && y <= -1.72 && z >= 2.21 && z <= 2.51) return "medical_supplies";
		}

		if (type.Contains("office_municipal2"))
		{
			if (x >= -1.85 && x <= -1.55 && y >= -7.28 && y <= -6.98 && z >= -4.05 && z <= -3.75) return "office";
			if (x >= -7.03 && x <= -6.73 && y >= -6.88 && y <= -6.58 && z >= 1.56 && z <= 1.86) return "office";
			if (x >= -1.44 && x <= -1.34 && y >= -7.20 && y <= -6.90 && z >= -5.70 && z <= -5.40) return "office";
			if (x >= 7.81 && x <= 8.11 && y >= -6.79 && y <= -6.49 && z >= 1.43 && z <= 1.73) return "office";
			if (x >= 1.81 && x <= 2.11 && y >= -6.72 && y <= -6.42 && z >= 6.22 && z <= 6.52) return "office";
		}
		// ============================================================================
		// ЧАСТЬ 5: МЭРИЯ (ОКОНЧАНИЕ), ВОЕННЫЕ БАЗЫ, АЭРОДРОМЫ И ЛАГЕРЯ (ТОЧКИ 86–100)
		// ============================================================================
		if (type.Contains("office_municipal2"))
		{
			if (x >= -8.11 && x <= -7.81 && y >= -3.26 && y <= -2.96 && z >= 4.21 && z <= 4.51) return "office";
			if (x >= -2.05 && x <= -1.75 && y >= -3.00 && y <= -2.70 && z >= 6.22 && z <= 6.52) return "office";
			if (x >= 4.12 && x <= 4.42 && y >= -3.14 && y <= -2.84 && z >= 7.98 && z <= 8.28) return "office";
			if (x >= 7.81 && x <= 8.11 && y >= -3.25 && y <= -2.95 && z >= 1.43 && z <= 1.73) return "office";
			if (x >= -6.69 && x <= -6.39 && y >= -3.57 && y <= -3.27 && z >= 1.34 && z <= 1.64) return "office";
			if (x >= -0.16 && x <= 0.13 && y >= -3.50 && y <= -3.20 && z >= -2.96 && z <= -2.66) return "office";
		}

		if (type.Contains("mil_barracks5"))
		{
			if (x >= -13.97 && x <= -13.67 && y >= -5.15 && y <= -4.85 && z >= -4.86 && z <= -4.56) return "military_weapon_rack";
			if (x >= -14.16 && x <= -13.86 && y >= -0.00 && y <= 0.29 && z >= -4.69 && z <= -4.39) return "military_weapon_rack";
			if (x >= -9.90 && x <= -9.60 && y >= -4.21 && y <= -3.91 && z >= -4.70 && z <= -4.40) return "military_bookshelf";
			if (x >= -7.90 && x <= -7.60 && y >= -5.12 && y <= -4.82 && z >= -7.46 && z <= -7.16) return "military";
			if (x >= -7.90 && x <= -7.60 && y >= 0.07 && y <= 0.37 && z >= -8.89 && z <= -8.59) return "military";
			if (x >= -7.90 && x <= -7.60 && y >= -0.15 && y <= 0.14 && z >= -6.52 && z <= -6.22) return "military";
		}

		if (type.Contains("mil_barracks1"))
		{
			if (x >= 3.57 && x <= 3.87 && y >= -1.10 && y <= -0.80 && z >= -4.52 && z <= -4.22) return "military";
			if (x >= 6.26 && x <= 6.56 && y >= -1.09 && y <= -0.79 && z >= 4.24 && z <= 4.54) return "military";
			if (x >= 3.24 && x <= 3.54 && y >= -1.12 && y <= -0.82 && z >= 4.24 && z <= 4.54) return "military";
			if (x >= 0.78 && x <= 1.08 && y >= -1.09 && y <= -0.79 && z >= 4.24 && z <= 4.54) return "military";
			if (x >= -1.52 && x <= -1.22 && y >= -1.02 && y <= -0.72 && z >= 4.24 && z <= 4.54) return "military";
			if (x >= -4.24 && x <= -3.94 && y >= -1.26 && y <= -0.96 && z >= 4.24 && z <= 4.54) return "military";
			if (x >= -6.57 && x <= -6.27 && y >= -1.19 && y <= -0.89 && z >= 4.24 && z <= 4.54) return "military";
			if (x >= -7.14 && x <= -6.84 && y >= -0.97 && y <= -0.67 && z >= -4.52 && z <= -4.22) return "military";
			if (x >= -4.74 && x <= -4.44 && y >= -1.18 && y <= -0.88 && z >= -4.52 && z <= -4.22) return "military";
			if (x >= -2.13 && x <= -1.83 && y >= -1.20 && y <= -0.90 && z >= -4.37 && z <= -4.07) return "military";
			if (x >= 0.55 && x <= 0.85 && y >= -1.11 && y <= -0.81 && z >= -4.52 && z <= -4.22) return "military";
			if (x >= 3.78 && x <= 4.08 && y >= -1.03 && y <= -0.73 && z >= -4.52 && z <= -4.22) return "military";
		}

		if (type.Contains("mil_barracks2"))
		{
			if (x >= 3.36 && x <= 3.66 && y >= -0.35 && y <= -0.05 && z >= 0.27 && z <= 0.57) return "military_weapon_rack";
			if (x >= -2.74 && x <= -2.44 && y >= -0.53 && y <= -0.23 && z >= 0.59 && z <= 0.89) return "military";
			if (x >= -9.41 && x <= -9.11 && y >= -0.26 && y <= 0.03 && z >= 0.57 && z <= 0.87) return "military";
		}

		if (type.Contains("garage_row_big"))
		{
			if (x >= 5.90 && x <= 6.20 && y >= -2.33 && y <= -2.03 && z >= 0.39 && z <= 0.69) return "industrial";
			if (x >= 0.35 && x <= 0.65 && y >= -2.31 && y <= -2.01 && z >= -0.13 && z <= 0.16) return "industrial";
			if (x >= -6.05 && x <= -5.75 && y >= -2.30 && y <= -2.00 && z >= -5.60 && z <= -5.30) return "industrial";
			if (x >= -12.72 && x <= -12.42 && y >= -2.41 && y <= -2.11 && z >= -1.21 && z <= -0.91) return "industrial_wardrobe";
		}

		if (type.Contains("garage_office"))
		{
			if (x >= -3.30 && x <= -3.00 && y >= -0.11 && y <= 0.18 && z >= 3.05 && z <= 3.35) return "industrial_wardrobe";
			if (x >= -3.31 && x <= -3.01 && y >= -0.14 && y <= 0.15 && z >= -0.40 && z <= -0.10) return "industrial_wardrobe";
			if (x >= 2.31 && x <= 2.61 && y >= -0.23 && y <= 0.06 && z >= -0.70 && z <= -0.40) return "industrial_wardrobe";
		}

		if (type.Contains("tisy_hq"))
		{
			if (x >= 9.34 && x <= 9.64 && y >= -2.95 && y <= -2.65 && z >= -3.13 && z <= -2.83) return "military_weapon_rack";
			if (x >= 9.35 && x <= 9.65 && y >= 0.97 && y <= 1.27 && z >= -3.03 && z <= -2.73) return "military_weapon_rack";
			if (x >= 9.96 && x <= 10.26 && y >= 0.97 && y <= 1.27 && z >= 3.81 && z <= 4.11) return "military_weapon_rack";
			if (x >= -6.78 && x <= -6.48 && y >= 1.17 && y <= 1.47 && z >= 4.27 && z <= 4.57) return "military_weapon_rack";
			if (x >= 2.82 && x <= 3.12 && y >= -7.09 && y <= -6.79 && z >= 4.69 && z <= 4.99) return "military_weapon_rack";
			if (x >= -2.38 && x <= -2.08 && y >= -2.11 && y <= -1.81 && z >= -9.11 && z <= -8.81) return "military_weapon_rack";
			if (x >= 8.20 && x <= 8.50 && y >= 1.88 && y <= 2.18 && z >= -6.97 && z <= -6.67) return "military_weapon_rack";
		}

		if (type.Contains("mil_barracks6_old"))
		{
			if (x >= 1.50 && x <= 1.80 && y >= 4.70 && y <= 5.00 && z >= 2.97 && z <= 3.27) return "military_weapon_rack";
			if (x >= 10.59 && x <= 10.89 && y >= 4.62 && y <= 4.92 && z >= 4.50 && z <= 4.80) return "military_weapon_rack";
			if (x >= 14.50 && x <= 14.80 && y >= 4.71 && y <= 5.01 && z >= -2.01 && z <= -1.71) return "military_weapon_rack";
		}

		if (type.Contains("mil_aircraftshelter"))
		{
			if (x >= 9.32 && x <= 9.62 && y >= 0.15 && y <= 0.45 && z >= -12.85 && z <= -12.55) return "military";
		}

		if (type.Contains("mil_atc_big"))
		{
			if (x >= -0.01 && x <= 0.28 && y >= -14.39 && y <= -14.09 && z >= -13.85 && z <= -13.55) return "vending_drinks";
			if (x >= 7.13 && x <= 7.43 && y >= -4.78 && y <= -4.48 && z >= -14.29 && z <= -13.99) return "military";
			if (x >= 12.98 && x <= 13.28 && y >= -4.89 && y <= -4.59 && z >= -19.37 && z <= -19.07) return "military";
			if (x >= 11.58 && x <= 11.88 && y >= -0.22 && y <= 0.07 && z >= -20.00 && z <= -19.70) return "military";
			if (x >= 12.98 && x <= 13.28 && y >= -0.09 && y <= 0.20 && z >= -13.07 && z <= -12.77) return "military";
			if (x >= 8.38 && x <= 8.68 && y >= -1.13 && y <= 0.16 && z >= -6.95 && z <= -6.65) return "military_weapon_rack";
			if (x >= 9.71 && x <= 10.01 && y >= -1.12 && y <= 0.17 && z >= -6.95 && z <= -6.65) return "military_weapon_rack";
			if (x >= 12.98 && x <= 13.28 && y >= -1.11 && y <= 0.18 && z >= -7.79 && z <= -7.49) return "military_weapon_rack";
		}

		if (type.Contains("mil_tent_big4"))
		{
			if (x >= 1.00 && x <= 1.30 && y >= -0.95 && y <= -0.65 && z >= -6.08 && z <= -5.78) return "military_tent";
			if (x >= 3.46 && x <= 3.76 && y >= -1.31 && y <= -1.01 && z >= -0.05 && z <= 0.24) return "military_tent";
		}

		if (type.Contains("mil_tent_big3"))
		{
			if (x >= 4.50 && x <= 4.80 && y >= -1.18 && y <= -0.88 && z >= 1.24 && z <= 1.54) return "military_tent";
		}

		if (type.Contains("fortified_nest_watchtower"))
		{
			if (x >= -1.10 && x <= -0.80 && y >= 1.34 && y <= 1.64 && z >= -3.53 && z <= -3.23) return "military_tent";
		}

		if (type.Contains("tisy_barracks"))
		{
			if (x >= 16.79 && x <= 17.09 && y >= -3.20 && y <= -2.90 && z >= 3.08 && z <= 3.38) return "military";
			if (x >= 11.50 && x <= 11.80 && y >= 1.20 && y <= 1.50 && z >= 3.17 && z <= 3.47) return "military";
		}

		if (type.Contains("mil_barracks_round"))
		{
			if (x >= 2.66 && x <= 2.96 && y >= -0.84 && y <= -0.54 && z >= -0.57 && z <= -0.27) return "military_ammo_box";
			if (x >= -2.83 && x <= -2.53 && y >= -0.84 && y <= -0.54 && z >= -0.16 && z <= 0.13) return "military_ammo_box";
			if (x >= 2.60 && x <= 2.90 && y >= -0.84 && y <= -0.54 && z >= 1.34 && z <= 1.64) return "military_ammo_box";
		}

		if (type.Contains("tisy_radarb_base"))
		{
			if (x >= -4.43 && x <= -4.13 && y >= 5.50 && y <= 5.80 && z >= -13.00 && z <= -12.70) return "military";
			if (x >= -16.65 && x <= -16.35 && y >= 5.14 && y <= 5.44 && z >= 0.18 && z <= 0.48) return "military_refrigerator";
		}

		if (type.Contains("fuelstation_build"))
		{
			if (x >= -1.16 && x <= -0.86 && y >= -1.73 && y <= -1.43 && z >= -0.47 && z <= -0.47) return "fuel_station_display";
		}

		if (type.Contains("village_pub"))
		{
			if (x >= 1.32 && x <= 1.62 && y >= -3.64 && y <= -3.34 && z >= -3.10 && z <= -2.80) return "refrigerator";
			if (x >= -4.59 && x <= -4.29 && y >= -0.27 && y <= 0.03 && z >= -3.33 && z <= -3.03) return "wardrobe";
			if (x >= -0.78 && x <= -0.48 && y >= -2.85 && y <= -2.55 && z >= 2.64 && z <= 2.94) return "village_tools";
			if (x >= 0.43 && x <= 0.73 && y >= -3.12 && y <= -2.82 && z >= 4.40 && z <= 4.70) return "village_tools";
			if (x >= 1.54 && x <= 1.84 && y >= -3.09 && y <= -2.79 && z >= 4.40 && z <= 4.70) return "village_tools";
		}

		if (type.Contains("airfield_small_hangar"))
		{
			if (x >= -13.57 && x <= -13.27 && y >= -2.39 && y <= -2.09 && z >= 14.11 && z <= 14.41) return "military";
		}

		if (type.Contains("airfield_small_control"))
		{
			if (x >= -11.06 && x <= -10.76 && y >= -8.76 && y <= -8.46 && z >= 7.86 && z <= 8.16) return "military";
			if (x >= -6.66 && x <= -6.36 && y >= -9.09 && y <= -8.79 && z >= 5.76 && z <= 6.06) return "military";
			if (x >= -10.96 && x <= -10.66 && y >= -9.44 && y <= -9.14 && z >= -2.53 && z <= -2.23) return "military_refrigerator";
		}

		return string.Empty;
	}
};