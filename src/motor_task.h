#pragma once

#include <Arduino.h>
#include <SimpleFOC.h>
#include <vector>

#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"

#include "hal/hal.h"


enum class CommandType {
    CALIBRATE,
    CONFIG,
    HAPTIC,
};

typedef struct
{
    bool is_outbound;
    int32_t position;
    float angle_offset;
} MotorStatusInfo;


struct HapticData {
    bool press;
};

struct Command {
    CommandType command_type;
    union CommandData {
        uint8_t unused;
        PB_SmartKnobConfig config;
        HapticData haptic;
    };
    CommandData data;
};

class MotorTask : public Task<MotorTask> {
    friend class Task<MotorTask>; // Allow base Task to invoke protected run()

    public:
        MotorTask(const uint8_t task_core);
        ~MotorTask();

        void setConfig(const PB_SmartKnobConfig& config);
        void playHaptic(bool press);
        void runCalibration();

        void addListener(QueueHandle_t queue);
        void setLogger(Logger* logger);
        void motor_shake(int strength, int delay_time);
        
        // BLDC motor instance
        BLDCMotor motor = BLDCMotor(1);

    protected:
        void run();

    private:
        QueueHandle_t queue_;
        Logger* logger_;
        std::vector<QueueHandle_t> listeners_;
        char buf_[72];

        // BLDC driver instance
        BLDCDriver6PWM driver = BLDCDriver6PWM(PIN_UH, PIN_UL, PIN_VH, PIN_VL, PIN_WH, PIN_WL);

        void publish(const PB_SmartKnobState& state);
        void calibrate();
        void checkSensorError();
        void log(const char* msg);
};
