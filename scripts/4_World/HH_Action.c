// ============================================================================
// HUSHazard - Hard Universal Search & Hazard Mod (Patched for DayZ 1.29)
// World Action Module - РЕЛЬСА №1: ОБЫСК УЛИЧНОЙ СТАТИКИ С УЧЕТОМ КУЛДАУНОВ
// ============================================================================

class ActionSearchHazardCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(4.0);
	}
};

class ActionSearchHazard : ActionContinuousBase
{
	void ActionSearchHazard()
	{
		m_CallbackClass = ActionSearchHazardCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; // ТИХАЯ СТОЯЧАЯ АНИМАЦИЯ
		m_FullBody = true;
		m_Text = "Обыскать";
		m_LockTargetOnUse = false; 
	}

	override bool CanBeUsedOnBack() { return false; }
	override bool IsLockTargetOnUse() { return false; }
	override typename GetInputType() { return ContinuousInteractActionInput; }
	override bool HasTarget() { return true; }

	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINone();
	}

	// ФИКС ХАНДЛИНГА: Этот флаг убирает софтлок и С++ баг "думающей" шкалы
	override int GetActionCategory() 
	{ 
		return AC_INTERACT; 
	}

	string ResolveTargetName(ActionTarget target)
	{
		Object targetObj = target.GetObject();
		if (!targetObj) targetObj = target.GetParent();
		
		// Если это статический объект, намертво зашитый в ландшафт карты
		if (!targetObj)
		{
			int componentIndex = target.GetComponentIndex();
			if (componentIndex != -1)
			{
				targetObj = target.GetObject();
			}
		}

		if (!targetObj) return string.Empty;

		string typeName = targetObj.GetType();
		if (typeName == string.Empty || typeName.Contains("chernarus") || typeName.Contains("sakhal") || typeName.Contains("livonia"))
		{
			typeName = targetObj.GetModelName();
		}
		
		if (typeName != string.Empty) 
		{
			typeName.ToLower();
		}
		
		return typeName;
	}

	// ИСПРАВЛЕНО: Добавлена проверка часового кулдауна уличных объектов на клиенте и сервере
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		HumanMovementState movementState = new HumanMovementState();
		player.GetMovementState(movementState);
		if (movementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE) return false;

		Object targetObj = target.GetObject();
		if (!targetObj) targetObj = target.GetParent();
		if (targetObj)
		{
			string uniqueCooldownKey = targetObj.GetID().ToString() + "_street";
			if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey)) return false;
			}
		}

		string targetName = ResolveTargetName(target);
		if (targetName == string.Empty) return false;
		targetName.ToLower();

		return HUSHazardConfigHolder.IsModelAllowed(targetName);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
	}

	// ИСПРАВЛЕНО: Кулдаун записывается строго ПОСЛЕ выдачи лута, чтобы не блокировать самый первый обыск!
	// ИСПРАВЛЕНО: Износ перчаток перенесен на самый верх финала, как это сделано в капоте машины!
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		
		if (!action_data || !action_data.m_Player || !action_data.m_Target) return;

		Object targetObj = action_data.m_Target.GetObject();
		if (!targetObj) targetObj = action_data.m_Target.GetParent();
		if (!targetObj) return;

		// ============================================================================
		// ЧЕСТНЫЙ ИЗНОС ПЕРЧАТОК ПРИ УСПЕШНОМ ОБЫСКЕ УЛИЧНОЙ СТАТИКИ
		// ============================================================================
		EntityAI gloves = action_data.m_Player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (gloves && !gloves.IsRuined())
		{
			// Изнашиваем перчатки на 6 единиц за успешное ковыряние в курятнике/будке
			gloves.DecreaseHealth("", "", 6.0);
			Print("[HUSHazard_Debug]: Обыск завершен! Перчатки уличного экшена успешно изношены на 6 единиц.");
		}

		string typeName = ResolveTargetName(action_data.m_Target);
		if (typeName == string.Empty) return;
		typeName.ToLower();

		// Стерильный маппинг строк для серверного шлюза экономики
		string targetCategory = typeName;
		if (targetCategory.Contains("toilet"))       targetCategory = "toilet_dry";
		else if (targetCategory.Contains("kennel") || targetCategory.Contains("dog")) targetCategory = "dog_kennel";
		else if (targetCategory.Contains("coop")   || targetCategory.Contains("chicken")) targetCategory = "chicken_coop";

		// ШАГ 1: Сначала выдаем честный лут и бросаем кубик шанса!
		HUSHazardServerManager.ProcessSearch(action_data.m_Player, targetObj, targetCategory);

		// ШАГ 2: Только ПОСЛЕ выдачи лута вешаем на уличный объект часовой кулдаун
		string uniqueCooldownKey = targetObj.GetID().ToString() + "_street";
		ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);

		// Сетевой RPC-импульс на клиент для мгновенной синхронизации во фреймворке игрока
		ref Param1<string> rpcKeyParam = new Param1<string>(uniqueCooldownKey);
		GetGame().RPCSingleParam(action_data.m_Player, 95202, rpcKeyParam, true, action_data.m_Player.GetIdentity());
	}
};

