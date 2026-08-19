// ============================================================================
// HUSHazard - Hard Universal Search & Hazard Mod (Patched for DayZ 1.29)
// World Player Module - Monolithic 1.29 Verified RPC Sync & Punishment
// ============================================================================

modded class PlayerBase
{
	// Нативный С++ шлюз DayZ для приёма сетевых RPC-пакетов
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == 95202)
		{
			ref Param1<string> rpcParam = new Param1<string>("");
			if (ctx.Read(rpcParam))
			{
				string rpcMarker = rpcParam.param1;

				// ----------------============================================================
				// КЛИЕНТСКАЯ СТОРОНА: Синхронизируем локальные часовые кулдауны мебели и улиц
				// ----------------============================================================
				if (GetGame().IsClient())
				{
					int endTime = GetGame().GetTime() + 3600000; 
					
					if (!ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
					{
						ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns = new map<string, int>();
					}
					ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(rpcMarker, endTime);
					return;
				}

				// ----------------============================================================
				// СЕРВЕРНАЯ СТОРОНА: Проверяем перчатки и наказываем игрока БЕЗОПАСНЫМИ методами!
				// ----------------============================================================
				if (GetGame().IsServer() && this.IsAlive())
				{
					if (!this.GetInventory()) return;
					
					EntityAI gloves = this.GetInventory().FindAttachment(InventorySlots.GLOVES);
					
					// Если перчаток нет или они порваны в ноль
					if (!gloves || gloves.IsRuined())
					{
						// 1. Пачкаем руки гарантированным ванильным методом DayZ 1.29
						this.SetBloodyHands(true);

						// 2. Специфика обыска ржавого багажника машины
						if (rpcMarker == "wreck_trunk")
						{
							if (this.GetBleedingManagerServer())
							{
								// Накладываем честный порез на правую руку через С++ ядро персонажа строго на сервере
								this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightHand");
								this.MessageAction("[HUSHazard]: Вы глубоко распороли ладонь об острый край ржавого багажника!");
							}

							if (Math.RandomFloat01() < 0.35)
							{
								// ИСПРАВЛЕНО ДЛЯ 1.29: Индекс 32 — это прямой C++ ID агента Wound Infection в коде DayZ!
								this.InsertAgent(32, 100);
								
								// ВЕРИФИЦИРОВАНО ПО ВАШЕМУ REPO: Индекс 31 — это WoundInfectStage1Mdfr
								if (this.GetModifiersManager() && !this.GetModifiersManager().IsModifierActive(31))
								{
									this.GetModifiersManager().ActivateModifier(31);
								}
								this.MessageAction("[HUSHazard]: Ржавчина попала в кровь. Рана начинает стремительно портиться и гноиться!");
							}
						}
						// 3. Специфика обыска уличных точек (курятники, туалеты, будки)
						else
						{
							if (this.GetBleedingManagerServer())
							{
								// Накладываем честный порез на левую руку через С++ ядро персонажа строго на сервере
								this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftHand");
								this.MessageAction("[HUSHazard]: Ай! Вы сильно порезали незащищенную руку о занозу или ржавый гвоздь!");
							}

							if (Math.RandomFloat01() < 0.45)
							{
								// ИСПРАВЛЕНО ДЛЯ 1.29: Индекс 32 — это прямой C++ ID агента Wound Infection в коде DayZ!
								this.InsertAgent(32, 100);
								
								if (this.GetModifiersManager() && !this.GetModifiersManager().IsModifierActive(31))
								{
									this.GetModifiersManager().ActivateModifier(31);
								}
								this.MessageAction("[HUSHazard]: В открытую рану попала грязь... Кажется, началось заражение крови!");
							}

							// Отдельный шлюз туалетов: Рвота от вони без маски
							if (rpcMarker == "toilet_dry")
							{
								EntityAI mask = this.GetInventory().FindAttachment(InventorySlots.MASK);
								if (!mask || mask.IsRuined())
								{
									if (this.GetSymptomManager())
									{
										SymptomBase vomit = this.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
										if (vomit)
										{
											this.MessageAction("[HUSHazard]: Кхэ! Ужасный трупный запах туалета выворачивает ваш желудок наизнанку!");
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Тотальное наказание игрока при любом попадании зомби (СВЕРЕНО С ВАНИЛЬНЫМ API 1.29)
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		// Если урон нанес именно зомби, и мы на сервере над живым игроком
		if (GetGame().IsServer() && source && source.IsInherited(ZombieBase) && this.IsAlive())
		{
			// 1. Зомби мгновенно сносит ровно 10% от максимального ХП выжившего
			float halfMaxHealth = this.GetMaxHealth("", "Health") * 0.1;
			this.DecreaseHealth("", "Health", halfMaxHealth, false);

			// 2. Гарантированный порез кости/зоны, куда физически прилетел удар лапы зомби
			if (this.GetBleedingManagerServer())
			{
				this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(dmgZone);
			}

			// 3. Бросаем честный кубик на 40%-й шанс заразиться безумием Куру при ударе зараженного
			if (Math.RandomFloat01() < 0.40)
			{
				// Намертво заливаем 1000 единиц мозговых агентов (Brain Agent) в кровь через жесткий индекс eAgents.BRAIN
				this.InsertAgent(eAgents.BRAIN, 1000);
				
				// ВЕРИФИЦИРОВАНО ПО ВАШЕМУ REPO: Индекс 24 — это класс BrainDiseaseMdfr (Куру)
				if (this.GetModifiersManager() && !this.GetModifiersManager().IsModifierActive(24))
				{
					this.GetModifiersManager().ActivateModifier(24); 
				}
				
				this.MessageAction("[HUSHazard]: Трупный яд зараженного попал в кровь! Вы получили инфицированную рану.");
			}
		}
	}
}
