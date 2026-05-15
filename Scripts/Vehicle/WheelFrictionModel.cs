using UnityEngine;

namespace RacingGame.Vehicle
{
    [System.Serializable]
    public class WheelFrictionModel
    {
        public float asphaltGrip = 1f;
        public float dirtGrip = 0.7f;
        public float wetGrip = 0.82f;
        public float handbrakeRearGripMultiplier = 0.55f;
        public float tcsSlipThreshold = 0.22f;
        public float absSlipThreshold = 0.25f;

        public float GetGrip(string surfaceTag)
        {
            return surfaceTag switch
            {
                "Dirt" => dirtGrip,
                "Wet" => wetGrip,
                _ => asphaltGrip,
            };
        }
    }
}
