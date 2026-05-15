using RacingGame.Vehicle;
using UnityEngine;

namespace RacingGame.CameraSystem
{
    public class RaceCameraController : MonoBehaviour
    {
        [SerializeField] Transform target;
        [SerializeField] VehicleController vehicle;
        [SerializeField] Vector3 offset = new Vector3(0f, 2.3f, -6.5f);
        [SerializeField] float smooth = 8f;
        [SerializeField] float minFov = 62f;
        [SerializeField] float maxFov = 84f;
        [SerializeField] float fovAtKmh = 300f;
        [SerializeField] float accelKick = 0.45f;
        [SerializeField] float brakeKick = 0.35f;

        Camera cam;
        float speedPrev;

        void Awake()
        {
            cam = GetComponent<Camera>();
        }

        void LateUpdate()
        {
            if (!target || !vehicle) return;
            float speed = vehicle.SpeedKmh;
            float accel = (speed - speedPrev) / Mathf.Max(Time.deltaTime, 0.001f);
            speedPrev = speed;

            float kick = accel > 0 ? -accelKick : brakeKick;
            Vector3 dyn = offset + new Vector3(0f, 0f, kick * Mathf.Clamp01(Mathf.Abs(accel) / 25f));
            Vector3 desired = target.TransformPoint(dyn);
            transform.position = Vector3.Lerp(transform.position, desired, smooth * Time.deltaTime);

            Vector3 lookPos = target.position + target.forward * 6f;
            transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(lookPos - transform.position), smooth * Time.deltaTime);

            cam.fieldOfView = Mathf.Lerp(minFov, maxFov, Mathf.Clamp01(speed / fovAtKmh));
        }
    }
}
