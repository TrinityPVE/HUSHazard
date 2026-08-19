// ============================================================================
// HUSHazard - Player RPC & Damage Core (Patched for DayZ 1.29)
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

				// КЛИЕНТСКАЯ СТОРОНА: Синхронизируем локальные часовые кулдауны мебели и улиц
				if (GetGame().IsClient())
				{
					int endTime = GetGame().GetTime() + 3600000; 
					
					if (!ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
					{
						ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns = new map<string, int>;
					}
					ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(rpcMarker, endTime);
					return;
				}

				// СЕРВЕРНАЯ СТОРОНА: Проверяем перчатки и наказываем игрока БЕЗОПАСНЫМИ методами!
				if (GetGame().IsServer() && this.IsAlive())
				{
					if (!this.GetInventory()) return;
					
					EntityAI gloves = this.GetInventory().FindAttachment(InventorySlots.GLOVES);
					
					// Если перчаток нет или они порваны в ноль
					if (!gloves || gloves.IsRuined())
					{
						this.SetBloodyHands(true);

						// Специфика обыска ржавого багажника машины
						if (rpcMarker == "wreck_trunk")
						{
							if (this.GetBleedingManagerServer())
							{
								this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightHand");
								this.MessageAction("[HUSHazard]: Вы глубоко распороли ладонь об острый край ржавого багажника!");
							}

							if (Math.RandomFloat01() < 0.35)
							{
								// СВЕРЕНО С ВАШИМ REPO: Заносим агентов по имени eAgents.WOUND_AGENT
								this.InsertAgent(eAgents.WOUND_AGENT, 100);
								
								// ВЕРИФИЦИРОВАНО ПО ВАШЕМУ REPO (Init): Индекс 31 — это класс WoundInfectStage1Mdfr
								if (this.GetModifiersManager() && !this.GetModifiersManager().IsModifierActive(31))
								{
									this.GetModifiersManager().ActivateModifier(31);
								}
								this.MessageAction("[HUSHazard]: Ржавчина попала в кровь. Рана начинает стремительно портиться и гноиться!");
							}
						}
						// Специфика обыска уличных точек (курятники, туалеты, будки)
						else
						{
							if (this.GetBleedingManagerServer())
							{
								this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftHand");
								this.MessageAction("[HUSHazard]: Ай! Вы сильно порезали незащищенную руку о занозу или ржавый гвоздь!");
							}

							if (Math.RandomFloat01() < 0.45)
							{
								// СВЕРЕНО С ВАШИМ REPO: Заносим агентов по имени eAgents.WOUND_AGENT
								this.InsertAgent(eAgents.WOUND_AGENT, 100);
								
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

		if (GetGame().IsServer() && source && source.IsInherited(ZombieBase) && this.IsAlive())
		{
			float halfMaxHealth = this.GetMaxHealth("", "Health") * 0.1;
			this.DecreaseHealth("", "Health", halfMaxHealth, false);

			if (this.GetBleedingManagerServer())
			{
				this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(dmgZone);
			}

			if (Math.RandomFloat01() < 0.40)
			{
				this.InsertAgent(eAgents.BRAIN, 1000);
				
				// ВЕРИФИЦИРОВАНО ПО ВАШЕМУ REPO (Init): Индекс 24 — это класс BrainDiseaseMdfr (Куру)
				if (this.GetModifiersManager() && !this.GetModifiersManager().IsModifierActive(24))
				{
					this.GetModifiersManager().ActivateModifier(24); 
				}
				
				this.MessageAction("[HUSHazard]: Трупный яд зараженного попал в кровь! Вы получили инфицированную рану.");
			}
		}
	}
}
