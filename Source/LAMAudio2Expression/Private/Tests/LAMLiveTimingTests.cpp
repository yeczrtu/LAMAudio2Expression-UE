#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LAMLiveTiming.h"
#include "LAMAudio2ExpressionComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMLiveTimingTest, "LAM.Live.IntervalAndDelay",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMLiveTimingTest::RunTest(const FString&)
{
    TestEqual(TEXT("Default 333ms"), LAM::LiveHop(333.333f), 10);
    TestEqual(TEXT("100ms"), LAM::LiveHop(100), 3);
    TestEqual(TEXT("Minimum"), LAM::LiveHop(-10), 1);
    TestEqual(TEXT("Maximum"), LAM::LiveHop(2000), 30);
    for (int H : {1, 3, 10, 15, 30})
    {
        int64 Last = 0;
        for (int I = 1; I <= 2000; ++I)
        {
            const int64 Frame = int64(I) * H;
            const int64 Samples = (Frame * 16000 + 29) / 30;
            const int64 Next = LAM::NextLiveFrame(Samples, Last, H);
            TestEqual(TEXT("No missing or duplicate frames"), Next, Frame);
            TestTrue(TEXT("Window end within one sample"), Samples - Next * 16000 / 30 <= 1);
            Last = Next;
        }
    }
    TestEqual(TEXT("Change 10 to 3 retains boundary"), LAM::NextLiveFrame(7000, 10, 3), int64(13));
    TestEqual(TEXT("Change 3 to 30 waits for input"), LAM::NextLiveFrame(16000, 13, 30), int64(13));
    TestEqual(TEXT("Catch up skips old hops"), LAM::NextLiveFrame(160000, 10, 3), int64(298));
    TestEqual(TEXT("Long integer timeline"), LAM::NextLiveFrame(int64(16000)*3600*48, 0, 10), int64(30)*3600*48);
    TestEqual(TEXT("Requested delay lower bound"), LAM::LiveDelay(.75, 0, 100, 25), 750.f);
    TestTrue(TEXT("Long hop increases delay"), LAM::LiveDelay(.75, 750, 1000, 50) > 1083);
    TestEqual(TEXT("Delay never shrinks in session"), LAM::LiveDelay(.4f, 1400, 33, 10), 1400.f);
    TestEqual(TEXT("Delay cap"), LAM::LiveDelay(.75, 750, 1000, 2000), 2000.f);
    TArray<float> Samples; for (int I=1; I<=100; ++I) Samples.Add(float(I));
    TestEqual(TEXT("95th percentile"), LAM::LiveP95(Samples), 95.f);
    auto* C = NewObject<ULAMAudio2ExpressionComponent>();
    C->SetLiveInferenceInterval(111);
    TestEqual(TEXT("BP reports quantized interval"), C->GetLiveInferenceInterval(), 100.f);
    TestFalse(TEXT("Reject negative volume"), C->SetVolume(-1));
    TestFalse(TEXT("Reject null submix"), C->SetSubmixSend(nullptr, 1));
    TestFalse(TEXT("Cannot fade without playback"), C->FadeOutAndStop(.1f));
    return true;
}
#endif
