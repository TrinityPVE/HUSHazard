// ============================================================================
// HUSHazard — НИЗКОУРОВНЕВЫЙ ОБРАБОТЧИК ЗВУКОВ АНИМАЦИИ (ПАТЧ DAYZ 1.29)
// ============================================================================

enum HHETrunkSoundEventID
{
	TRUNK_REPLACE = 877
}

modded class ReplaceSoundEventHandler
{
	void ReplaceSoundEventHandler(PlayerBase player)
	{	
		// Инжектим наше событие подмены звука в ванильный менеджер игрока
		RegisterEvent(new HHTrunkBushSoundEvent());
	}
}

class HHSearchSoundEventBase extends ReplaceSoundEventBase
{	
	override protected void SelectSoundID()
	{
		if (!m_Player || !m_Player.GetActionManager()) return;

		// Проверяем текущее выполняемое игроком действие
		ActionBase currentAction = m_Player.GetActionManager().GetRunningAction();
		
		// Если С++ кадр анимации вызвал хруст веток (41) И игрок выполняет экшен багажника — ставим наш звук!
		if (m_ID == HHETrunkSoundEventID.TRUNK_REPLACE && currentAction && currentAction.Type() == ActionSearchTrunkWreck)
		{
			// Движок принудительно запустит ID 95001 из SurvivorBase конфига!
			m_ReplacedSoundAnimID = 95001; 
		}
		else if (m_ID == HHETrunkSoundEventID.TRUNK_REPLACE)
		{
			m_ReplacedSoundAnimID = 41; // В лесу при сборе ресурсов ветки хрустят штатно
		}
	}
}

class HHTrunkBushSoundEvent extends HHSearchSoundEventBase
{
	void HHTrunkBushSoundEvent()
	{
		m_EventType = ESoundEventType.SOUND_COMMON;   
		m_ID = HHETrunkSoundEventID.TRUNK_REPLACE;           
		m_SoundAnimEventClassID = 41; // Перехватываем оригинальный нативный ID звука веток DayZ 1.29
	}
}
