modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == 95202)
		{
			ref Param1<string> rpcParam = new Param1<string>("");
			if (!ctx.Read(rpcParam)) return;
			
			string rpcMarker = rpcParam.param1;

			// Клиентская часть с защитой от падения рендеринга UI
			if (GetGame().IsClient())
			{
				if (ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns)
				{
					ActionSearchFurniture.m_HH_GlobalFurnitureCooldowns.Set(rpcMarker, GetGame().GetTime() + 3600000);
				}
				return;
			}

			// Серверная логика наказаний
			if (GetGame().IsServer() && this.IsAlive() && this.GetInventory())
			{
				EntityAI gloves = this.GetInventory().FindAttachment(InventorySlots.GLOVES);
				if (!gloves || gloves.IsRuined())
				{
					this.SetBloodyHands(true);
					if (this.GetBleedingManagerServer())
					{
						this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftHand");
					}
				}
			}
		}
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (GetGame().IsServer() && source && source.IsInherited(ZombieBase) && this.IsAlive())
		{
			float halfMaxHealth = this.GetMaxHealth("", "Health") * 0.5;
			this.DecreaseHealth("", "Health", halfMaxHealth, false);
		}
	}
};
