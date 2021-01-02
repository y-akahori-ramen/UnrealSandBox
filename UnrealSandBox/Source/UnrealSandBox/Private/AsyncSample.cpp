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


#if 0
class ExampleAsyncTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<ExampleAsyncTask>;

	int32 ExampleData;

	ExampleAsyncTask(int32 InExampleData)
		: ExampleData(InExampleData)
	{
	}

	void DoWork()
	{
		//... do the work here
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

void Example()
{

	//start an example job

	FAsyncTask<ExampleAsyncTask>* MyTask = new FAsyncTask<ExampleAsyncTask>(5);
	MyTask->StartBackgroundTask();

	//--or --

	MyTask->StartSynchronousTask();

	//to just do it now on this thread
	//Check if the task is done :

	if (MyTask->IsDone())
	{
	}

	//Spinning on IsDone is not acceptable( see EnsureCompletion ), but it is ok to check once a frame.
	//Ensure the task is done, doing the task on the current thread if it has not been started, waiting until completion in all cases.

	MyTask->EnsureCompletion();
	delete Task;
}
#endif
