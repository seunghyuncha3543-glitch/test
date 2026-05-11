using RacingGame.Vehicle;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace RacingGame.UI
{
    public class HUDController : MonoBehaviour
    {
        [SerializeField] VehicleController vehicle;
        [SerializeField] TMP_Text speedText;
        [SerializeField] TMP_Text gearText;
        [SerializeField] Slider rpmSlider;
        [SerializeField] TMP_Text absText;
        [SerializeField] TMP_Text tcsText;

        [SerializeField] float idleRpm = 900f;
        [SerializeField] float maxRpm = 7800f;

        void Update()
        {
            if (!vehicle) return;
            speedText.text = $"{vehicle.SpeedKmh:0} km/h";
            gearText.text = $"G{vehicle.CurrentGear}";
            rpmSlider.value = Mathf.InverseLerp(idleRpm, maxRpm, vehicle.EngineRpm);
            absText.text = vehicle.ABSActive ? "ABS ON" : "ABS";
            tcsText.text = vehicle.TCSActive ? "TCS ON" : "TCS";
        }
    }
}
