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
	friend class FAsyncTask<FSampleAsyncTask>;
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

void FAsyncSample::StartAsyncTask(float WaitSec)
{
	CancelAsyncTask();
	AsyncTask = MakeShareable(new FAsyncTask<FSampleAsyncTask>(WaitSec));
	AsyncTask->StartBackgroundTask();
}

void FAsyncSample::CancelAsyncTask()
{
	UE_LOG(LogTemp, Log, TEXT("Start FAsyncSample::CancelAsyncTask()"));
	// キャンセルの検証用
	// 完了していなければキャンセル。キャンセルできなければ完了をまってタスクを削除してからタスクを開始
	if (AsyncTask.IsValid())
	{
		if (AsyncTask->IsDone())
		{
			UE_LOG(LogTemp, Log, TEXT("Task is done."));
		}
		else if (AsyncTask->Cancel())
		{
			UE_LOG(LogTemp, Log, TEXT("Task canceled."))
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Task couldn't canceled."));
			UE_LOG(LogTemp, Log, TEXT("Start ensure completion."));
			AsyncTask->EnsureCompletion();
			UE_LOG(LogTemp, Log, TEXT("Finish ensure completion."));
		}
	}
	AsyncTask.Reset();
	UE_LOG(LogTemp, Log, TEXT("Finish FAsyncSample::CancelAsyncTask()"));
}

void FAsyncSample::CheckAsyncTaskBehaviour()
{
	using FTaskType = FAsyncTask<FSampleAsyncTask>;

	// 挙動チェックその１
	{
		UE_LOG(LogTemp, Log, TEXT("Check AsyncTask Behaviour 1"));
		TSharedPtr<FTaskType> Task = MakeShareable(new FTaskType(0.1));

		// スレッドプールに追加しなければIsDoneはtrueを返す。
		check(Task->IsDone());

		// スレッドプールに追加しなければタスク開始されていないためIsIdleはtrueを返す。
		check(Task->IsIdle());

		Task->StartBackgroundTask();

		// StartBackgroundTaskの状況したいで、このタイミングで呼び出してもキャンセルが行えない場合もある
		if (Task->Cancel())
		{
			UE_LOG(LogTemp, Log, TEXT("canceled"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Ensure completion"));
			Task->EnsureCompletion();
		}
	}

	// 挙動チェックその２　タスクのキャンセルが成功する状況の確認
	{
		UE_LOG(LogTemp, Log, TEXT("Check AsyncTask Behaviour 2"));

		// タスクのStartBackgroundTask関数呼び出しで引数を省略するとGThreadPoolが指定される
		// GThreadPoolで使用できるスレッド数以上のタスクを登録すればタスク実行待ちとなりキャンセルが可能になる
		UE_LOG(LogTemp, Log, TEXT("GThreadPool->GetNumThreads(): %d"), GThreadPool->GetNumThreads());
		const int32 NumTasks = GThreadPool->GetNumThreads() + 3;
		TArray<TSharedPtr<FTaskType>> Tasks;
		Tasks.Reserve(NumTasks);

		for (int32 i = 0; i < NumTasks; ++i)
		{
			TSharedPtr<FTaskType> Task = MakeShareable(new FTaskType(0.5f));
			Task->StartBackgroundTask(GThreadPool);
			Tasks.Add(Task);
		}

		// キャンセルを行う
		// スレッド数+3のタスクを開始させたため最後に追加した3つのタスクはキャンセルに成功する
		for (int32 i = 0; i < NumTasks; ++i)
		{
			const bool bCanceled = Tasks[i]->Cancel();
			UE_LOG(LogTemp, Log, TEXT("Task[%d] Canceled: %d"), i, bCanceled);
		}

		// スコープを抜けると削除されるので安全に削除するため完了を待つ
		for (TSharedPtr<FTaskType> Task : Tasks)
		{
			Task->EnsureCompletion();
		}
	}
}

void FAsyncSample::Update(float Deltatime)
{
	if (AsyncTask.IsValid() && AsyncTask->IsDone())
	{
		UE_LOG(LogTemp, Log, TEXT("Task is done."));
		AsyncTask.Reset();
	}
}