// ============================================================================
// ЧАСТЬ 2: СНАЙПЕРСКИЙ ОБЫСК МЕБЕЛИ ВНУТРИ ДОМОВ (ГЛОБАЛЬНЫЙ КУЛДАУН ПО ID)
// ============================================================================
class ActionSearchFurnitureCB : ActionContinuousBaseCB
{
	protected EffectSound m_SearchSoundLoop;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(4.0);
	}

	override void InitActionComponent()
	{
		super.InitActionComponent();
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			m_SearchSoundLoop = SEffectManager.PlaySoundOnObject("HH_Zombie_Search_SoundSet", m_ActionData.m_Player);
		}
	}

	override void OnFinish(bool pCanceled)
	{
		super.OnFinish(pCanceled);
		if (m_SearchSoundLoop) 
		{
			m_SearchSoundLoop.SoundStop();
		}
	}
};

class ActionSearchFurniture : ActionContinuousBase
{
	static ref map<string, int> m_HH_GlobalFurnitureCooldowns = new map<string, int>();
	const int FURNITURE_COOLDOWN_TIME = 3600;

	void ActionSearchFurniture()
	{
		m_CallbackClass = ActionSearchFurnitureCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; // ТИХАЯ СТОЯЧАЯ АНИМАЦИЯ
		m_FullBody = true;
		m_Text = "Обыскать"; 
		m_LockTargetOnUse = false; 
	}
	
	override int GetActionCategory() { return AC_INTERACT; }


	override bool CanBeUsedOnBack() { return false; }
	override bool IsLockTargetOnUse() { return false; }
	override typename GetInputType() { return ContinuousInteractActionInput; }
	override bool HasTarget() { return true; }

	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINone();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		HumanMovementState movementState = new HumanMovementState();
		player.GetMovementState(movementState);
		if (movementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE) return false;

		Object targetObj = target.GetObject();
		if (!targetObj) targetObj = target.GetParent();
		if (!targetObj) return false;

		string typeName = targetObj.GetType();
		typeName.ToLower();
		if (typeName.Contains("zmb") || typeName.Contains("corpse")) return false;
		if (typeName.Contains("wreck") || typeName.Contains("volha") || typeName.Contains("offroad")) return false;

		if (targetObj.IsInherited(House) || targetObj.IsInherited(Building))
		{
			int compIdx = target.GetComponentIndex();
			if (compIdx != -1)
			{
				string uniqueCooldownKey = targetObj.GetID().ToString() + "_" + compIdx.ToString();
				int currentTime = GetGame().GetTime();

				if (GetGame().IsServer() && m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
				{
					int cooldownEndTime = m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey);
					if (currentTime < cooldownEndTime) return false;
					else m_HH_GlobalFurnitureCooldowns.Remove(uniqueCooldownKey);
				}

				vector worldHitPos = target.GetCursorHitPos();
				if (worldHitPos != vector.Zero)
				{
					vector localHitPos = targetObj.WorldToModel(worldHitPos);
					string matrixCategory = HUSHazardConfigHolder.GetFurnitureCategoryByVector(targetObj.GetType(), localHitPos);
					if (matrixCategory != string.Empty) return true; 
				}
			}
		}
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PlayerBase player = action_data.m_Player;
		Object targetObj = action_data.m_Target.GetObject();
		if (!targetObj) targetObj = action_data.m_Target.GetParent();
		if (!player || !targetObj) return;

