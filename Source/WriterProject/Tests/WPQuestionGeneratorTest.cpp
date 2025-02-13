#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "WriterProject/QuestionGenerator/WPQuestionGenerator.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWPQuestionGeneratorTest,
    "Project.Tests.WPQuestionGeneratorTest",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

    bool FWPQuestionGeneratorTest::RunTest(const FString& Parameters)
{
    // 1. 객체 생성
    UWPQuestionGenerator* Generator = NewObject<UWPQuestionGenerator>();
    TestNotNull("Generator should be valid", Generator);

    // 2. 폴더 경로 설정 (테스트용 텍스트 파일이 있는 경로)
    FString TestFolderPath = FPaths::ProjectDir() / TEXT("TestFiles");

    // 3. 텍스트 파일 선택 테스트
    Generator->GetTxtFileFromFolder(TestFolderPath);
    TestTrue("SelectedFilePath should not be empty", !Generator->GetSelectedFilePath().IsEmpty());

    // 4. 파일 로드 테스트
    // Generator->LoadTxtFromPath();
    // TestTrue("FileLines should not be empty after loading", !Generator->FileLines.IsEmpty());

    // 5. 특정 줄 읽기 테스트
    FString FirstLine = Generator->ReadNthLineFromFile(1);
    TestTrue("First line should not be empty", !FirstLine.IsEmpty());

    // 6. 문장 유효성 검사 테스트
    TestTrue("Valid sentence check (한글)", Generator->IsValidSentence(TEXT("안녕하세요!")));
    TestTrue("Valid sentence check (영어)", Generator->IsValidSentence(TEXT("Hello, world!")));
    TestFalse("Invalid sentence check (@ 포함)", Generator->IsValidSentence(TEXT("Invalid@Sentence")));

    return true;
}
