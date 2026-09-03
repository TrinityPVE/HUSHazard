// ============================================================================
// HUSHazard - Player Core (Patched for DayZ 1.29 — СИНХРОНИЗАЦИЯ ЗВУКОВ И УРОН)
// ============================================================================

modded class PlayerBase
{
	// Флаг замены звуков анимации обыска багажника на кастомный hh_trunk.ogg
	protected bool m_HH_SoundReplace = false;

	// Конструктор класса: инициализация базовых С++ сетевых переменных
	void PlayerBase()
	{
		// РЕГИСТРАЦИЯ В С++ СЕТЕВОМ СЛОЕ: Синхронизируем переменную между клиентом и сервером
		RegisterNetSyncVariableBool("m_HH_SoundReplace");
	}

	// ИСПРАВЛЕНО: Аргумент убран, метод полностью соответствует ванильному родителю DayZ 1.29
	override void SetActions() 
	{
		super.SetActions();
		
		// Регистрация экшенов обыска мебели и автомобильного сектора HUSHazard
		AddAction(ActionSearchFurniture);
		AddAction(ActionSearchEngineWreck);
		AddAction(ActionSearchTrunkWreck);
	}

	// Управляет флагом подмены звука с пушем сетевой «грязи» (ЭТАЛОН СТОРОННЕГО МОДА)
	void SetHHSoundReplace(bool soundState)
	{
		if (m_HH_SoundReplace == soundState)
			return;

		m_HH_SoundReplace = soundState;

		// Если это server — принудительно пушим сетевой пакет обновления для всех клиентов в стриме
		if (GetGame().IsServer())
			SetSynchDirty();
	}

	bool GetHHSoundReplace()
	{
		return m_HH_SoundReplace;
	}

	// ============================================================================
	// СИСТЕМА УРОНА И ЗАРАЖЕНИЯ: ТОТАЛЬНОЕ ОРИГИНАЛЬНОЕ НАКАЗАНИЕ ИГРОКА ЗОМБИ
	// ============================================================================
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
