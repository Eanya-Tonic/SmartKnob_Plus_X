#include <FastLED.h>
#include <HX711.h>
#include <Adafruit_VEML7700.h>

#include "interface_task.h"
#include "util.h"
#include "hal\knob.h"
#include "hal\hal.h"

#define COUNT_OF(A) (sizeof(A) / sizeof(A[0]))

CRGB leds[NUM_LEDS];

HX711 scale;

Adafruit_VEML7700 veml = Adafruit_VEML7700();

bool inMenu = false;
bool compatibleMode = false;


InterfaceTask::InterfaceTask(const uint8_t task_core, MotorTask &motor_task) : Task("Interface", 4096, 1, task_core),
                                                                               stream_(),
                                                                               motor_task_(motor_task),
                                                                               plaintext_protocol_(stream_, motor_task_),
                                                                               proto_protocol_(stream_, motor_task_)
{

    log_queue_ = xQueueCreate(10, sizeof(std::string *));
    assert(log_queue_ != NULL);

    knob_state_queue_ = xQueueCreate(1, sizeof(PB_SmartKnobState));
    assert(knob_state_queue_ != NULL);
}

void InterfaceTask::run()
{
    stream_.begin();

    FastLED.addLeds<SK6812, PIN_LED_DATA, GRB>(leds, NUM_LEDS);

#if PIN_SDA >= 0 && PIN_SCL >= 0
    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);
#endif
    scale.begin(38, 2);

    if (veml.begin())
    {
        veml.setGain(VEML7700_GAIN_2);
        veml.setIntegrationTime(VEML7700_IT_400MS);
    }
    else
    {
        log("ALS sensor not found!");
    }

    motor_task_.setConfig(configs[3]);
    motor_task_.addListener(knob_state_queue_);
    // Start in legacy protocol mode
    plaintext_protocol_.init([this]()
                             { changeConfig(0); });
    SerialProtocol *current_protocol = &plaintext_protocol_;

    ProtocolChangeCallback protocol_change_callback = [this, &current_protocol](uint8_t protocol)
    {
        switch (protocol)
        {
        case SERIAL_PROTOCOL_LEGACY:
            current_protocol = &plaintext_protocol_;
            break;
        case SERIAL_PROTOCOL_PROTO:
            current_protocol = &proto_protocol_;
            break;
        default:
            log("Unknown protocol requested");
            break;
        }
    };

    plaintext_protocol_.setProtocolChangeCallback(protocol_change_callback);
    proto_protocol_.setProtocolChangeCallback(protocol_change_callback);

    // Interface loop:
    while (1)
    {
        PB_SmartKnobState state;
        if (xQueueReceive(knob_state_queue_, &state, 0) == pdTRUE)
        {
            current_protocol->handleState(state);
        }

        current_protocol->loop();

        std::string *log_string;
        while (xQueueReceive(log_queue_, &log_string, 0) == pdTRUE)
        {
            current_protocol->log(log_string->c_str());
            delete log_string;
        }

        updateHardware();

        delay(1);
    }
}

void InterfaceTask::log(const char *msg)
{
    // Allocate a string for the duration it's in the queue; it is free'd by the queue consumer
    std::string *msg_str = new std::string(msg);

    // Put string in queue (or drop if full to avoid blocking)
    xQueueSendToBack(log_queue_, &msg_str, 0);
}

void InterfaceTask::changeConfig(int32_t next)
{
    current_config_ = next;

    char buf_[256];
    snprintf(buf_, sizeof(buf_), "Changing config to %d -- %s", current_config_, configs[current_config_].text);
    log(buf_);
    motor_task_.setConfig(configs[current_config_]);
}

void InterfaceTask::updateHardware()
{
    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;

    const float LUX_ALPHA = 0.005;
    static float lux_avg;
    float lux = veml.readLux();
    lux_avg = lux * LUX_ALPHA + lux_avg * (1 - LUX_ALPHA);
    static uint32_t last_als;
    if (millis() - last_als > 1000)
    {
        last_als = millis();
    }

    if (scale.wait_ready_timeout(100))
    {
        int32_t reading = scale.read();

        static uint32_t last_reading_display;
        if (millis() - last_reading_display > 1000)
        {
            last_reading_display = millis();
        }

        // TODO: calibrate and track (long term moving average) zero point (lower); allow calibration of set point offset
        const int32_t lower = 950000;
        const int32_t upper = 1800000;
        // Ignore readings that are way out of expected bounds
        if (reading >= lower - (upper - lower) && reading < upper + (upper - lower) * 2)
        {
            long value = CLAMP(reading, lower, upper);
            press_value_unit = 1. * (value - lower) / (upper - lower);

            static bool pressed;
            if (!pressed && press_value_unit > 0.75)
            {
                motor_task_.playHaptic(true);
                    knob::isPressed(1);
                    while (press_value_unit > 0.75)
                    {
                        reading = scale.read();
                        value = CLAMP(reading, lower, upper);
                        press_value_unit = 1. * (value - lower) / (upper - lower);
                        delay(1);
                    }
                    knob::isPressed(0);
            }
            else if (pressed && press_value_unit < 0.25)
            {
                motor_task_.playHaptic(false);
                pressed = false;
            }
        }
    }

    uint16_t brightness = UINT16_MAX;
    // TODO: brightness scale factor should be configurable (depends on reflectivity of surface)
    brightness = (uint16_t)CLAMP(lux_avg * 13000, (float)1280, (float)32767);

    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        leds[i].setHSV(200 * press_value_unit, 255, brightness >> 8);

        // Gamma adjustment
        leds[i].r = dim8_video(leds[i].r);
        leds[i].g = dim8_video(leds[i].g);
        leds[i].b = dim8_video(leds[i].b);
    }
    FastLED.show();
}
