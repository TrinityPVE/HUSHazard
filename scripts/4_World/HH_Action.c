// ============================================================================
// HUSHazard - Action, Combat & Registration Core (Patched for DayZ 1.29)
// ============================================================================

// ----------------------------------------------------------------------------
// СНАЧАЛА ОБЪЯВЛЯЕМ ВСЕ КЛАССЫ КОЛБЭКОВ (ПОРЯДОК СИНТАКСИСА ДЛЯ DAYZ 1.29)
// ----------------------------------------------------------------------------
class ActionSearchHazardCB : ActionContinuousBaseCB
{
	override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(4.0); }
};

class ActionSearchFurnitureCB : ActionContinuousBaseCB
{
	protected EffectSound m_SearchSoundLoop;

	override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(4.0); }
	override void InitActionComponent()
	{
		super.InitActionComponent();
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			m_SearchSoundLoop = SEffectManager.PlaySoundOnObject("HH_Zombie_Search_SoundSet", m_ActionData.m_Player);
		}
	}
	override void OnFinish(bool pCanceled) { super.OnFinish(pCanceled); if (m_SearchSoundLoop) m_SearchSoundLoop.SoundStop(); }
};

class ActionSearchEngineWreckCB : ActionContinuousBaseCB
{
	override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(4.0); }
};

class ActionSearchTrunkWreckCB : ActionContinuousBaseCB
{
	override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(4.0); }
};
// ----------------------------------------------------------------------------
// ЧАСТЬ 1: УЛИЧНЫЙ СЕКТОР (КУРЯТНИКИ, БУДКИ, СУХИЕ ТУАЛЕТЫ)
// ----------------------------------------------------------------------------
class ActionSearchHazard : ActionContinuousBase
{
	void ActionSearchHazard() { m_CallbackClass = ActionSearchHazardCB; m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; m_FullBody = true; m_Text = "Обыскать"; m_LockTargetOnUse = false; }
	override typename GetInputType() { return ContinuousInteractActionInput; }
	override void CreateConditionComponents() { m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT); m_ConditionItem = new CCINone(); }

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObj = target.GetObject();
		if (!targetObj) targetObj = target.GetParent();
		if (!targetObj) return false;

		string typeName = targetObj.GetType(); typeName.ToLower();
		if (typeName.Contains("toilet") || typeName.Contains("kennel") || typeName.Contains("dog") || typeName.Contains("coop") || typeName.Contains("chicken"))
		{
			if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns && ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(targetObj.GetID().ToString() + "_street"))
			{
				if (GetGame().GetTime() < ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(targetObj.GetID().ToString() + "_street")) return false;
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

		string typeName = targetObj.GetType(); typeName.ToLower();

		EntityAI gloves = player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (gloves && !gloves.IsRuined())
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		string targetCategory = typeName;
		if (targetCategory.Contains("toilet"))       targetCategory = "toilet_dry";
		else if (targetCategory.Contains("kennel") || targetCategory.Contains("dog")) targetCategory = "dog_kennel";
		else if (targetCategory.Contains("coop")   || targetCategory.Contains("chicken")) targetCategory = "chicken_coop";

		HUSHazardServerManager.ProcessSearch(player, targetObj, targetCategory);
		
		string uniqueCooldownKey = targetObj.GetID().ToString() + "_street";
		if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
		{
			ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
		}
		
		ref Param1<string> rpcKeyParam = new Param1<string>(targetCategory);
		GetGame().RPCSingleParam(player, 95202, rpcKeyParam, true, player.GetIdentity());
	}
};

// ----------------------------------------------------------------------------
// ЧАСТЬ 2: МЕБЕЛЬНЫЙ СЕКТОР (ШКАФЫ И ИНТЕРЬЕРЫ)
// ----------------------------------------------------------------------------
class ActionSearchFurniture : ActionContinuousBase
{
	static ref map<string, int> m_HH_GlobalFurnitureCooldowns = new map<string, int>();

