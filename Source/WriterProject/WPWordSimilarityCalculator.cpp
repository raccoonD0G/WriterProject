// Fill out your copyright notice in the Description page of Project Settings.

#include "WPWordSimilarityCalculator.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

void UWPWordSimilarityCalculator::RunServerFastTextSimilarity(const FString& Word1, const FString& Word2, TFunction<void(float)> Callback)
{
    FString Url = TEXT("http://127.0.0.1:5000/similarity");

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("word1", Word1);
    JsonObject->SetStringField("word2", Word2);

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(JsonString);

    Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
        if (!bWasSuccessful)
        {
            UE_LOG(LogTemp, Error, TEXT("HTTP Request failed"));
            Callback(-2.0f);
            return;
        }

        if (!Response.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Response is invalid"));
            Callback(-2.0f);
            return;
        }

        int32 ResponseCode = Response->GetResponseCode();
        FString ResponseStr = Response->GetContentAsString();

        UE_LOG(LogTemp, Log, TEXT("HTTP Response Code: %d"), ResponseCode);
        UE_LOG(LogTemp, Log, TEXT("Server Response: %s"), *ResponseStr);

        float LastSimilarityScore;

        // JSON �Ľ�
        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

        if (!FJsonSerializer::Deserialize(Reader, JsonResponse) || !JsonResponse.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response: %s"), *ResponseStr);
            LastSimilarityScore = -2.0f;
            Callback(-2.0f);
            return;
        }

        if (!JsonResponse->HasField("similarity"))
        {
            UE_LOG(LogTemp, Error, TEXT("Missing 'similarity' field in JSON response"));
            LastSimilarityScore = -2.0f;
            Callback(-2.0f);
            return;
        }

        // ���絵 �� ����
        LastSimilarityScore = JsonResponse->GetNumberField("similarity");

        UE_LOG(LogTemp, Log, TEXT("Parsed similarity score: %f"), LastSimilarityScore);

        // �ݹ� ����
        Callback(LastSimilarityScore);
        });

    Request->ProcessRequest();
}


float UWPWordSimilarityCalculator::RunFastTextSimilarity(const FString& Word1, const FString& Word2)
{
    EnsurePackageInstalled();

    // �ý��ۿ� ��ϵ� �⺻ Python ���
    FString PythonPath = TEXT("python");  

    // ������Ʈ Content ���丮 ���� Python ��ũ��Ʈ ���
    FString PythonScriptPath = FPaths::ProjectContentDir() / TEXT("Scripts/fasttext_similarity.py");

    // Python ���� ���� ����
    FString Command = FString::Printf(TEXT("\"%s\" \"%s\" \"%s\""), *PythonScriptPath, *Word1, *Word2);

    UE_LOG(LogTemp, Log, TEXT("%s"), *Command);

    // ���μ��� ���� �� ��� ĸó
    FString Output;
    FString StdErr;
    int32 ReturnCode;

    FPlatformProcess::ExecProcess(*PythonPath, *Command, &ReturnCode, &Output, &StdErr);

    if (ReturnCode != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("FastText ���� ����: %s"), *StdErr);
        return -1.0f; // ���� �߻� �� -1 ��ȯ
    }

    UE_LOG(LogTemp, Log, TEXT("Out : %s"), *Output);

    // Python ��ũ��Ʈ�� ��°��� float�� ��ȯ
    return FCString::Atof(*Output);
}


void UWPWordSimilarityCalculator::EnsurePackageInstalled()
{
    FString PythonPath = TEXT("python"); // Python ���� ���� ���
    TArray<FString> ModulesToCheck = { TEXT("fasttext"), TEXT("numpy") };

    for (const FString& Module : ModulesToCheck)
    {
        FString CheckCommand = FString::Printf(TEXT("-c \"import %s\""), *Module);
        FString InstallCommand = FString::Printf(TEXT("-m pip install %s"), *Module);

        FString Output;
        FString StdErr;
        int32 ReturnCode;

        // Python �����Ͽ� ��� Ȯ��
        FPlatformProcess::ExecProcess(*PythonPath, *CheckCommand, &ReturnCode, &Output, &StdErr);

        if (ReturnCode != 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s ����� ��ġ���� �ʾҽ��ϴ�. ��ġ�� �õ��մϴ�..."), *Module);
            FPlatformProcess::ExecProcess(*PythonPath, *InstallCommand, &ReturnCode, &Output, &StdErr);

            if (ReturnCode == 0)
            {
                UE_LOG(LogTemp, Log, TEXT("%s ����� ���������� ��ġ�Ǿ����ϴ�."), *Module);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("%s ��� ��ġ ����! ���� �޽���: %s"), *Module, *StdErr);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("%s ����� ���������� ��ġ�Ǿ� �ֽ��ϴ�."), *Module);
        }
    }
}