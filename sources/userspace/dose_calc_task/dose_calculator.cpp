#include "dose_calculator.h"

float CDose_Calculator::Calculate_Dose(float actual_val)
{
    float error = actual_val - mTarget_val;

    float p = mKp * error;

    float d = mKd * (error - mLast_err);

    float active_intervention = p + d;

    if (active_intervention < 0)
    {
        active_intervention = mMAX_DOSE;
    }

    if (active_intervention > mMAX_DOSE)
    {
        active_intervention = mMAX_DOSE;
    }

    mLast_err = error;

    return active_intervention;
}