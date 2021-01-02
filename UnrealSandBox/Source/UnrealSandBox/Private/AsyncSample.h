// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


class FAsyncSample final
{
public:

	void StartAutoDeleteAsync(float WaitSec);
	void StartAsyncTask(float WaitSec);
	void CancelAsyncTask();
	void CheckAsyncTaskBehaviour();

	void Update(float Deltatime);

private:
	class FSampleAsyncTask;
	TSharedPtr<FAsyncTask<FSampleAsyncTask>> AsyncTask;
};
