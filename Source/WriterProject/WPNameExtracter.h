// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WPNameExtracter.generated.h"

/**
 * 
 */
UCLASS()
class WRITERPROJECT_API UWPNameExtracter : public UObject
{
	GENERATED_BODY()
	
public:
	static void ExtractNamedEntities(FString InputText);
};
