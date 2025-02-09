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

        // JSON 파싱
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

        // 유사도 값 저장
        LastSimilarityScore = JsonResponse->GetNumberField("similarity");

        UE_LOG(LogTemp, Log, TEXT("Parsed similarity score: %f"), LastSimilarityScore);

        // 콜백 실행
        Callback(LastSimilarityScore);
        });

    Request->ProcessRequest();
}


float UWPWordSimilarityCalculator::RunFastTextSimilarity(const FString& Word1, const FString& Word2)
{
    EnsurePackageInstalled();

    // 시스템에 등록된 기본 Python 사용
    FString PythonPath = TEXT("python");  

    // 프로젝트 Content 디렉토리 내의 Python 스크립트 경로
    FString PythonScriptPath = FPaths::ProjectContentDir() / TEXT("Scripts/fasttext_similarity.py");

    // Python 실행 인자 설정
    FString Command = FString::Printf(TEXT("\"%s\" \"%s\" \"%s\""), *PythonScriptPath, *Word1, *Word2);

    UE_LOG(LogTemp, Log, TEXT("%s"), *Command);

    // 프로세스 실행 및 결과 캡처
    FString Output;
    FString StdErr;
    int32 ReturnCode;

    FPlatformProcess::ExecProcess(*PythonPath, *Command, &ReturnCode, &Output, &StdErr);

    if (ReturnCode != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("FastText 실행 오류: %s"), *StdErr);
        return -1.0f; // 오류 발생 시 -1 반환
    }

    UE_LOG(LogTemp, Log, TEXT("Out : %s"), *Output);

    // Python 스크립트의 출력값을 float로 변환
    return FCString::Atof(*Output);
}


void UWPWordSimilarityCalculator::EnsurePackageInstalled()
{
    FString PythonPath = TEXT("python"); // Python 실행 파일 경로
    TArray<FString> ModulesToCheck = { TEXT("fasttext"), TEXT("numpy") };

    for (const FString& Module : ModulesToCheck)
    {
        FString CheckCommand = FString::Printf(TEXT("-c \"import %s\""), *Module);
        FString InstallCommand = FString::Printf(TEXT("-m pip install %s"), *Module);

        FString Output;
        FString StdErr;
        int32 ReturnCode;

        // Python 실행하여 모듈 확인
        FPlatformProcess::ExecProcess(*PythonPath, *CheckCommand, &ReturnCode, &Output, &StdErr);

        if (ReturnCode != 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s 모듈이 설치되지 않았습니다. 설치를 시도합니다..."), *Module);
            FPlatformProcess::ExecProcess(*PythonPath, *InstallCommand, &ReturnCode, &Output, &StdErr);

            if (ReturnCode == 0)
            {
                UE_LOG(LogTemp, Log, TEXT("%s 모듈이 성공적으로 설치되었습니다."), *Module);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("%s 모듈 설치 실패! 오류 메시지: %s"), *Module, *StdErr);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("%s 모듈이 정상적으로 설치되어 있습니다."), *Module);
        }
    }
}