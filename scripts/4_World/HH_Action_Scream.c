// ============================================================================
// HUSHazard Mod — Чистый С++ Экшен Тактического Свиста (DayZ 1.29)
// ============================================================================
class ActionHH_Scream: ActionSingleUseBase
{
	protected int m_HH_LastScreamTime = 0;

	void ActionHH_Scream()
	{
		m_CommandUID = -1; // Полностью отключаем Full-Body анимации рук выжившего
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNone;
		m_ConditionItem = new CCINone;
	}

	override typename GetInputType() { return DefaultActionInput; }
	override bool HasTarget() { return false; }
	override string GetText() { return "Свистнуть"; }

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player && player.HH_CanScream() && !player.GetHumanInventory().GetEntityInHands())
		{
			if (target && target.GetObject()) return false;
			int currentTime = GetGame().GetTime();
			if (m_HH_LastScreamTime > 0 && (currentTime - m_HH_LastScreamTime) < 3000) return false;
			if (player.GetStaminaHandler() && player.GetStaminaHandler().GetStamina() >= 15.0) return true;
		}
		return false;
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);
		PlayerBase player = action_data.m_Player;
		if (player)
		{
			m_HH_LastScreamTime = GetGame().GetTime();
			SEffectManager.PlaySoundOnObject("HH_Scream_SoundSet", player);
		}
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		PlayerBase player = action_data.m_Player;
		if (!player || !player.IsAlive()) return;

		m_HH_LastScreamTime = GetGame().GetTime();

		if (player.GetStaminaHandler())
		{
			float currentStamina = player.GetStaminaHandler().GetStamina();
			player.GetStaminaHandler().SetStamina(currentStamina - 15.0);
		}

		// ЛЕГАЛЬНОЕ ВЕЩАНИЕ АКУСТИЧЕСКОГО ИМПУЛЬСА ПО С++ СЕТКЕ КООРДИНАТ
		NoiseParams screamNoise = new NoiseParams();
		// Загружаем наш кастомный шаблон, который прописан в CfgAI суб-модуля
		screamNoise.Load("HH_Scream_Noise_60M"); 
		
		// Пускаем волну от координат игрока. Так как уши зомби расширены до 65м,
		// они зафиксируют этот акустический вектор, выйдут из покоя Calm 
		// и лавиной побегут со всей площади строго к точке вашего свиста!
		GetGame().GetNoiseSystem().AddNoiseTarget(player.GetPosition(), 1.0, screamNoise, 1.0);
	}
}
