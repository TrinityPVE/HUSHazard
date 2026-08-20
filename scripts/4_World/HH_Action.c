// ============================================================================
// HUSHazard - Action & Combat Core (Patched for DayZ 1.29 — СТЕРИЛЬНЫЙ ВОЗВРАТ)
// ============================================================================

class ActionSearchHazardCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(4.0);
	}
};

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

class ActionSearchEngineWreckCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(4.0);
	}
};

class ActionSearchTrunkWreckCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(4.0);
	}
};
// ----------------------------------------------------------------------------
// ЧАСТЬ 1: УЛИЧНЫЙ СЕКТОР (КУРЯТНИКИ, БУДКИ, СУХИЕ ТУАЛЕТЫ)
// ----------------------------------------------------------------------------
class ActionSearchHazard : ActionContinuousBase
{
	void ActionSearchHazard()
	{
		m_CallbackClass = ActionSearchFurnitureCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_Text = "Обыскать";
		m_LockTargetOnUse = false;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINone();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObj = target.GetObject();
		if (!targetObj) targetObj = target.GetParent();
		if (!targetObj) return false;

		string typeName = targetObj.GetType();
		typeName.ToLower();

		if (typeName.Contains("toilet") || typeName.Contains("kennel") || typeName.Contains("dog") || typeName.Contains("coop") || typeName.Contains("chicken"))
		{
			string uniqueCooldownKey = targetObj.GetID().ToString() + "_street";
			if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey))
					return false;
			}
			return true;
		}
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		if (!action_data || !action_data.m_Player) return;

		PlayerBase player = action_data.m_Player;
		Object targetObj = action_data.m_Target.GetObject();
		if (!targetObj) targetObj = action_data.m_Target.GetParent();
		if (!targetObj) return;

		string typeName = targetObj.GetType();
		typeName.ToLower();

		// НАКАЗАНИЕ НА СЕРВЕРЕ ПРИ ОБЫСКЕ УЛИЦЫ БЕЗ ПЕРЧАТОК
		EntityAI gloves = player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (!gloves || gloves.IsRuined())
		{
			player.SetBloodyHands(true);
			if (player.GetBleedingManagerServer())
			{
				player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftArm");
				player.MessageAction("[HUSHazard]: Ай! Вы сильно порезали незащищенную руку о занозу!");
			}
		}
		else
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		string targetCategory = typeName;
		if (targetCategory.Contains("toilet"))       targetCategory = "toilet_dry";
		else if (targetCategory.Contains("kennel") || targetCategory.Contains("dog")) targetCategory = "dog_kennel";
		else if (targetCategory.Contains("coop")   || targetCategory.Contains("chicken")) targetCategory = "chicken_coop";

		HUSHazardServerManager.ProcessSearch(player, targetObj, targetCategory);
		
		string uniqueCooldownKey = targetObj.GetID().ToString() + "_street";
		ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
		
		ref Param1<string> rpcKeyParam = new Param1<string>(uniqueCooldownKey);
		GetGame().RPCSingleParam(player, 95202, rpcKeyParam, true, player.GetIdentity());
	}
};
// ----------------------------------------------------------------------------
// ЧАСТЬ 2: МЕБЕЛЬНЫЙ СЕКТОР (РОДНОЙ МЕТОД, КОТОРЫЙ РАБОТАЛ НА 100%)
// ----------------------------------------------------------------------------
class ActionSearchFurniture : ActionContinuousBase
{
	static ref map<string, int> m_HH_GlobalFurnitureCooldowns = new map<string, int>;

