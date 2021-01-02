// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsoleCommands.h"
#include "SampleSubSystem.h"

namespace ConsoleCommandsInternal
{
	UWorld* GetAnyGameWorld()
	{
		UWorld* TestWorld = nullptr;
		const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
		for (const FWorldContext& WorldContext : WorldContexts)
		{
			if (((WorldContext.WorldType == EWorldType::PIE) || (WorldContext.WorldType == EWorldType::Game)) && (WorldContext.World() != nullptr))
			{
				TestWorld = WorldContext.World();
				break;
			}
		}

		return TestWorld;
	}

	USampleSubSystem* GetSampleSubSystem()
	{
		UWorld* World = GetAnyGameWorld();
		if (World != nullptr)
		{
			return World->GetGameInstance()->GetSubsystem<USampleSubSystem>();
		}
		else
		{
			return nullptr;
		}
	}
}

void RegisterSandBoxConsoleCommand()
{
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("StartAutoDeleteAsyncSample"),
		TEXT("StartAutoDeleteAsyncSample WaitSec"),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			USampleSubSystem* SubSystem = ConsoleCommandsInternal::GetSampleSubSystem();
			if (Args.Num() == 1 && SubSystem != nullptr)
			{
				const float WaitSec = FCString::Atof(*Args[0]);
				SubSystem->StartAutoDeleteAsyncSample(WaitSec);
			}
		}),
		ECVF_Default
	);
}
