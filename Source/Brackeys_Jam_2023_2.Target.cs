// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Brackeys_Jam_2023_2Target : TargetRules
{
	public Brackeys_Jam_2023_2Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Brackeys_Jam_2023_2" } );
	}
}
