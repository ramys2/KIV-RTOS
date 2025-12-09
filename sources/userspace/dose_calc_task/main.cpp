#include <stdfile.h>
#include <stdstring.h>
#include <stdmutex.h>

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

    float Calculate_Dose(float actual_val)
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
};

int main()
{
    CDose_Calculator calc;

    uint32_t led = open("DEV:gpio/20", NFile_Open_Mode::Write_Only);
    uint32_t gluc_dose_pipe = pipe("gluc_dose_pipe", sizeof(float));
    uint32_t dose_pat_pipe = pipe("dose_pat_pipe", sizeof(float));
    uint32_t dose_disp_pipe = pipe("dose_disp_pipe", sizeof(float));

    while(true)
    {
        char data[sizeof(float)];
        uint32_t read_bytes = read(gluc_dose_pipe, data, sizeof(data));
        write(led, "1", 1);

        float glucose = *reinterpret_cast<float *>(data);
        float dose = calc.Calculate_Dose(glucose);

        // Send to patient and display
        write(dose_pat_pipe, reinterpret_cast<char *>(&dose), sizeof(dose));
        write(dose_disp_pipe, reinterpret_cast<char *>(&dose), sizeof(dose));
    }

    close(gluc_dose_pipe);
    close(dose_pat_pipe);
    close(dose_disp_pipe);

    return 0;
}
