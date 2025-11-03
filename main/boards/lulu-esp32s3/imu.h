#ifndef _IMU_H_
#define _IMU_H_

#ifdef __cplusplus
extern "C" {
#endif

void imu_init(void);
void imu_read_once(void);

extern float roll;
extern float pitch;
extern float yaw;

#ifdef __cplusplus
}
#endif

#endif
