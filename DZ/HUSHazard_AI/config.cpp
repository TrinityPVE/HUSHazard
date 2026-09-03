class CfgPatches
{
	class HUSHazard_AI
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {
			"DZ_Data",
			"DZ_AI",
			"JM_CF_Scripts"
		};
	};
};

class CfgAI
{
	class NoiseTemplates
	{
		class HH_Scream_Noise_60M
		{
			strength = 90; // Сила импульса, рассчитанная строго на 60 метров площади
			type = "shot"; // Маркер shot снимает 30-метровый аппаратный лимит пустых рук
		};
	};
};

class CfgAIBehaviours
{
	class Infected
	{
		class NoiseTemplates
		{
			class StepNoise
			{
				strength = 10;
				type = "sound"; // Обычные шаги — строгая ванила (игроки могут тихо ходить)
			};
			class HH_Scream_Noise_60M
			{
				strength = 90;
				type = "shot";
			};
		};

		class BehaviourHLZombie
		{
			class SlotAlerted
			{
				class BehaviourZombieAlerted
				{
					maxTimeInDisturbedState = 0; // Зомби мгновенно срываются на свист без 2-секундных раздумий
					alertToAttract = 0.80000001;
					attractedSearchDistance = 40; 
				};
			};

			class SlotAttracted
			{
				class BehaviourZombieAttracted
				{
					searchDistance = 6;          // Точечный фикс камушка: поиск в радиусе 6 метров
					maxTimeInAttractedState = 4; // Быстро успокаиваются от броска камня (4 секунды)
				};
			};

			class AlertSystem
			{
				visionToAlertMultiplier = 20;
				noiseToAlertMultiplier = 1;        // Строгая ванила слуха шагов
				damageToAlertMultiplier = 10000;
				noiseShotToAlertMultiplier = 1.7;  // Строгая ванила паники от шагов
				class Calm
				{
					DropSpeed = 1;
					DropDelay = 0;
					MaxAlertValue = 20;
				};
			};
		};

		// СЕНСОРЫ ЗРЕНИЯ ЗОМБИ (СТРОЖАЙШАЯ ВАНИЛА — ОГРАНИЧЕНИЕ 40 МЕТРОВ)
		class TargetSystemDZBase
		{
			visionProximityRange = 2.5;
			visionProximityStrengthMult = 2;
			visionCloseRange = 10;
			visionCloseHeight = 1.8;
			visionCloseStrengthMult = 1.5;
			visionRangeMin = 20;
			visionRangeMax = 40; // Больше с 80 метров они вас не увидят и баланс не сломается!
			visionFov = 1;
			visionPeripheralRangeMin = 2.5;
			visionPeripheralRangeMax = 20;
			visionPeripheralFov = 2.3;
		};

		// УШИ ЗОМБИ (РАСШИРЕНИЕ СЛУХА СТРОГО ДО 60-70 МЕТРОВ ПЛОЩАДИ)
		class NoiseSystemParams
		{
			rangeMin = 0;
			rangeMax = 65;          // ТОЧЕЧНОЕ РАСШИРЕНИЕ: Зомби способны уловить свист кулаков до 65 метров!
			rangeShotMin = 25;
			rangeShotMax = 110;     // Оставляем ванильный предел выстрелов
			radiusMin = 10;
			radiusMax = 25;         
			radiusShotMin = 35;
			radiusShotMax = 70;     
			shotVolExponent = 1;
		};
	};
};
