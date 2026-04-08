// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Special_effects : ModuleRules
{
	public Special_effects(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "MotionWarping", "UMG", "LevelSequence", "WebSockets", "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		// Add HTTP module dependency for WebSocket functionality
		PrivateDependencyModuleNames.AddRange(new string[] { "HTTP", "Json", "JsonUtilities" });

		// Define RPG_TU2_API macro globally for all files in this module
		PublicDefinitions.Add("RPG_TU2_API=");
	}
}
