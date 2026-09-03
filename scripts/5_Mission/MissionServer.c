modded class ActionTargetsCursor
{
	// Перегружаем ванильный метод DayZ 1.29, который вы прислали из репозитория
	override void GetItemCargoCount(out int cargoCount)
	{
		// Инициализируем стандартный ноль предметов
		cargoCount = 0;
		
		Object tgObject = m_DisplayInteractTarget;
		if (!tgObject && m_Target)
		{
			tgObject = m_Target.GetObject();
		}

		if (tgObject)
		{
			// ИСПРАВЛЕНО НАМЕРТВО ПО ВАШЕЙ ИДЕЕ: Исключаем зомби из класса контейнеров для 3D-HUD!
			if (tgObject.IsInherited(ZombieBase) || tgObject.IsInherited(DayZInfected))
			{
				// Принудительно отдаем 0 предметов, полностью блокируя вызов счетчика одежды и карманов
				cargoCount = 0;
				return; // Мгновенный выход, плюс стерт из 3D-прицела навсегда!
			}
		}

		// Для всех остальных объектов (шкафов, ящиков, машин) возвращаем стандартную логику DayZ
		super.GetItemCargoCount(cargoCount);
	}
};
modded class Container
{
	// Нативный С++ метод DayZ 1.29 (уровень 5_Mission), управляющий раскрытием и видимостью вложений одежды в UI
	override bool CanOpenCloseContainerEx(EntityAI focusedEntity)
	{
		if (m_Entity)
		{
			// Приводим тип строго к вашему ZombieBase, где лежит переменная
			ZombieBase h_zombie = ZombieBase.Cast(m_Entity);
			if (h_zombie && !h_zombie.IsAlive())
			{
				// Используем вашу оригинальную и 100% рабочую переменную обыска из HH_Zombie.c!
				if (!h_zombie.m_HH_IsZombieSearched)
				{
					// Намертво возвращаем false для UI! 
					// Менеджер инвентаря считает, что слотов одежды нет, и ПОЛНОСТЬЮ СТИРАЕТ их из Tab!
					return false;
				}
			}
		}
		
		return super.CanOpenCloseContainerEx(focusedEntity);
	}
};