	void ActionSearchFurniture()
	{
		m_CallbackClass = ActionSearchFurnitureCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_Text = "Обыскать";
		m_LockTargetOnUse = false;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINone();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObj = target.GetObject();
		if (!targetObj) return false;
		
		int componentIndex = target.GetComponentIndex();
		string selectionName = targetObj.GetActionComponentName(componentIndex);
		if (selectionName == string.Empty) return false;
		selectionName.ToLower();

		if (selectionName.Contains("component") || selectionName.Contains("door") || selectionName.Contains("drawer") || selectionName.Contains("shaf") || selectionName.Contains("box") || selectionName.Contains("lodka"))
		{
			vector playerLocalPos = targetObj.WorldToModel(player.GetPosition());
			if (vector.Distance(playerLocalPos, target.GetCursorHitPos()) > 1.8)
				return false;

			string uniqueCooldownKey = targetObj.GetID().ToString() + "_" + selectionName;
			if (m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey))
					return false;
			}
			return true;
		}
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Player || !action_data.m_Target) return;
		
		PlayerBase player = action_data.m_Player;
		Object targetObj = action_data.m_Target.GetObject();
		if (!targetObj) return;

		int componentIndex = action_data.m_Target.GetComponentIndex();
		string selectionName = targetObj.GetActionComponentName(componentIndex);
		selectionName.ToLower();

		// НАКАЗАНИЕ НА СЕРВЕРЕ ПРИ ОБЫСКЕ МЕБЕЛИ БЕЗ ПЕРЧАТОК
		EntityAI gloves = player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (!gloves || gloves.IsRuined())
		{
			player.SetBloodyHands(true);
			if (player.GetBleedingManagerServer())
			{
				player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftArm");
				player.MessageAction("[HUSHazard]: Ай! Вы сильно порезали незащищенную руку о занозу в шкафу!");
			}
		}
		else
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		string uniqueCooldownKey = targetObj.GetID().ToString() + "_" + selectionName;
		HUSHazardServerManager.ProcessSearch(player, targetObj, selectionName);
		m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
	}
};
// ----------------------------------------------------------------------------
// ЧАСТЬ 3: АВТОМОБИЛЬНЫЙ СЕКТОР (СВЕРЕНО С ВАНИЛЬНЫМ API DAYZ 1.29)
// ----------------------------------------------------------------------------
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
		if (!action_data || !action_data.m_Target || !action_data.m_Target.GetObject() || !action_data.m_MainItem) return;
		
		PlayerBase player = action_data.m_Player;
		Object targetObj = action_data.m_Target.GetObject();
		string uniqueCooldownKey = targetObj.GetID().ToString() + "_engine";

		action_data.m_MainItem.DecreaseHealth("", "", 4.0);

		EntityAI gloves = player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (!gloves || gloves.IsRuined())
		{
			if (Math.RandomFloat01() < 0.30)
			{
				if (player.GetBleedingManagerServer())
				{
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftArm");
					player.MessageAction("[HUSHazard]: Вы сильно распороли ладонь о ржавые детали моторного отсека!");
				}

				// СВЕРЕНО С ВАШИМ РЕПОЗИТОРИЕМ 1.29: Внедряем строго eAgents.WOUND_AGENT
				player.InsertAgent(eAgents.WOUND_AGENT, 100); 
				if (player.GetModifiersManager() && !player.GetModifiersManager().IsModifierActive(31))
				{
					player.GetModifiersManager().ActivateModifier(31); 
				}
				player.MessageAction("[HUSHazard]: Грязь и моторное масло попали в кровь. Рана начинает стремительно гноиться!");
			}
		}
		else
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		HUSHazardServerManager.ProcessSearch(player, targetObj, "wreck_engine");
		ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
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
			if (modelPos[2] >= 0.0) return true;
		}
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Target || !action_data.m_Target.GetObject()) return;
		
		PlayerBase player = action_data.m_Player;
		Object targetObj = action_data.m_Target.GetObject();
		string uniqueCooldownKey = targetObj.GetID().ToString() + "_trunk";

		EntityAI gloves = player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (!gloves || gloves.IsRuined())
		{
			if (Math.RandomFloat01() < 0.30)
			{
				if (player.GetBleedingManagerServer())
				{
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightArm");
					player.MessageAction("[HUSHazard]: Вы глубоко распороли ладонь об острый край ржавого багажника!");
				}

				// СВЕРЕНО С ВАШИМ РЕПОЗИТОРИЕМ 1.29: Внедряем строго eAgents.WOUND_AGENT
				player.InsertAgent(eAgents.WOUND_AGENT, 100); 
				if (player.GetModifiersManager() && !player.GetModifiersManager().IsModifierActive(31))
				{
					player.GetModifiersManager().ActivateModifier(31); 
				}
				player.MessageAction("[HUSHazard]: Ржавчина попала в кровь. Развивается острое заражение раны!");
			}
		}
		else
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		HUSHazardServerManager.ProcessSearch(player, targetObj, "wreck_trunk");
		ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
	}
};

