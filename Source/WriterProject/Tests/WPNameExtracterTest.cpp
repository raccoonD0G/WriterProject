#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "WriterProject/WPNameExtracter.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWPNameExtracterTest, "Project.Tests.WPNameExtracterTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWPNameExtracterTest::RunTest(const FString& Parameters)
{
    // 테스트할 입력 텍스트
    FString TestText = TEXT("John이 서울에서 살고 있으며 Alice와 함께 New York으로 갔다.");
    // FString TestText = TEXT("My name is Wolfgang and I live in Berlin");

    // 비동기 요청을 보낼 것이므로, 로그를 확인하여 결과를 검증
    UWPNameExtracter::ExtractNamedEntities(TestText);

    // Unreal Engine의 로깅 시스템을 활용하여 결과를 확인할 수 있음
    UE_LOG(LogTemp, Warning, TEXT("Sent request to ExtractNamedEntities with text: %s"), *TestText);

    // 비동기 요청이므로 직접적인 반환값이 없음. 성공적으로 실행될 경우 UE_LOG에서 확인 가능.
    return true;
}
