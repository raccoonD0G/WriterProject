// Fill out your copyright notice in the Description page of Project Settings.


#include "WriterProject/QuestionGenerator/WPQuestionGenerator.h"

UWPQuestionGenerator::UWPQuestionGenerator()
{
    SelectedFilePath = TEXT("");
    FileLines.Empty();
}

void UWPQuestionGenerator::GetTxtFileFromFolder(const FString& FolderPath)
{
    if (SelectedFilePath != TEXT(""))
    {
        UE_LOG(LogTemp, Log, TEXT("File Already Selected"));
        return;
    }

    TArray<FString> FileNames;

    // 폴더에서 .txt 파일을 검색
    FString SearchPattern = FolderPath / TEXT("*.txt");
    IFileManager& FileManager = IFileManager::Get();

    FileManager.FindFiles(FileNames, *SearchPattern, true, false); // true: 파일만 검색, false: 디렉토리는 제외

    // .txt 파일이 있는지 확인
    if (FileNames.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No .txt files found in folder: %s"), *FolderPath);
        return;
    }

    // 파일 목록 출력 (옵션)
    for (int32 i = 0; i < FileNames.Num(); i++)
    {
        UE_LOG(LogTemp, Log, TEXT("%d: %s"), i + 1, *FileNames[i]);
    }

    int32 SelectedIndex = FMath::RandRange(0, FileNames.Num() - 1);
    FString SelectedFile = FolderPath / FileNames[SelectedIndex];

    UE_LOG(LogTemp, Log, TEXT("Selected file: %s"), *SelectedFile);
    
    SelectedFilePath = SelectedFile;
}

void UWPQuestionGenerator::LoadTxtFromPath()
{
    // 파일 경로가 유효한지 확인
    if (!FPaths::FileExists(SelectedFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("File not found: %s"), *SelectedFilePath);
    }

    if (!FileLines.IsEmpty())
    {
        UE_LOG(LogTemp, Log, TEXT("Text Already Loaded"));
        return;
    }

    if (!FFileHelper::LoadFileToStringArray(FileLines, *SelectedFilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Text Load Failed"));
    }
}

FString UWPQuestionGenerator::ReadNthLineFromFile(int32 LineNumber)
{
    LoadTxtFromPath();

    // 라인 번호가 유효한지 확인
    if (LineNumber > 0 && LineNumber <= FileLines.Num())
    {
        return FileLines[LineNumber - 1]; // 배열은 0부터 시작하므로 -1
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Line number %d is out of range. File has %d lines."), LineNumber, FileLines.Num());
        return FString();
    }
}

bool UWPQuestionGenerator::IsValidSentence(const FString& InSentence)
{
    // 허용할 문자: 한글, 영어, 숫자, 공백, 문장 부호(.,!?'") 허용
    FString AllowedCharacters = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .,!?'\"");

    for (int32 i = 0; i < InSentence.Len(); i++)
    {
        TCHAR Character = InSentence[i];

        // 한글 범위 체크 (유니코드 기준: U+AC00 ~ U+D7A3)
        if ((Character >= 0xAC00 && Character <= 0xD7A3) || AllowedCharacters.Contains(FString(1, &Character)))
        {
            continue; // 허용된 문자이므로 계속 검사
        }

        return false; // 허용되지 않은 특수문자 발견 시 false 반환
    }

    return true; // 특수문자가 없으면 true 반환
}
