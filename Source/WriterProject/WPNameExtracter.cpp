// Fill out your copyright notice in the Description page of Project Settings.


#include "WPNameExtracter.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UWPNameExtracter::ExtractNamedEntities(FString InputText)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://127.0.0.1:5000/extract_named_entities"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    FString JsonPayload = FString::Printf(TEXT("{\"text\": \"%s\"}"), *InputText);
    Request->SetContentAsString(JsonPayload);

    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful && Response.IsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("Named Entities: %s"), *Response->GetContentAsString());
            }
        });

    Request->ProcessRequest();
}
