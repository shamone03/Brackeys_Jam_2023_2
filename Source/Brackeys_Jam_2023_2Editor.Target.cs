// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Brackeys_Jam_2023_2EditorTarget : TargetRules
{
	public Brackeys_Jam_2023_2EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Brackeys_Jam_2023_2" } );
	}
}
