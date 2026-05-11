using UnityEngine;

namespace RacingGame.Vehicle
{
    [System.Serializable]
    public class Powertrain
    {
        public AnimationCurve torqueCurve = new AnimationCurve(
            new Keyframe(1000f, 420f),
            new Keyframe(3500f, 610f),
            new Keyframe(6500f, 530f),
            new Keyframe(7600f, 420f)
        );

        public float[] gearRatios = { 3.60f, 2.19f, 1.59f, 1.24f, 1.00f, 0.82f, 0.68f, 0.58f };
        public float reverseRatio = 3.25f;
        public float finalDrive = 3.10f;
        public float drivetrainEfficiency = 0.88f;
        public float shiftUpRpm = 6800f;
        public float shiftDownRpm = 2200f;
        public float shiftTime = 0.18f;

        public int CurrentGear { get; private set; } = 1;
        public bool IsShifting { get; private set; }

        float shiftTimer;

        public void Reset()
        {
            CurrentGear = 1;
            IsShifting = false;
            shiftTimer = 0f;
        }

        public void Tick(float rpm, float throttle, bool sportMode, float dt)
        {
            if (IsShifting)
            {
                shiftTimer -= dt;
                if (shiftTimer <= 0f) IsShifting = false;
                return;
            }

            float up = sportMode ? shiftUpRpm + 450f : shiftUpRpm;
            if (throttle > 0.2f && rpm >= up && CurrentGear < gearRatios.Length)
            {
                Shift(CurrentGear + 1);
            }
            else if (rpm <= shiftDownRpm && CurrentGear > 1)
            {
                Shift(CurrentGear - 1);
            }
        }

        void Shift(int target)
        {
            CurrentGear = Mathf.Clamp(target, 1, gearRatios.Length);
            IsShifting = true;
            shiftTimer = shiftTime;
        }

        public float GetCurrentRatio(bool reversing)
        {
            if (reversing) return -reverseRatio * finalDrive;
            return gearRatios[Mathf.Clamp(CurrentGear - 1, 0, gearRatios.Length - 1)] * finalDrive;
        }

        public float GetDriveTorque(float rpm, float throttle, bool reversing)
        {
            if (IsShifting) return 0f;
            float engineTorque = torqueCurve.Evaluate(Mathf.Clamp(rpm, 800f, 8000f));
            return throttle * engineTorque * GetCurrentRatio(reversing) * drivetrainEfficiency;
        }
    }
}
