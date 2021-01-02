// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SampleSubSystem.generated.h"

class FAsyncSample;

/**
 * 色々試す用のサブシステム
 */
UCLASS()
class USampleSubSystem final : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	
	void StartAutoDeleteAsyncSample(float WaitSec);
	void StartAsyncSample(float WaitSec);
	void CheckAsyncTaskBehaviour();
	void CancelAsyncSample();

private:
	TSharedPtr<FAsyncSample> AsyncSample;
};
