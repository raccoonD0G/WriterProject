#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "WriterProject/WPWordSimilarityCalculator.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWPWordSimilarityCalculatorTest, "Project.Tests.WPWordSimilarityCalculator", 
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWPWordSimilarityCalculatorTest::RunTest(const FString& Parameters)
{
    // 1. 테스트할 객체 생성
    UWPWordSimilarityCalculator* SimilarityCalculator = NewObject<UWPWordSimilarityCalculator>();
    if (!SimilarityCalculator)
    {
        AddError(TEXT("Failed to create UWPWordSimilarityCalculator instance."));
        return false;
    }

    // 2. fasttext가 설치되어 있는지 확인
    // SimilarityCalculator->EnsureFastTextInstalled();

    // 3. 실제 유사도 테스트
    FString Word1 = TEXT("먹었고");
    FString Word2 = TEXT("먹었는데");

    float SimilarityScore = SimilarityCalculator->RunFastTextSimilarity(Word1, Word2);

    // 4. 유사도 결과값 검증 (0.0~1.0 범위)
    if (SimilarityScore < -1.0f || SimilarityScore > 1.0f)
    {
        AddError(FString::Printf(TEXT("Invalid similarity score: %f"), SimilarityScore));
        return false;
    }

    // 5. 정상 동작 확인 로그
    AddInfo(FString::Printf(TEXT("Similarity score between '%s' and '%s' is: %f"), *Word1, *Word2, SimilarityScore));

    return true; // 테스트 성공
}
