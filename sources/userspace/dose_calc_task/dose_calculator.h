class CDose_Calculator final
{
private:
    const float mKp = 2.0;
    const float mKd = 10.0;
    const float mMAX_DOSE = 5.0;

    float mLast_err = 0.0;
    float mTarget_val = 5.5;

public:
    CDose_Calculator() = default;
    ~CDose_Calculator() = default;

    float Calculate_Dose(float actual_val);
};