		int compIdx = action_data.m_Target.GetComponentIndex();
		vector worldHitPos = action_data.m_Target.GetCursorHitPos();
		if (worldHitPos == vector.Zero || compIdx == -1) return;

		string uniqueCooldownKey = targetObj.GetID().ToString() + "_" + compIdx.ToString();

		if (m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
		{
			int checkEndTime = m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey);
			if (GetGame().GetTime() < checkEndTime) return; 
		}

		int endTime = GetGame().GetTime() + (FURNITURE_COOLDOWN_TIME * 1000);
		m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, endTime);

		vector localHitPos = targetObj.WorldToModel(worldHitPos);
		string matrixCategory = HUSHazardConfigHolder.GetFurnitureCategoryByVector(targetObj.GetType(), localHitPos);
		
		if (matrixCategory != string.Empty)
		{
			// ИСПРАВЛЕНО: Передаем имя найденной в HH_Config категории строкой напрямую в новый менеджер лута
			HUSHazardServerManager.ProcessSearch(player, targetObj, matrixCategory);
		}
	}
};
// ============================================================================
// ЧАСТЬ 3: АВТОМОБИЛЬНЫЙ СЕКТОР (БAГAЖНИКИ И КАПОТЫ ПО ОСИ Z) — ФИНАЛЬНЫЙ ЭТАЛОН
// ============================================================================

class ActionSearchTrunkWreckCB : ActionContinuousBaseCB
{
	protected EffectSound m_SearchSoundLoop;

	override void CreateActionComponent() 
	{ 
		m_ActionData.m_ActionComponent = new CAContinuousTime(4.0); 
	}
	
	override void InitActionComponent()
	{
		super.InitActionComponent();
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			// ВОЗВРАЩЕНО К ОРИГИНАЛУ: Звук багажника проиграется ровно ОДИН раз (one-shot), без зацикливания!
			m_SearchSoundLoop = SEffectManager.PlaySoundOnObject("HH_Zombie_Search_SoundSet", m_ActionData.m_Player);
		}
	}
	
	override void OnFinish(bool pCanceled)
	{
		super.OnFinish(pCanceled);
		if (m_SearchSoundLoop) m_SearchSoundLoop.SoundStop();
	}
};

class ActionSearchTrunkWreck : ActionContinuousBase
{
	void ActionSearchTrunkWreck()
	{
		m_CallbackClass = ActionSearchTrunkWreckCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; 
		m_FullBody = true;
		m_Text = "Обыскать багажник"; 
		m_LockTargetOnUse = false;
	}
	
	override typename GetInputType() { return ContinuousInteractActionInput; }
	override void CreateConditionComponents() { m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT); m_ConditionItem = new CCINone(); }

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObj = target.GetObject();
		if (!targetObj) targetObj = target.GetParent();
		if (!targetObj) return false;
		
		// ЗАЩИТА ТРАНСПОРТА: Если это живой рабочий автомобиль игрока — полностью блокируем экшен!
		if (targetObj.IsKindOf("CarScript") || targetObj.IsKindOf("Car")) return false;

		string typeName = targetObj.GetType(); typeName.ToLower();
		if (typeName.Contains("wreck") || typeName.Contains("volha") || typeName.Contains("offroad"))
		{
			string uniqueCooldownKey = targetObj.GetID().ToString() + "_trunk";
			if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey)) return false;
			}
			vector modelPos = targetObj.WorldToModel(player.GetPosition());
			if (modelPos[2] >= 0.0) return true; // Ось Z на месте!
		}
		return false;
	}

	// ИСПРАВЛЕНО: Ключевое слово override на месте, износ перчаток успешно интегрирован!
	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Target || !action_data.m_Target.GetObject()) return;
		
		Object targetObj = action_data.m_Target.GetObject();
		string uniqueCooldownKey = targetObj.GetID().ToString() + "_trunk";

		// ============================================================================
		// ХАРДКОРНЫЙ ИЗНОС ПЕРЧАТОК ДЛЯ БАГАЖНИКА МАШИНЫ
		// ============================================================================
		EntityAI gloves = action_data.m_Player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (gloves && !gloves.IsRuined())
		{
			// Изнашиваем перчатки на 6 единиц за обыск грязного ржавого багажника
			gloves.DecreaseHealth("", "", 6.0);
			Print("[HUSHazard_Gloves]: Багажник автомобиля обыскан! Перчатки изношены на 6 единиц.");
		}

		// Сначала честно бросаем кубик шанса и спавним лут
		HUSHazardServerManager.ProcessSearch(action_data.m_Player, targetObj, "wreck_trunk");

		// Только после этого вешаем на этот багажник часовой замок
		ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
	}
};


