// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSandBox.h"
#include "Modules/ModuleManager.h"
#include "ConsoleCommands.h"

class FUnrealSandBoxModule final : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
};

void FUnrealSandBoxModule::StartupModule()
{
	if (!IsRunningCommandlet())
	{
		RegisterSandBoxConsoleCommand();
	}
}


IMPLEMENT_PRIMARY_GAME_MODULE(FUnrealSandBoxModule, UnrealSandBox, "UnrealSandBox");
