// ============================================================================
// HUSHazard - Hard Universal Search & Hazard Mod (Patched for DayZ 1.29)
// World Zombie Module - Clean Synchronized Subclass
// ============================================================================

modded class ZombieBase
{
	bool m_HH_IsZombieSearched = false;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == 95201)
		{
			m_HH_IsZombieSearched = true;
		}
	}

	override bool CanDisplayCargo()
	{
		if (GetHealth("", "") <= 0 && !m_HH_IsZombieSearched) return false;
		return super.CanDisplayCargo();
	}

	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (GetHealth("", "") <= 0 && !m_HH_IsZombieSearched) return false;
		return super.CanReleaseAttachment(attachment);
	}
}