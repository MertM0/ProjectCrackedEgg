

using UnrealBuildTool;
using System.IO;

public class ProjectCrackedEgg : ModuleRules
{
	public ProjectCrackedEgg(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem", "UMG", "GameplayTasks" });

		PublicIncludePaths.AddRange(new string[] {
			Path.Combine(ModuleDirectory, "Public"),
			Path.Combine(ModuleDirectory, "Public/Knight"),
			Path.Combine(ModuleDirectory, "Public/Dragon"),
			Path.Combine(ModuleDirectory, "Public/Enemies"),
			Path.Combine(ModuleDirectory, "Public/Components"),
			Path.Combine(ModuleDirectory, "Public/StatusEffects"),
			Path.Combine(ModuleDirectory, "Public/Core")
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });



	}
}

