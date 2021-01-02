// Fill out your copyright notice in the Description page of Project Settings.


#include "SampleSubSystem.h"
#include "AsyncSample.h"

//---------------------------------------------------------------------------------
// SampleSubSystem
//---------------------------------------------------------------------------------
TStatId USampleSubSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(SampleSubSystem, STATGROUP_Tickables);
}

void USampleSubSystem::Tick(float DeltaTime)
{
	AsyncSample->Update(DeltaTime);
}

bool USampleSubSystem::IsTickable() const
{
	return true;
}

ETickableTickType USampleSubSystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Conditional;
}

void USampleSubSystem::StartAutoDeleteAsyncSample(float WaitSec)
{
	AsyncSample->StartAutoDeleteAsync(WaitSec);
}

void USampleSubSystem::StartAsyncSample(float WaitSec)
{
	AsyncSample->StartAsyncTask(WaitSec);
}

void USampleSubSystem::CheckAsyncTaskBehaviour()
{
	AsyncSample->CheckAsyncTaskBehaviour();
}

void USampleSubSystem::CancelAsyncSample()
{
	AsyncSample->CancelAsyncTask();
}

void USampleSubSystem::CheckAsyncCrash()
{
	AsyncSample->CheckCrash();
}

void USampleSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AsyncSample = MakeShareable(new FAsyncSample());
}
