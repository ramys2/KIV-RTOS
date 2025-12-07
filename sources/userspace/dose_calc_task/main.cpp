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

    // Glucose receiver communication primitives
    uint32_t dose_gluc_pipe = pipe("dose_gluc_pipe", 64);
    semaphore_t dose_gluc_sem = sem_create("dose_gluc_sem#1");
    semaphore_t gluc_dose_sem = sem_create("gluc_dose_sem#1");

    // Display communication primitives

    sem_acquire(dose_gluc_sem, 1);

    while(true)
    {
        sem_release(gluc_dose_sem, 1);
        sem_acquire(dose_gluc_sem, 1);

        char data[4];
        uint32_t read_bytes = read(dose_gluc_pipe, data, sizeof(data));

        if (read_bytes == sizeof(data))
        {
            float glucose_value = *reinterpret_cast<float*>(data);
            float dose = calc.Calculate_Dose(glucose_value);
        }
    }
}
