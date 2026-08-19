// --- КЛАСС КЛЮЧЕЙ: ГАРАНТИРУЕМ ИНИЦИАЛИЗАЦИЮ ТЕКСТОВОЙ ПАМЯТИ В ДВИЖКЕ ---
modded class HandcuffKeys
{
    protected ref WrittenNoteData m_HUSHNoteData;

    override WrittenNoteData GetWrittenNoteData()
    {
        if (!m_HUSHNoteData)
        {
            m_HUSHNoteData = new WrittenNoteData(this);
        }
        return m_HUSHNoteData;
    }
}

// --- КЛАСС АВТОМОБИЛЯ: СИСТЕМА ПРИВЯЗКИ, ЗАЖИГАНИЯ И ДУБЛИКАТОВ ---
modded class CarScript
{
    protected string m_CarKeyTextGUID = "";

    // --- НАДЕЖНАЯ СИСТЕМА СОХРАНЕНИЯ В БАЗУ ДАННЫХ ПРИ РЕСТАРТАХ ---
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        ctx.Write(m_CarKeyTextGUID);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version)) return false;
        if (!ctx.Read(m_CarKeyTextGUID)) m_CarKeyTextGUID = "";
        return true;
    }

    string HUSH_GetCarGUID() 
    { 
        return m_CarKeyTextGUID; 
    }

    bool HUSH_ClientHasKey()
    {
        PlayerBase p = PlayerBase.Cast(g_Game.GetPlayer());
        if (p)
        {
            ItemBase key = ItemBase.Cast(p.GetHumanInventory().GetEntityInHands());
            if (key && key.GetType() == "HandcuffKeys" && key.GetWrittenNoteData())
            {
                string keyMark = key.GetWrittenNoteData().GetNoteText();
                if (m_CarKeyTextGUID != "" && keyMark == m_CarKeyTextGUID) 
                {
                    return true;
                }
            }
        }
        return false;
    }

    override bool OnBeforeEngineStart()
    {
        if (g_Game.IsServer())
        {
            Human driver = CrewMember(DayZPlayerConstants.VEHICLESEAT_DRIVER);
            PlayerBase pDriver = PlayerBase.Cast(driver);
            if (pDriver && pDriver.GetIdentity())
            {
                ItemBase key = ItemBase.Cast(pDriver.GetHumanInventory().GetEntityInHands());
                bool isKey = (key && key.GetType() == "HandcuffKeys");
                
                string keyMark = "";
                if (isKey && key.GetWrittenNoteData())
                {
                    keyMark = key.GetWrittenNoteData().GetNoteText();
                }

                // Сценарий 1: Машина абсолютно новая/ничья
                if (m_CarKeyTextGUID == "")
                {
                    if (isKey && key.GetWrittenNoteData())
                    {
                        if (keyMark != "") 
                        { 
                            pDriver.MessageAction("Этот ключ уже привязан к другому авто!"); 
                            return false; 
                        }
                        
                        m_CarKeyTextGUID = "KEY-" + Math.RandomInt(100000, 999999).ToString();
                        key.GetWrittenNoteData().SetNoteText(m_CarKeyTextGUID);
                        
                        this.SetSynchDirty();
                        pDriver.MessageAction("Автомобиль успешно привязан!");
                        return super.OnBeforeEngineStart();
                    }
                    
                    pDriver.MessageAction("Требуется ключ");
                    return false;
                }
                // Сценарий 2: Машина уже чья-то, проверяем совпадение строкового кода ключа
                else if (m_CarKeyTextGUID != "" && isKey && key.GetWrittenNoteData() && keyMark == m_CarKeyTextGUID)
                {
                    return super.OnBeforeEngineStart();
                }
                
                pDriver.MessageAction("Автомобиль закрыт");
                return false;
            }
        }
        return super.OnBeforeEngineStart();
    }

    override void OnEngineStop()
    {
        super.OnEngineStop();

        if (g_Game.IsServer() && m_CarKeyTextGUID != "")
        {
            Human driver = CrewMember(DayZPlayerConstants.VEHICLESEAT_DRIVER);
            PlayerBase pDriver = PlayerBase.Cast(driver);
            if (pDriver)
            {
                ItemBase key = ItemBase.Cast(pDriver.GetHumanInventory().GetEntityInHands());
                if (key && key.GetType() == "HandcuffKeys" && key.GetWrittenNoteData())
                {
                    if (key.GetWrittenNoteData().GetNoteText() == "")
                    {
                        key.GetWrittenNoteData().SetNoteText(m_CarKeyTextGUID);
                        pDriver.MessageAction("Успешно создан дубликат ключа!");
                    }
                }
            }
        }
    }

    override bool CanDisplayCargo()
    {
        if (m_CarKeyTextGUID != "" && !HUSH_ClientHasKey())
        {
            EntityAI hood = this.FindAttachmentBySlotName("NivaHood");
            if (!hood) hood = this.FindAttachmentBySlotName("Hood");
            
            if (hood && this.GetAnimationPhase("doorshood") == 0) 
            {
                return false;
            }
        }
        return super.CanDisplayCargo();
    }

    // ИСПРАВЛЕНО: Развернутый многострочный синтаксис с явным финальным return
    override bool CanReceiveItemIntoCargo(EntityAI item) 
    { 
        if (m_CarKeyTextGUID != "" && this.GetAnimationPhase("doorshood") == 0) 
        {
            return false; 
        } 
        return super.CanReceiveItemIntoCargo(item); 
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId) 
    { 
        if (m_CarKeyTextGUID != "" && this.GetAnimationPhase("doorshood") == 0) 
        {
            return false; 
        } 
        return super.CanReceiveAttachment(attachment, slotId); 
    }

    override bool CanReleaseCargo(EntityAI cargo) 
    { 
        if (m_CarKeyTextGUID != "" && this.GetAnimationPhase("doorshood") == 0) 
        {
            return false; 
        } 
        return super.CanReleaseCargo(cargo); 
    }

    override bool CanReleaseAttachment(EntityAI attachment) 
    { 
        if (m_CarKeyTextGUID != "" && this.GetAnimationPhase("doorshood") == 0) 
        {
            return false; 
        } 
        return super.CanReleaseAttachment(attachment); 
    }
}

// --- БЛОКИРОВКА ВЗАИМОДЕЙСТВИЯ С НАВЕСНЫМИ ДВЕРЬМИ, КАПОТОМ И БАГАЖНИКОМ ---
modded class CarDoor
{
    override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
    {
        super.GetActions(action_input_type, actions);
        
        PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
        if (player && player.GetIdentity())
        {
            Object parentObj = this.GetHierarchyParent();
            CarScript car = CarScript.Cast(parentObj);
            
            if (car && car.HUSH_GetCarGUID() != "")
            {
                ItemBase key = ItemBase.Cast(player.GetHumanInventory().GetEntityInHands());
                if (key && key.GetType() == "HandcuffKeys" && key.GetWrittenNoteData())
                {
                    if (key.GetWrittenNoteData().GetNoteText() == car.HUSH_GetCarGUID()) 
                    {
                        return; 
                    }
                }
                
                if (actions)
                {
                    actions.Clear();
                    player.MessageAction("Автомобиль закрыт");
                }
            }
        }
    }
}
