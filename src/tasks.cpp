#include <M5StickCPlus.h>
#include "tasks.h"
#include <utility/MPU6886.h>

static const char *TAG = "TASKS_UTIL";

namespace TASKS
{

    Dimmer::Dimmer()
    {
        dimmed = false;
        asleep = false;
    }

    void Dimmer::go_dim()
    {
        if (dimmed)
        {
            return;
        }
        brightness = M5.Axp.Read8bit(0x28) >> 4;
        dimmed = true;
        M5.Axp.ScreenBreath(7);
    }

    void Dimmer::go_dark()
    {
        dimmed = true;
        asleep = true;
        M5.Axp.Write1Byte(0x12, 0b01001011);
    }

    void Dimmer::recover()
    {
        if (!(dimmed || asleep)) 
        {
            return;
        }
        if (asleep)
        {
            M5.Axp.Write1Byte(0x12, 0x4d);
        }
        M5.Axp.ScreenBreath(brightness);
        dimmed = false;
        asleep = false;
        dim_exit_time = millis();
    }

    bool Dimmer::dim_exitting()
    {
        return millis() - dim_exit_time < 500;
    }

    bool Dimmer::is_dim()
    {
        return dimmed || asleep;
    }

    int8_t get_batt_percentage()
    {
        float volt = M5.Axp.GetBatVoltage();

        if (volt < 3.20)
            return -1;
        if (volt < 3.27)
            return 0;
        if (volt < 3.61)
            return 5;
        if (volt < 3.69)
            return 10;
        if (volt < 3.71)
            return 15;
        if (volt < 3.73)
            return 20;
        if (volt < 3.75)
            return 25;
        if (volt < 3.77)
            return 30;
        if (volt < 3.79)
            return 35;
        if (volt < 3.80)
            return 40;
        if (volt < 3.82)
            return 45;
        if (volt < 3.84)
            return 50;
        if (volt < 3.85)
            return 55;
        if (volt < 3.87)
            return 60;
        if (volt < 3.91)
            return 65;
        if (volt < 3.95)
            return 70;
        if (volt < 3.98)
            return 75;
        if (volt < 4.02)
            return 80;
        if (volt < 4.08)
            return 85;
        if (volt < 4.11)
            return 90;
        if (volt < 4.15)
            return 95;
        if (volt < 4.20)
            return 100;
        if (volt >= 4.20)
            return 101;
        return 0;
    }

    IMUManager::IMUManager()
    {
        M5.Imu.getAccelData(&aX, &aY, &aZ);
        M5.Imu.getAccelData(&next_aX, &next_aY, &next_aZ);
    }

    bool IMUManager::is_moved()
    {
        M5.Imu.getAccelData(&next_aX, &next_aY, &next_aZ);

        if (abs(next_aX - aX) > 0.5 || abs(next_aY - aY) > 0.5 || abs(next_aZ - aZ) > 0.5)
        {
            aX = next_aX;
            aY = next_aY;
            aZ = next_aZ;
            return true;
        }
        return false;
    }
};

enum Ascale {
        AFS_2G = 0,
        AFS_4G,
        AFS_8G,
        AFS_16G
      };


// void enableWakeOnMotion(Ascale ascale, uint8_t thresh_num_lsb) {
//     uint8_t regdata;
//     /* 5.1 WAKE-ON-MOTION INTERRUPT
//         The MPU-6886 provides motion detection capability. A qualifying motion sample is one where the high passed sample
//         from any axis has an absolute value exceeding a user-programmable threshold. The following steps explain how to
//         configure the Wake-on-Motion Interrupt.
//     */

//     /* Step 0: this isn't explicitly listed in the steps, but configuring the 
//        FSR or full-scale-range of the accelerometer is important to setting up
//        the accel/motion threshold in Step 4
//     */
//     regdata = (ascale << 3);
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG, 1, &regdata);
//     delay(10);

//     /* Step 1: Ensure that Accelerometer is running
//         • In PWR_MGMT_1 register (0x6B) set CYCLE = 0, SLEEP = 0, and GYRO_STANDBY = 0
//         • In PWR_MGMT_2 register (0x6C) set STBY_XA = STBY_YA = STBY_ZA = 0, and STBY_XG = STBY_YG = STBY_ZG = 1
//     */
//     M5.Imu.I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
//     regdata = regdata & 0b10001111; // set cyle, sleep, and gyro to standby, i.e. 0
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);

//     regdata = 0b00000111; // set accel x, y, and z to standby 
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_2, 1, &regdata);

//     /* Step 2: Set Accelerometer LPF bandwidth to 218.1 Hz
//         • In ACCEL_CONFIG2 register (0x1D) set ACCEL_FCHOICE_B = 0 and A_DLPF_CFG[2:0] = 1 (b001)
//     */
//     M5.Imu.I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG2, 1, &regdata);
//     regdata = 0b00100001; // average 32 samples, use 218 Hz DLPF
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG2, 1, &regdata);

//     /* Step 2.5 - active low? */
//     M5.Imu.I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &regdata);
//     regdata =  ((regdata | 0b10000000) & 0b11011111); // configure pin active-low, no latch
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &regdata);

//     /* Step 3: Enable Motion Interrupt
//         • In INT_ENABLE register (0x38) set WOM_INT_EN = 111 to enable motion interrupt
//     */
//     regdata = 0b11100000; // enable wake-on-motion interrupt for X, Y, and Z axes
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_ENABLE, 1, &regdata);
    
//     /* Step 4: Set Motion Threshold
//         • Set the motion threshold in ACCEL_WOM_THR register (0x1F)
//         NOTE: the data sheet mentions 0x1F, but is probably referring to
//               registers 0x20, 0x21, and 0x22 based on empirical tests
//     */
//     regdata = thresh_num_lsb; // set accel motion threshold for X, Y, and Z axes
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_WOM_X_THR, 1, &regdata);
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_WOM_Y_THR, 1, &regdata);
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_WOM_Z_THR, 1, &regdata);

//     /* Step 5: Enable Accelerometer Hardware Intelligence
//         • In ACCEL_INTEL_CTRL register (0x69) set ACCEL_INTEL_EN = ACCEL_INTEL_MODE = 1;
//           Ensure that bit 0 is set to 0
//     */
//     regdata = 0b11000010; // enable wake-on-motion if any of X, Y, or Z axes is above threshold
//     // WOM_STEP5_ACCEL_INTEL_CTRL_INTEL_EN_1_MODE_1_WOM_TH_MODE_0;
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_INTEL_CTRL, 1, &regdata);

//     /* Step 7: Set Frequency of Wake-Up
//         • In SMPLRT_DIV register (0x19) set SMPLRT_DIV[7:0] = 3.9 Hz – 500 Hz
//     */
//     // sample_rate = 1e3 / (1 + regdata)
//     //   4.0 Hz = 1e3 / (1 + 249)
//     //  10.0 Hz = 1e3 / (1 +  99)
//     //  20.0 Hz = 1e3 / (1 +  49)
//     //  25.0 Hz = 1e3 / (1 +  39)
//     //  50.0 Hz = 1e3 / (1 +  19) <----
//     // 500.0 Hz = 1e3 / (1 +   1)
//     regdata = 19;
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_SMPLRT_DIV, 1, &regdata);

//     /* Step 8: Enable Cycle Mode (Accelerometer Low-Power Mode)
//         • In PWR_MGMT_1 register (0x6B) set CYCLE = 1
//     */
//     M5.Imu.I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
//     regdata = regdata | 0b00100000; // enable accelerometer low-power mode
//     M5.Imu.I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
// }