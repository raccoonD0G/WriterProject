// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WPWordSimilarityCalculator.generated.h"

/**
 * 
 */
UCLASS()
class WRITERPROJECT_API UWPWordSimilarityCalculator : public UObject
{
	GENERATED_BODY()
	
public:
	static void RunServerFastTextSimilarity(const FString& Word1, const FString& Word2, TFunction<void(float)> Callback);
	static float RunFastTextSimilarity(const FString& Word1, const FString& Word2);

private:
	static void EnsurePackageInstalled();
};
