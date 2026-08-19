modded class CarScript
{
    // Временные переменные для кеширования факта удара зомби и точной зоны контакта
    protected bool m_ZombieHitRecorded = false;
    protected string m_ZombieHitZoneName = "";
    protected vector m_ZombieHitPosVector = "0 0 0";

    // 1. УМНОЕ ОТКЛЮЧЕНИЕ УРОНА ОТ СТАТИКИ ДО 40 КМ/Ч ЧЕРЕЗ ВОССТАНОВЛЕНИЕ КАДРА
    override void CheckContactCache()
    {
        // Сначала сохраняем здоровье корпуса ДО удара о статику
        float hpBeforeCrash = this.GetHealth("GlobalHealth", "Health");

        // Кэшируем прочность ключевых зон кузова до аварии
        float frontHp = this.GetHealth("Front", "Health");
        float backHp  = this.GetHealth("Back", "Health");
        float leftHp  = this.GetHealth("Left", "Health");
        float rightHp = this.GetHealth("Right", "Health");

        // Даем игре выполнить честный ванильный обсчет ДТП (это включит звуки, искры и урон на 80 км/ч)
        super.CheckContactCache();

        if (!g_Game.IsServer())
            return;

        float currentSpeed = GetSpeedometer();
        float absSpeed = Math.AbsFloat(currentSpeed);

        // Если скорость в момент аварии о стену/дерево была 40 км/ч или ниже — ОТМЕНЯЕМ урон
        if (absSpeed <= 40.0)
        {
            // Возвращаем здоровье каркаса автомобиля в исходное состояние
            this.SetHealth("GlobalHealth", "Health", hpBeforeCrash);

            // Мгновенно восстанавливаем кузовные зоны, чтобы они не помялись и не требовали паяльника
            if (this.m_DamageZoneMap && this.m_DamageZoneMap.Contains("Front")) this.SetHealth("Front", "Health", frontHp);
            if (this.m_DamageZoneMap && this.m_DamageZoneMap.Contains("Back"))  this.SetHealth("Back", "Health", backHp);
            if (this.m_DamageZoneMap && this.m_DamageZoneMap.Contains("Left"))  this.SetHealth("Left", "Health", leftHp);
            if (this.m_DamageZoneMap && this.m_DamageZoneMap.Contains("Right")) this.SetHealth("Right", "Health", rightHp);

            // ЛЕЧИМ ИГРОКОВ ВНУТРИ: Если удар о стену на малой скорости нанес урон водителю/пассажирам — полностью исцеляем их
            for (int c = 0; c < CrewSize(); ++c)
            {
                Human crew = CrewMember(c);
                if (crew)
                {
                    PlayerBase player;
                    if (Class.CastTo(player, crew) && player.IsAlive())
                    {
                        // Полностью убираем полученный шок и восстанавливаем здоровье до 100%
                        player.SetHealth("", "Shock", 100.0);
                        player.SetHealth("", "Health", 100.0);
                    }
                }
            }
        }
    }

    // 2. Ловим низкоуровневое касание с зомби на сервере и мгновенно фиксируем точную зону кузова!
    override void OnContact(string zoneName, vector localPos, IEntity other, Contact data)
    {
        super.OnContact(zoneName, localPos, other, data);

        if (!g_Game.IsServer())
            return;

        if (!other)
            return;

        Object hitObject = Object.Cast(other);
        if (!hitObject)
            return;

        // Наш урон по зомби полностью изолирован от CheckContactCache и сработает со 100% гарантией
        if (hitObject.IsInherited(ZombieBase))
        {
            float currentSpeed = GetSpeedometer(); 
            // Срабатывание и звуки от зомби строго от 1.0 км/ч!
            if (Math.AbsFloat(currentSpeed) >= 1.0)
            {
                m_ZombieHitRecorded = true;
                m_ZombieHitZoneName = zoneName; 
                m_ZombieHitPosVector = hitObject.GetPosition(); // Сохраняем мировую позицию зомби для матрицы
            }
        }
    }
    // 3. Безопасное и суровое нанесение урона по деталям при наезде на зомби
    override void OnUpdate(float dt)
    {
        super.OnUpdate(dt);

        if (!g_Game.IsServer())
            return;

        // Если в этом цикле обновления был зафиксирован наезд на зомби
        if (m_ZombieHitRecorded)
        {
            m_ZombieHitRecorded = false; // Сразу сбрасываем триггер кадра

            float currentSpeed = GetSpeedometer(); 
            float absSpeed = Math.AbsFloat(currentSpeed);

            // Мягкое масштабирование урона от скорости (солидный урон по зомби)
            float speedMultiplier = absSpeed / 25.0; 
            if (speedMultiplier < 0.15) speedMultiplier = 0.15; 
            
            float finalDamage = Math.RandomFloat(25.0, 45.0) * speedMultiplier;

            // Вычисляем сторону удара через матрицу трансформации 1.29
            vector carMatrix; 
            this.GetTransform(carMatrix); 

            vector relativeHitPos = m_ZombieHitPosVector - this.GetPosition();
            
            // ИСПРАВЛЕНО: Сначала записываем результат трансформации в полноценный вектор hitLoc
            vector hitLoc = relativeHitPos.InvMultiply3(carMatrix);

            // Теперь безопасно извлекаем оси float по индексам [0] и [2]
            float hitX = hitLoc[0]; // Поперечная ось X (Лево / Право)
            float hitZ = hitLoc[2]; // Продольная ось Z (Вперед / Назад)

            // Списки зон кузова для точечного нанесения урона через DecreaseHealth
            ref array<string> partsToDamage = new array<string>;
            string selectedZone = m_ZombieHitZoneName;

            string checkZone = m_ZombieHitZoneName;
            checkZone.ToLower();

            // Распределяем урон на основе физической зоны из OnContact
            if (checkZone.Contains("front") || checkZone.Contains("bumperfront") || checkZone.Contains("reflector")) 
            {
                // ---------------- Передняя часть автомобиля ----------------
                selectedZone = "Front";
                partsToDamage.Insert("Front"); 

                EntityAI hoodAttachment = this.FindAttachmentBySlotName("NivaHood");
                if (!hoodAttachment) hoodAttachment = this.FindAttachmentBySlotName("Hood");

                if (hoodAttachment && !hoodAttachment.IsRuined())
                {
                    hoodAttachment.AddHealth("", "Health", -finalDamage);
                }

                float hoodHealthCoef = 1.0;
                if (hoodAttachment) hoodHealthCoef = hoodAttachment.GetHealth01("", "");

                if (!hoodAttachment || hoodHealthCoef < 0.7)
                {
                    partsToDamage.Insert("Reflector_1_1"); // Левая фара
                    partsToDamage.Insert("Reflector_2_1"); // Правая фара
                    
                    EntityAI radiatorItem = this.FindAttachmentBySlotName("CarRadiator");
                    if (radiatorItem && !radiatorItem.IsRuined())
                    {
                        radiatorItem.AddHealth("", "Health", -finalDamage);
                        if (absSpeed > 25.0) this.LeakFluid(CarFluid.COOLANT); // Пускаем пар радиатора
                    }
                }
            }
            else if (checkZone.Contains("back") || checkZone.Contains("bumperback") || checkZone.Contains("trunk")) 
            {
                // ---------------- Задняя полусфера ----------------
                selectedZone = "Back";
                partsToDamage.Insert("Back"); 

                EntityAI trunkAttachment = this.FindAttachmentBySlotName("NivaTrunk");
                if (!trunkAttachment) trunkAttachment = this.FindAttachmentBySlotName("Trunk");

                if (trunkAttachment && !trunkAttachment.IsRuined())
                {
                    trunkAttachment.AddHealth("", "Health", -finalDamage);
                }
            }
            else if (checkZone.Contains("left") || checkZone.Contains("fender_1")) 
            {
                // ---------------- Левая сторона ----------------
                selectedZone = "Left";
                partsToDamage.Insert("Fender_1_1"); // Переднее левое крыло
                partsToDamage.Insert("Fender_1_2"); // Заднее левое крыло
                partsToDamage.Insert("Body");       // Борта V3S

                EntityAI doorL1 = this.FindAttachmentBySlotName("NivaDriverDoors");
                if (!doorL1) doorL1 = this.FindAttachmentBySlotName("DriverDoors");
                
                EntityAI doorL2 = this.FindAttachmentBySlotName("LeftDoors");
                
                if (doorL1 && !doorL1.IsRuined()) doorL1.AddHealth("", "Health", -finalDamage);
                if (doorL2 && !doorL2.IsRuined()) doorL2.AddHealth("", "Health", -finalDamage);
            }
            else 
            {
                // ---------------- Правая сторона ----------------
                selectedZone = "Right";
                partsToDamage.Insert("Fender_2_1"); // Переднее правое крыло
                partsToDamage.Insert("Fender_2_2"); // Заднее правое крыло
                partsToDamage.Insert("Body");       // Борта V3S

                EntityAI doorR1 = this.FindAttachmentBySlotName("NivaCoDriverDoors");
                if (!doorR1) doorR1 = this.FindAttachmentBySlotName("CoDriverDoors");
                
                EntityAI doorR2 = this.FindAttachmentBySlotName("RightDoors");
                
                if (doorR1 && !doorR1.IsRuined()) doorR1.AddHealth("", "Health", -finalDamage);
                if (doorR2 && !doorR2.IsRuined()) doorR2.AddHealth("", "Health", -finalDamage);
            }

            // Абсолютно БЕЗОПАСНОЕ точечное нанесение урона зонам кузова самой машины через m_DamageZoneMap
            foreach (string zoneClassName : partsToDamage)
            {
                if (this.m_DamageZoneMap && this.m_DamageZoneMap.Contains(zoneClassName))
                {
                    this.DecreaseHealth(zoneClassName, "Health", finalDamage);
                }
            }

            if (this.m_DamageZoneMap && this.m_DamageZoneMap.Contains(selectedZone))
            {
                this.DecreaseHealth(selectedZone, "Health", finalDamage);
            }

            // Наносим суровый урон напрямую в ТЕЛО машины (GlobalHealth). 
            this.DecreaseHealth("GlobalHealth", "Health", finalDamage * 0.75);

            // 4. Звуковое сопровождение удара зомби
            if (absSpeed > 35.0)
            {
                this.SynchCrashHeavySound(true);
            }
            else
            {
                this.SynchCrashLightSound(true);
            }
            
            this.SetSynchDirty();
        }
    }
}
