// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncSample.h"

class FAsyncSample::FSampleAsyncTask final : public FNonAbandonableTask
{
public:
	explicit FSampleAsyncTask(float SleepSec)
		: SleepSec(SleepSec)
	{
	}

	~FSampleAsyncTask()
	{
		UE_LOG(LogTemp, Log, TEXT("~FSampleAsyncTask"));
	}

	void DoWork()
	{
		UE_LOG(LogTemp, Log, TEXT("Start at %s"), *FDateTime::Now().ToString());
		FPlatformProcess::Sleep(SleepSec);
		UE_LOG(LogTemp, Log, TEXT("Stop at %s"), *FDateTime::Now().ToString());
	}

	TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	friend class FAutoDeleteAsyncTask<FSampleAsyncTask>;
	const float SleepSec;
};

void FAsyncSample::StartAutoDeleteAsync(float WaitSec)
{
	FAutoDeleteAsyncTask<FSampleAsyncTask>* Task = new FAutoDeleteAsyncTask<FSampleAsyncTask>(WaitSec);
	Task->StartBackgroundTask();

	// Memo:
	// FAutoDeleteAsyncTaskはSharedPtrで持っても強制的に削除される。
	// そのためSharedPtrに入れて削除あとにアクセスしようとするとクラッシュする。
}
