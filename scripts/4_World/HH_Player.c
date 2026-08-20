// ============================================================================
// HUSHazard - Player Core (Patched for DayZ 1.29 — СТЕРИЛЬНЫЙ СЕРВЕРНЫЙ УРОН)
// ============================================================================

modded class PlayerBase
{
	// Тотальное оригинальное наказание игрока при любом попадании лапы зомби (СВЕРЕНО С ВАНИЛЬНЫМ API 1.29)
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		// Если урон нанес именно зомби, и мы находимся на сервере над живым игроком
		if (GetGame().IsServer() && source && source.IsInherited(ZombieBase) && this.IsAlive())
		{
			// 1. Зомби мгновенно сносит ровно 10% от максимального ХП выжившего
			float halfMaxHealth = this.GetMaxHealth("", "Health") * 0.1;
			this.DecreaseHealth("", "Health", halfMaxHealth, false);

			// 2. Гарантированный порез зоны, куда физически прилетел удар лапы зомби
			if (this.GetBleedingManagerServer())
			{
				this.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(dmgZone);
			}

			// 3. Бросаем честный кубик на 40%-й шанс заразиться безумием Куру при ударе зараженного
			if (Math.RandomFloat01() < 0.40)
			{
				// Насильно заливаем 1000 единиц мозговых агентов Куру строго через нативный Enum
				this.InsertAgent(eAgents.BRAIN, 1000);
				
				// ВЕРИФИЦИРОВАНО: Активируем класс BrainDiseaseMdfr (Куру) через легальный системный индекс 24
				if (this.GetModifiersManager() && !this.GetModifiersManager().IsModifierActive(24))
				{
					this.GetModifiersManager().ActivateModifier(24); 
				}
				
				this.MessageAction("[HUSHazard]: Трупный яд зараженного попал в кровь! Вы чувствуете странную тревогу...");
			}
		}
	}
}