class ActionSearchEngineWreckCB : ActionContinuousBaseCB
{
	protected EffectSound m_WrenchSoundLoop;

	override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(4.0); }
	override void InitActionComponent()
	{
		super.InitActionComponent();
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			m_WrenchSoundLoop = SEffectManager.PlaySoundOnObject("HH_Wrench_Loop_SoundSet", m_ActionData.m_Player);
			// Звук разводного ключа принудительно закольцован на С++ уровне до конца шкалы прогресса!
			if (m_WrenchSoundLoop)
			{
				m_WrenchSoundLoop.SetSoundLoop(true);
			}
		}
	}
	override void OnFinish(bool pCanceled)
	{
		super.OnFinish(pCanceled);
		if (m_WrenchSoundLoop) m_WrenchSoundLoop.SoundStop();
	}
};

class ActionSearchEngineWreck : ActionContinuousBase
{
	void ActionSearchEngineWreck()
	{
		m_CallbackClass = ActionSearchEngineWreckCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; 
		m_FullBody = true;
		m_Text = "Попытаться разобрать"; 
		m_LockTargetOnUse = false;
	}
	
	override typename GetInputType() { return ContinuousInteractActionInput; }
	override void CreateConditionComponents() { m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT); m_ConditionItem = new CCINone(); }

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !player.GetHumanInventory()) return false;
		
		EntityAI itemInHands = player.GetHumanInventory().GetEntityInHands();
		if (!itemInHands || (itemInHands.GetType() != "PipeWrench" && !itemInHands.GetType().Contains("Wrench"))) return false; 

		Object targetObj = target.GetObject();
		if (!targetObj) targetObj = target.GetParent();
		if (!targetObj) return false;
		
		// ЗАЩИТА ТРАНСПОРТА: Если это живой рабочий автомобиль игрока — полностью блокируем экшен!
		if (targetObj.IsKindOf("CarScript") || targetObj.IsKindOf("Car")) return false;

		string typeName = targetObj.GetType(); typeName.ToLower();
		if (typeName.Contains("wreck") || typeName.Contains("volha") || typeName.Contains("offroad"))
		{
			string uniqueCooldownKey = targetObj.GetID().ToString() + "_engine";
			if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey)) return false;
			}
			vector modelPos = targetObj.WorldToModel(player.GetPosition());
			if (modelPos[2] < 0.0) return true; 
		}
		return false;
	}

	
	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Target || !action_data.m_Target.GetObject()) return;
		
		string uniqueCooldownKey = action_data.m_Target.GetObject().GetID().ToString() + "_engine";
		Object targetObj = action_data.m_Target.GetObject();

		// Сначала сервер честно изнашивает разводной ключ и спавнит авто-лут!
		HUSHazardServerManager.ProcessSearch(action_data.m_Player, targetObj, "wreck_engine");

		// Только после этого вешаем на капот часовой замок
		ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
	}
};

// ============================================================================
// ЧАСТЬ 4: ОБЫСК ТРУПОВ ЗОМБИ И РЕГИСТРАЦИЯ ВСЕХ ЭКШЕНОВ В СИСТЕМЕ DAYZ
// ============================================================================
class ActionSearchZombieCB : ActionContinuousBaseCB
{
	protected EffectSound m_SearchSoundLoop;
	override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(5.0); }
	override void InitActionComponent()
	{
		super.InitActionComponent();
		if (GetGame().IsClient() || !GetGame().IsMultiplayer()) m_SearchSoundLoop = SEffectManager.PlaySoundOnObject("HH_Zombie_Search_SoundSet", m_ActionData.m_Player);
	}
	override void OnFinish(bool pCanceled)
	{
		super.OnFinish(pCanceled);
		if (m_SearchSoundLoop) m_SearchSoundLoop.SoundStop();
	}
};