// ----------------------------------------------------------------------------
// ЧАСТЬ 4: ОБЫСК ТРУПОВ ЗОМБИ (ОРИГИНАЛЬНАЯ РАБОЧАЯ СИСТЕМА TrinityPVE)
// ----------------------------------------------------------------------------
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
		super.OnFinishProgressServer(action_data); 
		PlayerBase player = action_data.m_Player; 
		ZombieBase zombie = ZombieBase.Cast(action_data.m_Target.GetObject());
		if (!zombie) zombie = ZombieBase.Cast(action_data.m_Target.GetParent()); 
		if (!player || !zombie) return;
		
		zombie.m_HH_IsZombieSearched = true; 
		ref Param p = new Param(); 
		GetGame().RPCSingleParam(zombie, 95201, p, true, player.GetIdentity());
		
		int glovesSlotId = InventorySlots.GetSlotIdFromString("Gloves"); 
		EntityAI glovesItem = player.GetInventory().FindAttachment(glovesSlotId);
		if (!glovesItem || glovesItem.IsDamageDestroyed())
		{
			PluginLifespan moduleLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan)); 
			if (moduleLifespan) moduleLifespan.UpdateBloodyHandsVisibility(player, true);
			player.InsertAgent(eAgents.BRAIN, 1000); 
			player.MessageAction("[HUSHazard]: Вы испачкали руки в зараженной крови!");
		}
		
		int maskSlotId = InventorySlots.GetSlotIdFromString("Mask"); 
		EntityAI maskItem = player.GetInventory().FindAttachment(maskSlotId);
		if (!maskItem || maskItem.IsDamageDestroyed())
		{
			player.InsertAgent(eAgents.CHOLERA, 1000); 
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT); 
			player.MessageAction("[HUSHazard]: Вдохнув трупные газы без маски, вас выворачивает наизнанку!");
		}
		player.MessageAction("[HUSHazard]: Обыск трупа зараженного завершен.");
	}
};
// ----------------------------------------------------------------------------
// ЧАСТЬ 5: ХАРДКОРНЫЙ БOЕВOЙ ФИЛЬТР ЗOМБИ И ДВОЙНОЙ РЕЕСТР (API DAYZ 1.29)
// ----------------------------------------------------------------------------
modded class DayZPlayerMeleeFightLogic_LightHeavy
{
	override void EvaluateHit_Common(InventoryItem weapon, Object target, bool forcedDummy = false, int forcedWeaponMode = -1)
	{
		if (!GetGame().IsServer() || !target)
		{
			super.EvaluateHit_Common(weapon, target, forcedDummy, forcedWeaponMode);
			return;
		}

		if (target.IsInherited(ZombieBase))
		{
			if (m_MeleeCombat.GetFinisherType() > -1)
			{
				super.EvaluateHit_Common(weapon, target, forcedDummy, forcedWeaponMode);
				return;
			}

			int currentHitZoneIdx = m_MeleeCombat.GetHitZoneIdx();
			string zoneName = target.GetActionComponentName(currentHitZoneIdx);
			string lowerZone = zoneName; lowerZone.ToLower();

			if (!weapon)
			{
				DamageSystem.CloseCombatDamage(m_Player, target, currentHitZoneIdx, "Dummy_Light", target.GetPosition(), ProcessDirectDamageFlags.NO_ATTACHMENT_TRANSFER);
				return;
			}

			if (weapon && lowerZone != "head" && lowerZone != "neck")
			{
				DamageSystem.CloseCombatDamage(m_Player, target, currentHitZoneIdx, "Dummy_Light", target.GetPosition(), ProcessDirectDamageFlags.NO_ATTACHMENT_TRANSFER);
				return;
			}

			if (lowerZone == "head" || lowerZone == "neck")
			{
				string itemType = weapon.GetType();
				if (itemType == "FirefighterAxe" || itemType == "Crowbar" || itemType == "PipeWrench" || itemType == "NailedBaseballBat" || itemType == "BarbedBaseballBat" || itemType == "WoodAxe")
				{
					super.EvaluateHit_Common(weapon, target, forcedDummy, forcedWeaponMode);
					ZombieBase zomb = ZombieBase.Cast(target);
					if (zomb && zomb.IsAlive())
					{
						zomb.SetHealth("Head", "Health", 0.0);
						zomb.SetHealth("GlobalHealth", "Health", 0.0);
					}
					return;
				}
				
				DamageSystem.CloseCombatDamage(m_Player, target, currentHitZoneIdx, "MeleeFist", target.GetPosition(), ProcessDirectDamageFlags.NO_ATTACHMENT_TRANSFER);
				return;
			}
		}

		super.EvaluateHit_Common(weapon, target, forcedDummy, forcedWeaponMode);
	}
}

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
