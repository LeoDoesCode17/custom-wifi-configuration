#include "wifi_configuration_page.h"
#include "lcd_utils.h"
#include "page_id.h"
#include "managers/page_manager.h"
#include "managers/wifi_config_manager.h"

namespace pages
{
    WifiConfigurationPage wifi_configuration_page; // Create the global instance

    namespace
    {
        constexpr int OPTION_COUNT = 2; // Two buttons: Configure, Back
    }

    void WifiConfigurationPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        selected_ = 0;                               // Always start on "Configure"
        lcd.clear();                                 // Wipe the previous page
        lcd.backlight();                             // Make sure the backlight is on
        lcd_utils::printLine(lcd, 0, "WIFI CONFIG"); // Row 0: title
        render(lcd);                                 // Rows 2-3: buttons
    }

    void WifiConfigurationPage::onExit() {} // Nothing to clean up

    void WifiConfigurationPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed) // Encoder button was clicked
        {
            if (selected_ == 0) // "Configure" is highlighted
            {
                if (wifi_config_manager::start()) // Try to start AP + web server
                {
                    page_manager::navigate_to(PageId::ConfigMode); // Success: show the guide page
                }
                else
                {
                    lcd_utils::printLine(lcd, 1, "AP start failed!"); // Failure: stay here, show error
                }
            }
            else // "Back" is highlighted
            {
                page_manager::navigate_to(PageId::Menu); // Return to the menu
            }
            return; // Done for this loop iteration
        }

        if (encoderDelta != 0) // Knob was rotated
        {
            // Move the cursor with wrap-around (flip the sign if the direction feels inverted)
            const int step = static_cast<int>(encoderDelta % OPTION_COUNT);
            selected_ = (selected_ + step + OPTION_COUNT) % OPTION_COUNT;
            lcd_utils::printLine(lcd, 1, ""); // Clear a previous error message, if any
            render(lcd);                      // Redraw cursor position
        }
    }

    void WifiConfigurationPage::render(LiquidCrystal_I2C &lcd)
    {
        lcd_utils::printLine(lcd, 1, wifi_config_manager::getReceivedSsid() == "" ? "Not configured" : wifi_config_manager::getReceivedSsid());
        lcd_utils::printLine(lcd, 2, String(selected_ == 0 ? "> " : "  ") + "Configure"); // Row 2
        lcd_utils::printLine(lcd, 3, String(selected_ == 1 ? "> " : "  ") + "Back");      // Row 3
    }
}