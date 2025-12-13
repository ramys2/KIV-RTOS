class CDose_Calculator final
{
private:
    const float mKp = 2.0f;
    const float mKd = 10.0f;
    const float mMAX_DOSE = 5.0f;

    float mLast_err = 0.0f;
    float mTarget_val = 5.5f;

public:
    CDose_Calculator() = default;
    ~CDose_Calculator() = default;

    float Calculate_Dose(float actual_val);
};