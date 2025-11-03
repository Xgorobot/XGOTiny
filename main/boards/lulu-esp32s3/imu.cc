#include "imu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include <cstdio>
#include <cmath>

#define TAG "ICM42670P"

// ---------------------- I2C配置 ----------------------
#define ICM_ADDR        0x69
#define SDA_PIN         GPIO_NUM_48
#define SCL_PIN         GPIO_NUM_14
#define I2C_FREQ_HZ     400000

// ---------------------- 寄存器 ----------------------
#define REG_WHO_AM_I        0x75
#define REG_BANK_SEL        0x76
#define REG_PWR_MGMT0       0x1F
#define REG_GYRO_CONFIG0    0x4F
#define REG_ACCEL_CONFIG0   0x50
#define REG_INT_STATUS      0x2D
#define REG_ACCEL_DATA_X1_H 0x0B  // 加速度计数据起始地址
#define REG_GYRO_DATA_X1_H  0x17  // 陀螺仪数据起始地址
#define REG_TEMP_DATA1      0x1D  // 温度数据

#define WHOAMI_EXPECTED     0x67  // ICM42670P datasheet

// ---------------------- 句柄 ----------------------
static i2c_master_bus_handle_t bus_handle = nullptr;
static i2c_master_dev_handle_t dev_handle = nullptr;

// ---------------------- 工具函数 ----------------------
static void write_reg(uint8_t reg, uint8_t val) {
    uint8_t data[2] = {reg, val};
    i2c_master_transmit(dev_handle, data, 2, pdMS_TO_TICKS(20));
}

static void read_regs(uint8_t reg, uint8_t *buf, size_t len) {
    i2c_master_transmit_receive(dev_handle, &reg, 1, buf, len, pdMS_TO_TICKS(20));
}
float roll = 0.0;
float pitch = 0.0;
float yaw = 0.0;

// ---------------------- 初始化 ----------------------
extern "C" void imu_init(void) {
    // 创建I2C总线
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .flags = {.enable_internal_pullup = true},
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    // 探测设备
    for (int addr = 0x03; addr <= 0x77; addr++) {
        if (i2c_master_probe(bus_handle, addr, pdMS_TO_TICKS(10)) == ESP_OK)
            printf("Found I2C device at 0x%02X\n", addr);
    }

    // 添加IMU设备
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ICM_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    vTaskDelay(pdMS_TO_TICKS(100));

    // 选择Bank0
    write_reg(REG_BANK_SEL, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));

    // 唤醒陀螺仪与加速度计
    write_reg(REG_PWR_MGMT0, 0x0F);
    vTaskDelay(pdMS_TO_TICKS(50));

    // 配置陀螺仪 ±2000dps, ODR=1kHz
    write_reg(REG_GYRO_CONFIG0, 0x06);
    // 配置加速度 ±8g, ODR=1kHz
    write_reg(REG_ACCEL_CONFIG0, 0x06);

    // 校验WHO_AM_I
    uint8_t id = 0;
    read_regs(REG_WHO_AM_I, &id, 1);
    printf("WHO_AM_I = 0x%02X\n", id);
    if (id != WHOAMI_EXPECTED)
        printf("Warning: unexpected WHO_AM_I value! Expected 0x%02X\n", WHOAMI_EXPECTED);

    printf("ICM42670P initialized.\n");
}

// ---------------------- 读取一次数据 ----------------------
extern "C" void imu_read_once(void) {
    if (!dev_handle) {
        printf("IMU not initialized!\n");
        return;
    }
    // 确保在Bank0
    write_reg(REG_BANK_SEL, 0x00);
    
    // 检查数据是否就绪 (INT_STATUS寄存器的bit0 = DATA_RDY_INT)
    uint8_t int_status = 0;
    read_regs(REG_INT_STATUS, &int_status, 1);
    
    // 如果数据未就绪，等待一小段时间后重试
    if (!(int_status & 0x01)) {
        vTaskDelay(pdMS_TO_TICKS(1));
        read_regs(REG_INT_STATUS, &int_status, 1);
    }

    // 从0x11连续读取12字节：加速度计(6字节) + 陀螺仪(6字节)
    // ICM42670P数据格式：高字节在前（大端序）
    uint8_t raw[12] = {0};
    read_regs(REG_ACCEL_DATA_X1_H, raw, 12);

    // 解析数据 (高字节在前，大端序)
    // 加速度计数据 (前6字节: X_H, X_L, Y_H, Y_L, Z_H, Z_L)
    int16_t ax = (int16_t)((raw[0] << 8) | raw[1]);
    int16_t ay = (int16_t)((raw[2] << 8) | raw[3]);
    int16_t az = (int16_t)((raw[4] << 8) | raw[5]);
    
    // 陀螺仪数据 (后6字节: X_H, X_L, Y_H, Y_L, Z_H, Z_L)
    int16_t gx = (int16_t)((raw[6] << 8) | raw[7]);
    int16_t gy = (int16_t)((raw[8] << 8) | raw[9]);
    int16_t gz = (int16_t)((raw[10] << 8) | raw[11]);
    
    // 调试：打印原始16位数据
    // printf("Raw: AX=%d AY=%d AZ=%d GX=%d GY=%d GZ=%d\n", ax, ay, az, gx, gy, gz);

    // 转换为物理单位
    float acc[3] = {
        ax / 32768.0f * 9.80665f * 16.0f,
        ay / 32768.0f * 9.80665f * 16.0f,
        az / 32768.0f * 9.80665f * 16.0f
    };
    // 陀螺仪: ±2000dps量程, 32768对应满量程
    float gyro[3] = {
        gx / 32768.0f * 2000.0f,
        gy / 32768.0f * 2000.0f,
        gz / 32768.0f * 2000.0f
    };
    pitch = -atan2(acc[1], acc[2])*180.0/3.1415;
    roll = atan2(acc[0], acc[2])*180.0/3.1415;
    // if(abs(gyro[2])>3.0){              //如果要对yaw估计需要持续运行该函数积分，暂不处理
    //     yaw = yaw + gyro[2] * 0.005*1.8;
    // }   
    // printf("Acc [m/s^2]: X=%5.1f Y=%5.1f Z=%5.1f | Gyro [dps]: X=%5.1f Y=%5.1f Z=%5.1f\n",
    //        acc[0], acc[1], acc[2], gyro[0], gyro[1], gyro[2]);
    printf("Pitch=%5.1f Roll=%5.1f Yaw=%5.1f\n", pitch, roll, yaw);
}
