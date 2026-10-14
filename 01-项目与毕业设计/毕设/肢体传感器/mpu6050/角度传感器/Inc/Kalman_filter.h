#ifndef Kalman_filter
#define Kalman_filter

typedef struct {
    double x;  // ä¼°è?¡å€?
    double p;  // ä¼°è?¡è??å·?åæ–¹å·?
    double q;  // è¿‡ç¨‹å™?å£°æ–¹å·?
    double r;  // æµ‹é‡å™?å£°æ–¹å·?
} KalmanFilter;

void KalmanFilter_Init(KalmanFilter* kf, double initial_value, double process_noise, double measurement_noise);
double KalmanFilter_Update(KalmanFilter* kf, double measurement);

#endif
