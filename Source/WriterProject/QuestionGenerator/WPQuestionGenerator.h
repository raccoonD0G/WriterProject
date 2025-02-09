// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WPQuestionGenerator.generated.h"

/**
 * 
 */
UCLASS()
class WRITERPROJECT_API UWPQuestionGenerator : public UObject
{
	GENERATED_BODY()
	
public:
	UWPQuestionGenerator();
	
// Text Finding Section
public:
	void GetTxtFileFromFolder(const FString& FolderPath);
	FORCEINLINE FString GetSelectedFilePath() const { return SelectedFilePath; }

private:
	FString SelectedFilePath;

// Text Loading Section
private:
	void LoadTxtFromPath();

private:
	TArray<FString> FileLines;

// Sentence Finding Section
public:
	FString ReadNthLineFromFile(int32 LineNumber);

// Validation Check Section
public:
	static bool IsValidSentence(const FString& InSentence);
};