class ActionSearchZombie : ActionContinuousBase
{
	void ActionSearchZombie()
	{
		m_CallbackClass = ActionSearchZombieCB; m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; m_FullBody = true; m_Text = "Search Body (Hazard)"; m_LockTargetOnUse = false; 
	}
	override bool CanBeUsedOnBack() { return false; }
	override bool IsLockTargetOnUse() { return false; }
	override typename GetInputType() { return ContinuousInteractActionInput; }
	override bool HasTarget() { return true; }
	override void CreateConditionComponents() { m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT); m_ConditionItem = new CCINone(); }
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		HumanMovementState movementState = new HumanMovementState(); player.GetMovementState(movementState);
		if (movementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE) return false;
		Object targetObj = target.GetObject(); if (!targetObj) targetObj = target.GetParent(); if (!targetObj || targetObj.IsAlive()) return false;
		string typeName = targetObj.GetType();
		if (typeName != string.Empty)
		{
			typeName.ToLower();
			if (typeName.Contains("zmb") || typeName.Contains("corpse"))
			{
				ZombieBase zBase = ZombieBase.Cast(targetObj); if (zBase && !zBase.m_HH_IsZombieSearched) return true; 
			}
		}
		return false;
	}
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data); PlayerBase player = action_data.m_Player; ZombieBase zombie = ZombieBase.Cast(action_data.m_Target.GetObject());
		if (!zombie) zombie = ZombieBase.Cast(action_data.m_Target.GetParent()); if (!player || !zombie) return;
		zombie.m_HH_IsZombieSearched = true; ref Param p = new Param(); GetGame().RPCSingleParam(zombie, 95201, p, true, player.GetIdentity());
		int glovesSlotId = InventorySlots.GetSlotIdFromString("Gloves"); EntityAI glovesItem = player.GetInventory().FindAttachment(glovesSlotId);
		if (!glovesItem || glovesItem.IsDamageDestroyed())
		{
			PluginLifespan moduleLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan)); if (moduleLifespan) moduleLifespan.UpdateBloodyHandsVisibility(player, true);
			player.InsertAgent(eAgents.BRAIN, 1000); player.MessageAction("[HUSHazard]: Вы испачкали руки в зараженной крови!");
		}
		int maskSlotId = InventorySlots.GetSlotIdFromString("Mask"); EntityAI maskItem = player.GetInventory().FindAttachment(maskSlotId);
		if (!maskItem || maskItem.IsDamageDestroyed())
		{
			player.InsertAgent(eAgents.CHOLERA, 1000); player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT); player.MessageAction("[HUSHazard]: Вдохнув трупные газы без маски, вас выворачивает наизнанку!");
		}
		player.MessageAction("[HUSHazard]: Обыск трупа зараженного завершен.");
	}
};

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionSearchHazard);     
		actions.Insert(ActionSearchFurniture);  
		actions.Insert(ActionSearchZombie);    
		actions.Insert(ActionSearchTrunkWreck);   
		actions.Insert(ActionSearchEngineWreck);  
	}
};

modded class PlayerBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSearchHazard);
		AddAction(ActionSearchFurniture);
		AddAction(ActionSearchZombie);
		AddAction(ActionSearchTrunkWreck);   
		AddAction(ActionSearchEngineWreck);  
	}
}

// ============================================================================
// ХАРДКОРНЫЙ БОЕВОЙ ФИЛЬТР HUSHazard (БЕСШУМНАЯ СЕРВЕРНАЯ ВЕРСИЯ БЕЗ ЛОГОВ)
// ============================================================================

