#pragma once
#include "i_page.h"

namespace pages
{
    class WifiConfigurationPage : public IPage
    {
    public:
        void onEnter(LiquidCrystal_I2C &lcd) override;
        void onExit() override;
        void update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed) override;

    private:
        void render(LiquidCrystal_I2C &lcd); // Draws the two buttons with the cursor

        int selected_ = 0; // 0 = Configure, 1 = Back
    };

    extern WifiConfigurationPage wifi_configuration_page; // The single instance used by PageManager
}