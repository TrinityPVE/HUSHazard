// ============================================================================
// HUSHazard Mod — Нативная Перегрузка Слуха с Выводом Дебаг-Логов (DayZ 1.29)
// ============================================================================
modded class NoiseAIEvaluate 
{	
	override static float GetNoiseMultiplier(DayZPlayerImplement playerImplement)
	{
		// 1. Чистокровный ванильный расчет шума шагов движка Enfusion
		float speedNoise = GetNoiseMultiplierByPlayerSpeed(playerImplement);
		float shoesNoise = GetNoiseMultiplierByShoes(playerImplement);
		float surfaceNoise = GetNoiseMultiplierBySurface(playerImplement);
		
		surfaceNoise *= SURFACE_NOISE_WEIGHT;
		float avgNoise = (shoesNoise + surfaceNoise) / (1 + SURFACE_NOISE_WEIGHT);
		avgNoise *= speedNoise;
		
		// 2. БЕЗОПАСНЫЙ С++ ДЕБАГ-МОНИТОРИНГ БЕЗ ИЗМЕНЕНИЯ БАЛАНСА ШАГОВ
		if (GetGame().IsDedicatedServer() && playerImplement)
		{
			HumanMovementState playerState = new HumanMovementState;
			playerImplement.GetMovementState(playerState);
			
			if (playerState)
			{
				string stanceName = "STAND/RUN";
				
				if (playerState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_CROUCH || playerState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH || playerState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || playerState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
				{
					stanceName = "CROUCH/PRONE";
				}	

				// Пишем лог строго в том виде, в каком его считает сама игра
				Print("[HH_DEBUG_EARS]: Игрок перемещается! Стойка: " + stanceName + " | Итоговый множитель шума (avgNoise): " + avgNoise.ToString());
			}
		}
		
		return avgNoise;
	}
}
