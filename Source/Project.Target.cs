// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectTarget : TargetRules
{
	public ProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

	    ExtraModuleNames.Add("Project");
	}

}
