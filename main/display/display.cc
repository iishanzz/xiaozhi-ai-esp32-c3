#include <esp_log.h>
#include <esp_err.h>
#include <string>
#include <cstdlib>
#include <cstring>
#include <font_awesome.h>

// Bring in hardware I2C drivers to send the override command
#include <driver/i2c.h>

#include "display.h"
#include "board.h"
#include "application.h"
#include "audio_codec.h"
#include "settings.h"
#include "assets/lang_config.h"

#define TAG "Display"

// Hardware address definitions for standard SSD1306 OLED screens
#define I2C_OLED_NUM       I2C_NUM_0 
#define OLED_I2C_ADDRESS   0x3C

Display::Display() {
    ESP_LOGI(TAG, "Initializing Screen Flip Overrides...");
    
    // Create an un-buffered command link payload for the I2C pipeline
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    
    // 0x00 signifies that the following bytes are raw hardware control commands
    i2c_master_write_byte(cmd, 0x00, true); 
    
    // --- THE 180 DEGREES FLIP COMMANDS ---
    i2c_master_write_byte(cmd, 0xA1, true); // Flip Segment Remap (Horizontal Axis mirror)
    i2c_master_write_byte(cmd, 0xC8, true); // Flip COM Output Scan Direction (Vertical Axis mirror)
    
    i2c_master_stop(cmd);
    
    // Push the hardware instruction packets over the active system wire tracks
    esp_err_t ret = i2c_master_cmd_begin(I2C_OLED_NUM, cmd, pdMS_TO_TICKS(100));
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Screen successfully flipped 180 degrees!");
    } else {
        ESP_LOGE(TAG, "Screen override failed. Retrying on I2C port 1...");
        // Fallback for custom breakout boards using the secondary bus line
        i2c_cmd_handle_t cmd2 = i2c_cmd_link_create();
        i2c_master_start(cmd2);
        i2c_master_write_byte(cmd2, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd2, 0x00, true);
        i2c_master_write_byte(cmd2, 0xA1, true);
        i2c_master_write_byte(cmd2, 0xC8, true);
        i2c_master_stop(cmd2);
        i2c_master_cmd_begin(I2C_NUM_1, cmd2, pdMS_TO_TICKS(100));
        i2c_cmd_link_delete(cmd2);
    }
    i2c_cmd_link_delete(cmd);
}

Display::~Display() {
}

void Display::SetStatus(const char* status) {
    ESP_LOGW(TAG, "SetStatus: %s", status);
}

void Display::ShowNotification(const std::string &notification, int duration_ms) {
    ShowNotification(notification.c_str(), duration_ms);
}

void Display::ShowNotification(const char* notification, int duration_ms) {
    ESP_LOGW(TAG, "ShowNotification: %s", notification);
}

void Display::UpdateStatusBar(bool update_all) {
}

void Display::SetEmotion(const char* emotion) {
    ESP_LOGW(TAG, "SetEmotion: %s", emotion);
}

void Display::SetChatMessage(const char* role, const char* content) {
    ESP_LOGW(TAG, "Role:%s", role);
    ESP_LOGW(TAG, "     %s", content);
}

void Display::SetTheme(Theme* theme) {
    current_theme_ = theme;
    Settings settings("display", true);
    settings.SetString("theme", theme->name());
}

void Display::SetPowerSaveMode(bool on) {
    ESP_LOGW(TAG, "SetPowerSaveMode: %d", on);
}
