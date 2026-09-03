// ============================================================================
// HUSHazard - Hard Universal Search & Hazard Mod (Patched for DayZ 1.29)
// World Zombie Module - Полнотекстовое скрытие из меню Tab до обыска
// ============================================================================
modded class ZombieBase
{
	// Синхронизированный статус: был ли обыскан этот конкретный зомби
	bool m_HH_IsZombieSearched = false;

	override void Init()
	{
		super.Init();
		// Регистрируем переменную в сетевой С++ таблице для мгновенного обновления интерфейса
		RegisterNetSyncVariableBool("m_HH_IsZombieSearched");
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		// Ловим пакет об успешном завершении обыска по кнопке F
		if (rpc_type == 95201)
		{
			m_HH_IsZombieSearched = true;
			SetSynchDirty(); // Заставляем сервер перерисовать меню Tab у игрока
		}
	}

	// ТОТ САМЫЙ ПОЗАВЧЕРАШНИЙ РАБОЧИЙ МЕТОД:
	// Полностью удаляет труп зомби из левой колонки "Окружение" в меню Tab до обыска!
	override bool IsInventoryVisible()
	{
		// Проверка здоровья строго по синтаксису вашего оригинального PlayerBase с GitHub
		if (!IsAlive() && !m_HH_IsZombieSearched)
		{
			return false; // Труп зомби становится абсолютно невидимым для UI инвентаря
		}
		return super.IsInventoryVisible();
	}

	// Скрывает внутреннее содержимое карманов зомби
	override bool CanDisplayCargo()
	{
		if (!IsAlive() && !m_HH_IsZombieSearched) return false;
		return super.CanDisplayCargo();
	}

	// Скрывает серые слоты одежды
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!IsAlive() && !m_HH_IsZombieSearched) return false;
		return super.CanReleaseAttachment(attachment);
	}
}
