#include "LAMViseme.h"
#include "LAMVisemeTemplates.inl"

namespace
{
constexpr int32 FitDimension = LAM::FittedVisemeCount;
constexpr double Regularization = 1.e-4;
template <SIZE_T Count> LAM::FVisemeBasis MakeBasis(const FTemplateEntry (&Entries)[Count])
{
    LAM::FVisemeBasis B;
    for (const auto &E : Entries)
    {
        const int32 Row = LAM::CurveNames().IndexOfByKey(FName(E.Curve));
        check(Row != INDEX_NONE);
        B.Values[Row][static_cast<uint8>(E.Slot) - 1] = E.Value;
    }
    return B;
}
// Euclidean projection onto {v >= 0, sum(v) <= 1}; fixed-size scratch only.
void Project(const double *Input, double *Output)
{
    double Sum = 0, Sorted[FitDimension];
    for (int32 I = 0; I < FitDimension; ++I)
    {
        Sorted[I] = Input[I];
        Output[I] = FMath::Max(0.0, Input[I]);
        Sum += Output[I];
    }
    if (Sum <= 1)
        return;
    for (int32 I = 1; I < FitDimension; ++I)
    {
        const double Value = Sorted[I];
        int32 J = I;
        while (J > 0 && Sorted[J - 1] < Value)
        {
            Sorted[J] = Sorted[J - 1];
            --J;
        }
        Sorted[J] = Value;
    }
    double Prefix = 0, Threshold = 0;
    for (int32 I = 0; I < FitDimension; ++I)
    {
        Prefix += Sorted[I];
        const double Candidate = (Prefix - 1) / (I + 1);
        if (Sorted[I] > Candidate)
            Threshold = Candidate;
    }
    for (int32 I = 0; I < FitDimension; ++I)
        Output[I] = FMath::Max(0.0, Input[I] - Threshold);
}
} // namespace

const LAM::FVisemeBasis &LAM::VisemeBasis(ELAMVisemeTemplate Template)
{
    static const FVisemeBasis OpenFaceFX = MakeBasis(OpenFaceFXEntries);
    static const FVisemeBasis TalkingHead = MakeBasis(TalkingHeadEntries);
    return Template == ELAMVisemeTemplate::TalkingHead ? TalkingHead : OpenFaceFX;
}
LAM::FPreparedVisemeSettings LAM::PrepareVisemeSettings(const FLAMVisemeSettings &Settings)
{
    FPreparedVisemeSettings P;
    P.Settings = Settings;
    TArray<FString> Errors;
    P.bValid = ValidateVisemeSettings(Settings, Errors);
    if (!P.bValid || Settings.ConversionMode != ELAMVisemeConversionMode::TemplateFit)
        return P;
    for (double &Q : P.ObservationWeights)
        Q = 1;
    for (const auto &C : Settings.InputCorrections)
        P.ObservationWeights[CurveNames().IndexOfByKey(C.SourceName)] = C.FitWeight;
    const auto &B = VisemeBasis(Settings.Template);
    P.Lipschitz = 0;
    for (int32 J = 0; J < FitDimension; ++J)
    {
        double RowSum = 0;
        for (int32 K = 0; K < FitDimension; ++K)
        {
            double Value = J == K ? Regularization : 0;
            for (int32 I = 0; I < CurveCount; ++I)
                Value += P.ObservationWeights[I] * B.Values[I][J] * B.Values[I][K];
            P.H[J][K] = Value;
            RowSum += FMath::Abs(Value);
        }
        P.Lipschitz = FMath::Max(P.Lipschitz, RowSum);
    }
    return P;
}
namespace LAM
{
FLAMVisemeFrame FitLAMVisemeTemplate(const FLAMExpressionFrame &Frame, const float *Input,
                                   const FPreparedVisemeSettings &P)
{
    const auto &B = VisemeBasis(P.Settings.Template);
    double C[FitDimension] = {}, V[FitDimension] = {}, Y[FitDimension] = {}, Z[FitDimension], Step[FitDimension], Residual[FitDimension];
    for (int32 J = 0; J < FitDimension; ++J)
        for (int32 I = 0; I < CurveCount; ++I)
            C[J] += P.ObservationWeights[I] * B.Values[I][J] * Input[I];
    double T = 1;
    for (int32 Iteration = 0; Iteration < 1024; ++Iteration)
    {
        for (int32 J = 0; J < FitDimension; ++J)
        {
            double Gradient = -C[J];
            for (int32 K = 0; K < FitDimension; ++K)
                Gradient += P.H[J][K] * Y[K];
            Step[J] = Y[J] - Gradient / P.Lipschitz;
        }
        Project(Step, Z);
        const double NextT = (1 + FMath::Sqrt(1 + 4 * T * T)) * .5;
        for (int32 J = 0; J < FitDimension; ++J)
        {
            Y[J] = Z[J] + (T - 1) / NextT * (Z[J] - V[J]);
            V[J] = Z[J];
        }
        T = NextT;
        // Check the projected gradient at the feasible iterate, not the extrapolation.
        for (int32 J = 0; J < FitDimension; ++J)
        {
            double Gradient = -C[J];
            for (int32 K = 0; K < FitDimension; ++K)
                Gradient += P.H[J][K] * V[K];
            Step[J] = V[J] - Gradient / P.Lipschitz;
        }
        Project(Step, Residual);
        double Error = 0;
        for (int32 J = 0; J < FitDimension; ++J)
            Error = FMath::Max(Error, P.Lipschitz * FMath::Abs(V[J] - Residual[J]));
        if (Error <= 1.e-8)
            break;
    }
    if (P.Settings.Template == ELAMVisemeTemplate::TalkingHead)
        V[5] = V[8] = (V[5] + V[8]) * .5; // CH/RR have identical columns.
    double Sum = 0;
    for (int32 J = 0; J < FitDimension; ++J)
    {
        const float *Gain = P.Settings.VisemeGains.Find(static_cast<ELAMViseme>(J + 1));
        V[J] *= Gain ? *Gain : 1.f;
        Sum += V[J];
    }
    FLAMVisemeFrame Result;
    double Total = 0;
    for (int32 J = 0; J < FitDimension; ++J)
    {
        Result.Values[J + 1] = float(V[J] / FMath::Max(1.0, Sum));
        Total += Result.Values[J + 1];
    }
    Result.Values[0] = float(FMath::Clamp(1.0 - Total, 0.0, 1.0));
    Result.TimeSeconds = Frame.TimeSeconds;
    Result.Weight = FMath::Clamp(Frame.Weight, 0.f, 1.f);
    Result.bValid = true;
    return Result;
}
} // namespace LAM
