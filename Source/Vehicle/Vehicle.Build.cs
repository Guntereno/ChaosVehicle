

using UnrealBuildTool;

public class Vehicle : ModuleRules
{
	public Vehicle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[] 
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ChaosVehicles",
				"EnhancedInput"
			});
	}
}
