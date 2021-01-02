// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


class FAsyncSample final
{
public:

	void StartAutoDeleteAsync(float WaitSec);

private:
	class FSampleAsyncTask;
};
