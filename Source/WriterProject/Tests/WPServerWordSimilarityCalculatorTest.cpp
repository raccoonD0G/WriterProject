#include "WriterProject/WPWordSimilarityCalculator.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

DEFINE_LOG_CATEGORY_STATIC(LogWordSimilarityTest, Log, All);

// 비동기 처리를 위한 Latent Command (수정됨)
class FWaitForSimilarityResponse : public IAutomationLatentCommand
{
public:
    FWaitForSimilarityResponse(TSharedPtr<bool> bCompleted, TSharedPtr<float> SimilarityScore, FAutomationTestBase* Test)
        : bTestCompleted(bCompleted), ReceivedSimilarityScore(SimilarityScore), TestInstance(Test) {}

    virtual bool Update() override
    {
        if (*bTestCompleted)
        {
            // 서버 응답이 왔으면 테스트 실행
            const bool bIsScoreValid = (*ReceivedSimilarityScore >= -1.0f && *ReceivedSimilarityScore <= 1.0f);
            TestInstance->TestTrue(TEXT("Similarity score should be between -1.0 and 1.0"), bIsScoreValid);

            // 테스트 기록에 점수 출력
            FString InfoMessage = FString::Printf(TEXT("Received similarity score: %f"), *ReceivedSimilarityScore);
            TestInstance->AddInfo(InfoMessage);

            // 로그에도 출력
            UE_LOG(LogWordSimilarityTest, Log, TEXT("Word similarity test passed! %s"), *InfoMessage);
            return true; // Latent Command 종료
        }
        return false; // 아직 완료되지 않음
    }

private:
    TSharedPtr<bool> bTestCompleted;
    TSharedPtr<float> ReceivedSimilarityScore;
    FAutomationTestBase* TestInstance;
};

// 테스트 클래스 정의
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWordSimilarityTest,
    "Project.Tests.WPServerWordSimilarityCalculator",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    bool FWordSimilarityTest::RunTest(const FString& Parameters)
{
    // 유사도 계산기 객체 생성
    UWPWordSimilarityCalculator* SimilarityCalculator = NewObject<UWPWordSimilarityCalculator>();

    if (!SimilarityCalculator)
    {
        AddError(TEXT("Failed to create UWPWordSimilarityCalculator instance"));
        return false;
    }

    // 테스트할 단어 설정
    FString TestWord1 = TEXT("먹었다");
    FString TestWord2 = TEXT("먹었고");

    // 비동기 테스트를 위한 변수
    TSharedPtr<float> ReceivedSimilarityScore = MakeShareable(new float(-2.0f));
    TSharedPtr<bool> bTestCompleted = MakeShareable(new bool(false));

    // 서버에 요청
    SimilarityCalculator->RunServerFastTextSimilarity(TestWord1, TestWord2,
        [ReceivedSimilarityScore, bTestCompleted](float SimilarityScore)
        {
            *ReceivedSimilarityScore = SimilarityScore;
            *bTestCompleted = true;
        });

    // 비동기 대기 및 응답 후 테스트 실행
    ADD_LATENT_AUTOMATION_COMMAND(FWaitForSimilarityResponse(bTestCompleted, ReceivedSimilarityScore, this));

    return true;
}
