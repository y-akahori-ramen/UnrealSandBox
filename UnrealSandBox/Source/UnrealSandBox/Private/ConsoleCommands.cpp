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

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("StartAsyncSample"),
		TEXT("StartAsyncSample WaitSec"),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			USampleSubSystem* SubSystem = ConsoleCommandsInternal::GetSampleSubSystem();
			if (Args.Num() == 1 && SubSystem != nullptr)
			{
				const float WaitSec = FCString::Atof(*Args[0]);
				SubSystem->StartAsyncSample(WaitSec);
			}
		}),
		ECVF_Default
	);

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("CancelAsyncSample"),
		TEXT("CancelAsyncSample"),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			USampleSubSystem* SubSystem = ConsoleCommandsInternal::GetSampleSubSystem();
			SubSystem->CancelAsyncSample();
		}),
		ECVF_Default
	);

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("CheckAsyncTaskBehaviour"),
		TEXT("CheckAsyncTaskBehaviour"),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			USampleSubSystem* SubSystem = ConsoleCommandsInternal::GetSampleSubSystem();
			SubSystem->CheckAsyncTaskBehaviour();
		}),
		ECVF_Default
	);

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("CheckAsyncCrash"),
		TEXT("CheckAsyncCrash"),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			USampleSubSystem* SubSystem = ConsoleCommandsInternal::GetSampleSubSystem();
			SubSystem->CheckAsyncCrash();
		}),
		ECVF_Default
	);
}
