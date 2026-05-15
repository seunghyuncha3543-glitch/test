using UnityEngine;

namespace RacingGame.Game
{
    public class SurfaceGripMap : MonoBehaviour
    {
        [SerializeField] string defaultTag = "Road";

        public string GetSurfaceTag(WheelHit hit)
        {
            if (hit.collider == null) return defaultTag;
            return string.IsNullOrWhiteSpace(hit.collider.tag) ? defaultTag : hit.collider.tag;
        }
    }
}
