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

    // �������� .txt ������ �˻�
    FString SearchPattern = FolderPath / TEXT("*.txt");
    IFileManager& FileManager = IFileManager::Get();

    FileManager.FindFiles(FileNames, *SearchPattern, true, false); // true: ���ϸ� �˻�, false: ���丮�� ����

    // .txt ������ �ִ��� Ȯ��
    if (FileNames.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No .txt files found in folder: %s"), *FolderPath);
        return;
    }

    // ���� ��� ��� (�ɼ�)
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
    // ���� ��ΰ� ��ȿ���� Ȯ��
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

    // ���� ��ȣ�� ��ȿ���� Ȯ��
    if (LineNumber > 0 && LineNumber <= FileLines.Num())
    {
        return FileLines[LineNumber - 1]; // �迭�� 0���� �����ϹǷ� -1
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Line number %d is out of range. File has %d lines."), LineNumber, FileLines.Num());
        return FString();
    }
}

bool UWPQuestionGenerator::IsValidSentence(const FString& InSentence)
{
    // ����� ����: �ѱ�, ����, ����, ����, ���� ��ȣ(.,!?'") ���
    FString AllowedCharacters = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .,!?'\"");

    for (int32 i = 0; i < InSentence.Len(); i++)
    {
        TCHAR Character = InSentence[i];

        // �ѱ� ���� üũ (�����ڵ� ����: U+AC00 ~ U+D7A3)
        if ((Character >= 0xAC00 && Character <= 0xD7A3) || AllowedCharacters.Contains(FString(1, &Character)))
        {
            continue; // ���� �����̹Ƿ� ��� �˻�
        }

        return false; // ������ ���� Ư������ �߰� �� false ��ȯ
    }

    return true; // Ư�����ڰ� ������ true ��ȯ
}