	void ActionSearchFurniture() { m_CallbackClass = ActionSearchFurnitureCB; m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; m_FullBody = true; m_Text = "Обыскать"; m_LockTargetOnUse = false; }
	override typename GetInputType() { return ContinuousInteractActionInput; }
	override void CreateConditionComponents() { m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT); m_ConditionItem = new CCINone(); }

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObj = target.GetObject();
		if (!targetObj) return false;
		
		int componentIndex = target.GetComponentIndex();
		string selectionName = targetObj.GetActionComponentName(componentIndex);
		if (selectionName == string.Empty) return false;
		selectionName.ToLower();

		string typeName = targetObj.GetType(); typeName.ToLower();
		
		if (typeName.Contains("cupboard") || typeName.Contains("shelves") || typeName.Contains("table") || typeName.Contains("dresser") || typeName.Contains("wardrobe") || typeName.Contains("refrigerator") || typeName.Contains("sideboard") || typeName.Contains("bookcase") || typeName.Contains("cabinet"))
		{
			string uniqueCooldownKey = targetObj.GetID().ToString() + "_" + selectionName;
			if (m_HH_GlobalFurnitureCooldowns && m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey)) return false;
			}
			return true;
		}
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Player || !action_data.m_Target) return;
		
		Object targetObj = action_data.m_Target.GetObject();
		if (!targetObj) return;

		int componentIndex = action_data.m_Target.GetComponentIndex();
		string selectionName = targetObj.GetActionComponentName(componentIndex);
		selectionName.ToLower();

		EntityAI gloves = action_data.m_Player.GetInventory().FindAttachment(InventorySlots.GLOVES);
		if (gloves && !gloves.IsRuined())
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		string uniqueCooldownKey = targetObj.GetID().ToString() + "_" + selectionName;
		HUSHazardServerManager.ProcessSearch(action_data.m_Player, targetObj, selectionName);
		if (m_HH_GlobalFurnitureCooldowns)
		{
			m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
		}
	}
};
// ----------------------------------------------------------------------------
// ЧАСТЬ 3: АВТОМОБИЛЬНЫЙ СЕКТОР (КАПОТЫ И БАГАЖНИКИ ОСТОВОВ МАШИН)
// ----------------------------------------------------------------------------
class ActionSearchEngineWreck : ActionContinuousBase
{
	void ActionSearchEngineWreck() { m_CallbackClass = ActionSearchEngineWreckCB; m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; m_FullBody = true; m_Text = "Обыскать капот"; m_LockTargetOnUse = false; }
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
			if (!ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
			{
				ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns = new map<string, int>();
			}

			string uniqueCooldownKey = targetObj.GetID().ToString() + "_engine";
			if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey)) return false;
			}
			vector modelPos = targetObj.WorldToModel(player.GetPosition());
			if (modelPos < 0.0) return true; 
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
		if (gloves && !gloves.IsRuined())
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		HUSHazardServerManager.ProcessSearch(player, targetObj, "wreck_engine");
		if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
		{
			ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
		}
	}
};

class ActionSearchTrunkWreck : ActionContinuousBase
{
	void ActionSearchTrunkWreck() { m_CallbackClass = ActionSearchTrunkWreckCB; m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING; m_FullBody = true; m_Text = "Обыскать багажник"; m_LockTargetOnUse = false; }
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
			if (!ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
			{
				ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns = new map<string, int>();
			}

			string uniqueCooldownKey = targetObj.GetID().ToString() + "_trunk";
			if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Contains(uniqueCooldownKey))
			{
				if (GetGame().GetTime() < ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Get(uniqueCooldownKey)) return false;
			}
			vector modelPos = targetObj.WorldToModel(player.GetPosition());
			if (modelPos >= 0.0) return true; 
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
		if (gloves && !gloves.IsRuined())
		{
			gloves.DecreaseHealth("", "", 6.0);
		}

		HUSHazardServerManager.ProcessSearch(player, targetObj, "wreck_trunk");
		if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
		{
			ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(uniqueCooldownKey, GetGame().GetTime() + 3600000);
		}

		ref Param1<string> rpcKeyParam = new Param1<string>("wreck_trunk");
		GetGame().RPCSingleParam(player, 95202, rpcKeyParam, true, player.GetIdentity());
	}
};

// ----------------------------------------------------------------------------
// ЧАСТЬ 4: ХАРДКОРНЫЙ БOЕВOЙ ФИЛЬТР ЗOМБИ (API DAYZ 1.29)
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

// ----------------------------------------------------------------------------
// РЕЛЬСА №5: ИСПРАВЛЕННЫЙ БЛОК ДВОЙНОЙ РЕГИСТРАЦИИ ЭКШЕНОВ (ПОРЯДОК DAYZ 1.29)
// ----------------------------------------------------------------------------
modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionSearchHazard);     
		actions.Insert(ActionSearchFurniture);  
		actions.Insert(ActionSearchTrunkWreck);   
		actions.Insert(ActionSearchEngineWreck);  
	}
};

modded class PlayerBase
{
	// ИСПРАВЛЕНО ДЛЯ 1.29: Метод SetActions приведен в идеальный нулевой вид без ложных стрингов!
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSearchHazard);
		AddAction(ActionSearchFurniture);
		AddAction(ActionSearchTrunkWreck);   
		AddAction(ActionSearchEngineWreck);  
	}
}
