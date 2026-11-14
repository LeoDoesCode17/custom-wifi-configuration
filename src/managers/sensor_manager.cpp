#include "sensor_manager.h"
#include "sensors/rotary_encoder.h"
#include <Arduino.h>

namespace
{
    const uint8_t ROTARY_ENCODER_DT = 5;
    const uint8_t ROTARY_ENCODER_CLK = 4;
    const uint8_t ROTARY_ENCODER_SW = 6;
}

namespace sensor_manager
{
    void initialize()
    {
        rotary_encoder::initialize(ROTARY_ENCODER_DT, ROTARY_ENCODER_CLK, ROTARY_ENCODER_SW);
    }

    long getEncoderPosition()
    {
        return rotary_encoder::getPosition();
    }

    void setEncoderPosition(long position)
    {
        rotary_encoder::setPosition(position);
    }

    bool wasEncoderButtonPressed()
    {
        return rotary_encoder::wasButtonPressed();
    }

}