modded class ZombieBase
{
	// Родная С++ сигнатура DayZ 1.29, проверенная вашими исходниками
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		// 1. Сначала даем игре применить ванильный удар
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (!GetGame().IsServer() || !damageResult) return;

		string lowerAmmo = ammo;
		lowerAmmo.ToLower(); // Защита регистра строк от багов компилятора

		string lowerZone = dmgZone;
		lowerZone.ToLower();

		// ----------------============================================================
		// А) ТЗ: ПОЛНОЕ АННУЛИРОВАНИЕ УДАРОВ КУЛАКАМИ
		// ----------------============================================================
		if (lowerAmmo == "meleefist" || lowerAmmo == "meleeheavyfist")
		{
			float infFistHP = damageResult.GetDamage(dmgZone, "Health");
			
			// Если зомби успел умереть на С++ уровне — мгновенно воскрешаем его каркас!
			if (!IsAlive())
			{
				this.SetHealth("GlobalHealth", "Health", 100.0);
			}

			// Возвращаем здоровье конкретной зоны и глобальный пул ХП
			this.SetHealth(dmgZone, "Health", this.GetHealth(dmgZone, "Health") + infFistHP);
			this.SetHealth("GlobalHealth", "Health", this.GetHealth("GlobalHealth", "Health") + infFistHP);
			
			m_DamageHitToProcess = false; // Отключаем падения
			m_ShockDamage = 0;
			return;
		}

		// ----------------============================================================
		// Б) ТЗ: ПОЛНОЕ АННУЛИРОВАНИЕ УДАРОВ ХОЛОДНЫМ ОРУЖИЕМ В ТЕЛО (БЕЗ ПРИНТОВ)
		// ----------------============================================================
		// Если бьют холодным оружием (Melee), но хитбокс попадания — НЕ голова ("Head")
		if (lowerAmmo.Contains("melee") && lowerZone != "head")
		{
			float infBodyHP = damageResult.GetDamage(dmgZone, "Health");

			// Если тяжелое оружие опустило ХП зомби до нуля — сервер принудительно возвращает статус живого
			if (!IsAlive())
			{
				this.SetHealth("GlobalHealth", "Health", 100.0);
			}

			// Восстанавливаем поврежденное туловище или конечность
			this.SetHealth(dmgZone, "Health", this.GetHealth(dmgZone, "Health") + infBodyHP);
			this.SetHealth("GlobalHealth", "Health", this.GetHealth("GlobalHealth", "Health") + infBodyHP);
			
			m_DamageHitToProcess = false; // Намертво блокируем анимацию падения на землю!
			m_ShockDamage = 0;
			return; // Строка Print удалена по ТЗ, лог чист!
		}

		// ----------------============================================================
		// В) ТЗ: МЕХАНИКА ВАНШОТА В ГОЛОВУ ДЛЯ БЕЛОГО СПИСКА
		// ----------------============================================================
		if (lowerZone == "head")
		{
			if (source)
			{
				string itemType = source.GetType();
				
				// ХАРДКОРНЫЙ БЕЛЫЙ СПИСОК ОРУЖИЯ ДЛЯ ВАНШОТА В ЧЕРЕП:
				// Топор пожарного, Лом, Трубный ключ, Бита с гвоздями, Колун
				if (itemType == "FirefighterAxe" || itemType == "Crowbar" || itemType == "PipeWrench" || itemType == "NailedBaseballBat" || itemType == "BarbedBaseballBat" || itemType == "WoodAxe")
				{
					// ОФОРМЛЯЕМ ЧЕСТНЫЙ ВАНШОТ СТРОГО В ГОЛОВУ
					this.SetHealth("Head", "Health", 0.0);
					this.SetHealth("GlobalHealth", "Health", 0.0);
					return; // Строка Print удалена по ТЗ
				}
			}
			
			// Если по голове ударили любым ДРУГИМ холодным оружием (ножи, мачете) — ослабляем урон в 4 раза
			float infHeadHP = damageResult.GetDamage(dmgZone, "Health");
			
			this.SetHealth(dmgZone, "Health", this.GetHealth(dmgZone, "Health") + (infHeadHP * 0.75));
			this.SetHealth("GlobalHealth", "Health", this.GetHealth("GlobalHealth", "Health") + (infHeadHP * 0.75));
			return;
		}
	}
}
