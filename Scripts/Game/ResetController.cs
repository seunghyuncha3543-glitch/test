using RacingGame.Vehicle;
using UnityEngine;

namespace RacingGame.Game
{
    public class ResetController : MonoBehaviour
    {
        [SerializeField] VehicleController vehicle;
        [SerializeField] Rigidbody rb;
        [SerializeField] Transform resetPoint;

        void Awake()
        {
            if (!rb && vehicle) rb = vehicle.GetComponent<Rigidbody>();
        }

        void Update()
        {
            if (Input.GetKeyDown(KeyCode.R)) ResetVehicle();
        }

        public void ResetVehicle()
        {
            if (!vehicle || !rb || !resetPoint) return;
            rb.velocity = Vector3.zero;
            rb.angularVelocity = Vector3.zero;
            vehicle.transform.SetPositionAndRotation(resetPoint.position, resetPoint.rotation);
        }
    }
}
