using RacingGame.Game;
using UnityEngine;

namespace RacingGame.Vehicle
{
    public class VehicleController : MonoBehaviour
    {
        public enum Drivetrain { AWD, RWD }

        [Header("Refs")]
        [SerializeField] Rigidbody rb;
        [SerializeField] WheelCollider fl;
        [SerializeField] WheelCollider fr;
        [SerializeField] WheelCollider rl;
        [SerializeField] WheelCollider rr;
        [SerializeField] SurfaceGripMap surfaceMap;

        [Header("Vehicle")]
        [SerializeField] float massKg = 1650f;
        [SerializeField] float maxBrakeTorque = 5200f;
        [SerializeField] float brakeBiasFront = 0.62f;
        [SerializeField] float steerAngle = 35f;
        [SerializeField] float dragCoefficient = 0.30f;
        [SerializeField] float rollingResistance = 14.5f;
        [SerializeField] float downforceCoefficient = 26f;
        [SerializeField] Drivetrain drivetrain = Drivetrain.AWD;
        [SerializeField] bool absEnabled = true;
        [SerializeField] bool tcsEnabled = true;

        [Header("Systems")]
        [SerializeField] Powertrain powertrain = new Powertrain();
        [SerializeField] WheelFrictionModel frictionModel = new WheelFrictionModel();

        public float SpeedKmh => rb.velocity.magnitude * 3.6f;
        public float EngineRpm { get; private set; }
        public int CurrentGear => powertrain.CurrentGear;
        public bool ABSActive { get; private set; }
        public bool TCSActive { get; private set; }
        public bool SportMode { get; private set; }

        float throttleInput;
        float brakeInput;
        float steerInput;
        bool handbrake;

        void Awake()
        {
            if (!rb) rb = GetComponent<Rigidbody>();
            rb.mass = massKg;
            powertrain.Reset();
        }

        void Update()
        {
            throttleInput = Mathf.Clamp01(Input.GetAxis("Vertical"));
            float rawBrake = -Mathf.Min(0f, Input.GetAxis("Vertical"));
            brakeInput = Mathf.Clamp01(rawBrake);
            steerInput = Input.GetAxis("Horizontal");
            handbrake = Input.GetKey(KeyCode.Space);
            SportMode = Input.GetKey(KeyCode.LeftShift);
        }

        void FixedUpdate()
        {
            ApplySteer();
            EstimateEngineRpm();
            powertrain.Tick(EngineRpm, throttleInput, SportMode, Time.fixedDeltaTime);
            ApplyDriveAndBrake();
            ApplyAeroAndResistance();
        }

        void ApplySteer()
        {
            float angle = steerInput * steerAngle;
            fl.steerAngle = angle;
            fr.steerAngle = angle;
        }

        void EstimateEngineRpm()
        {
            float avgWheelRpm = Mathf.Abs(rl.rpm + rr.rpm) * 0.5f;
            float ratio = Mathf.Abs(powertrain.GetCurrentRatio(false));
            EngineRpm = Mathf.Clamp(avgWheelRpm * ratio, 900f, 7800f);
        }

        void ApplyDriveAndBrake()
        {
            ABSActive = false;
            TCSActive = false;
            bool reversing = brakeInput > 0.1f && SpeedKmh < 3f;
            float torque = powertrain.GetDriveTorque(EngineRpm, throttleInput, reversing);

            if (tcsEnabled && throttleInput > 0.2f)
            {
                float slip = GetRearSlip();
                if (slip > frictionModel.tcsSlipThreshold)
                {
                    torque *= 0.55f;
                    TCSActive = true;
                }
            }

            ApplyMotorTorque(torque);
            ApplyBrake(brakeInput);
            ApplySurfaceFriction();
        }

        float GetRearSlip()
        {
            WheelHit hit;
            float slip = 0f;
            int count = 0;
            if (rl.GetGroundHit(out hit)) { slip += Mathf.Abs(hit.forwardSlip); count++; }
            if (rr.GetGroundHit(out hit)) { slip += Mathf.Abs(hit.forwardSlip); count++; }
            return count == 0 ? 0f : slip / count;
        }

        void ApplyMotorTorque(float torque)
        {
            if (drivetrain == Drivetrain.AWD)
            {
                fl.motorTorque = torque * 0.25f; fr.motorTorque = torque * 0.25f;
                rl.motorTorque = torque * 0.25f; rr.motorTorque = torque * 0.25f;
            }
            else
            {
                fl.motorTorque = 0f; fr.motorTorque = 0f;
                rl.motorTorque = torque * 0.5f; rr.motorTorque = torque * 0.5f;
            }
        }

        void ApplyBrake(float input)
        {
            float totalBrake = input * maxBrakeTorque;
            float front = totalBrake * brakeBiasFront;
            float rear = totalBrake * (1f - brakeBiasFront);

            if (handbrake) rear += maxBrakeTorque * 0.7f;

            if (absEnabled && input > 0.2f && IsWheelLocking())
            {
                front *= 0.65f;
                rear *= 0.65f;
                ABSActive = true;
            }

            fl.brakeTorque = front;
            fr.brakeTorque = front;
            rl.brakeTorque = rear;
            rr.brakeTorque = rear;
        }

        bool IsWheelLocking()
        {
            WheelHit hit;
            if (fl.GetGroundHit(out hit) && Mathf.Abs(hit.forwardSlip) > frictionModel.absSlipThreshold) return true;
            if (fr.GetGroundHit(out hit) && Mathf.Abs(hit.forwardSlip) > frictionModel.absSlipThreshold) return true;
            return false;
        }

        void ApplyAeroAndResistance()
        {
            float speed = rb.velocity.magnitude;
            float drag = dragCoefficient * speed * speed;
            float rrForce = rollingResistance * speed;
            rb.AddForce(-rb.velocity.normalized * (drag + rrForce), ForceMode.Force);
            rb.AddForce(-transform.up * (downforceCoefficient * speed), ForceMode.Force);
        }

        void ApplySurfaceFriction()
        {
            ApplyWheelGrip(fl, false);
            ApplyWheelGrip(fr, false);
            ApplyWheelGrip(rl, handbrake);
            ApplyWheelGrip(rr, handbrake);
        }

        void ApplyWheelGrip(WheelCollider wheel, bool rearHandbrake)
        {
            WheelFrictionCurve side = wheel.sidewaysFriction;
            WheelHit hit;
            float grip = 1f;
            if (wheel.GetGroundHit(out hit)) grip = frictionModel.GetGrip(surfaceMap ? surfaceMap.GetSurfaceTag(hit) : "Road");
            if (rearHandbrake) grip *= frictionModel.handbrakeRearGripMultiplier;
            side.stiffness = grip;
            wheel.sidewaysFriction = side;
        }
    }
}
