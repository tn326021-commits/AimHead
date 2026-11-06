using UnityEngine;

namespace Game.Config
{
    [CreateAssetMenu(fileName = "AimHeadConfig_Max", menuName = "Game/AimHead/Config")]
    public class AimHeadConfig : ScriptableObject
    {
        [Header("1. Detection (Phát hiện)")]
        [Range(1f, 180f)] public float MaxAngleDeg = 180f; 
        [Range(0.1f, 1000f)] public float MaxDistance = 500f;
        
        [Tooltip("Khoảng cách tối thiểu để bắt đầu khóa (tránh khóa mục tiêu quá gần gây giật)")]
        public float MinDistance = 1.5f;

        [Header("2. Smoothing (Độ mượt)")]
        [Range(0.01f, 1f)] public float Smoothing = 0.01f;
        [Range(0f, 1f)] public float SnapWeight = 1.0f;
        
        [Tooltip("Hệ số giảm tốc khi gần đạt mục tiêu (tránh overshoot)")]
        [Range(0.1f, 1f)] public float DampingFactor = 0.85f;

        [Header("3. Prediction (Dự đoán)")]
        public bool EnablePrediction = true;
        [Range(0f, 1f)] public float PredictionTime = 0.5f;

        [Header("4. Bone Target")]
        public string TargetBoneName = "Head";
        public Vector3 BoneOffset = new Vector3(0f, 0f, 0f);
        
        [Tooltip("Tốc độ xoay tối đa của Camera/Súng (độ/giây)")]
        public float MaxRotationSpeed = 720f;

        [Header("5. Priority")]
        [Range(0, 10)] public int Priority = 10;
        
        [Header("6. Encryption")]
        public bool EncryptOnBuild = true;

        // ==========================================
        // CÁC HÀM TÍNH TOÁN ĐỘNG (DYNAMIC FUNCTIONS)
        // ==========================================

        /// <summary>
        /// Kiểm tra xem mục tiêu có nằm trong vùng khóa (FOV & Distance) không.
        /// </summary>
        public bool IsTargetValid(Vector3 camPos, Vector3 camForward, Vector3 targetPos)
        {
            Vector3 dirToTarget = targetPos - camPos;
            float dist = dirToTarget.magnitude;

            // Check khoảng cách (Min & Max)
            if (dist < MinDistance || dist > MaxDistance) return false;

            // Check góc (FOV)
            float angle = Vector3.Angle(camForward, dirToTarget);
            return angle <= (MaxAngleDeg / 2f); // Chia 2 vì FOV tính từ tâm ra
        }

        /// <summary>
        /// Tính toán vị trí ngắm bắn dự đoán (Lead Target) dựa trên vận tốc mục tiêu.
        /// </summary>
        public Vector3 GetPredictedPosition(Vector3 targetPos, Vector3 targetVelocity, float bulletSpeed)
        {
            if (!EnablePrediction || bulletSpeed <= 0f) return targetPos;

            Vector3 dir = targetPos - Camera.main.transform.position;
            float dist = dir.magnitude;
            
            // Thời gian đạn bay tới mục tiêu
            float timeToTarget = dist / bulletSpeed;
            
            // Kết hợp thời gian dự đoán cấu hình + thời gian đạn bay
            float totalPredictionTime = timeToTarget + PredictionTime;

            // Trả về vị trí tương lai
            return targetPos + (targetVelocity * totalPredictionTime);
        }

        /// <summary>
        /// Tính toán góc xoay mượt mà (Slerp) áp dụng Smoothing và Damping.
        /// </summary>
        public Quaternion GetSmoothRotation(Quaternion currentRot, Quaternion targetRot, float deltaTime)
        {
            // Tốc độ xoay dựa trên Smoothing (Smoothing nhỏ -> xoay nhanh)
            float speed = Mathf.Lerp(1f, 30f, 1f - Smoothing);
            
            // Áp dụng MaxRotationSpeed để không bị quá nhanh (như hack)
            float finalSpeed = Mathf.Min(speed, MaxRotationSpeed);
            
            // Slerp có damping
            return Quaternion.Slerp(currentRot, targetRot, finalSpeed * DampingFactor * deltaTime);
        }
    }
}
