#pragma once
#include "esphome.h"
#include "tusb.h"

namespace esphome {
namespace hid_telephony {

// HID Telephony Report Descriptor (Usage Page 0x0B)
const uint8_t desc_hid_report[] = {
    0x05, 0x0B,        // USAGE_PAGE (Telephony)
    0x09, 0x05,        // USAGE (Headset)
    0xA1, 0x01,        // COLLECTION (Application)
    
    // INPUT REPORT (Buttons to Host)
    0x05, 0x0B,        //   USAGE_PAGE (Telephony)
    0x09, 0x20,        //   USAGE (Hook Switch)
    0x09, 0x2F,        //   USAGE (Phone Mute)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x02,        //   REPORT_COUNT (2)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0x95, 0x06,        //   REPORT_COUNT (6)
    0x81, 0x03,        //   INPUT (Cnst,Var,Abs)

    // OUTPUT REPORT (LED States from Windows)
    0x05, 0x0B,        //   USAGE_PAGE (Telephony)
    0x09, 0x20,        //   USAGE (Off-Hook LED)
    0x09, 0x2F,        //   USAGE (Mute LED)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x02,        //   REPORT_COUNT (2)
    0x91, 0x02,        //   OUTPUT (Data,Var,Abs)
    0x95, 0x06,        //   REPORT_COUNT (6)
    0x91, 0x03,        //   OUTPUT (Cnst,Var,Abs)
    
    0xC0               // END_COLLECTION
};

// Global instance pointers for the native TinyUSB callbacks to reference
static binary_sensor::BinarySensor *g_in_call_sensor = nullptr;
static binary_sensor::BinarySensor *g_mute_sensor = nullptr;

// TinyUSB Callbacks (Called automatically by the native stack)
extern "C" {
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    if (report_type == HID_REPORT_TYPE_OUTPUT && bufsize > 0) {
        uint8_t led_state = buffer[0];
        bool in_call = (led_state & 0x01) != 0;
        bool muted = (led_state & 0x02) != 0;

        if (g_in_call_sensor != nullptr) {
            g_in_call_sensor->publish_state(in_call);
        }
        if (g_mute_sensor != nullptr) {
            g_mute_sensor->publish_state(muted);
        }
    }
}
}

class HIDTelephonyComponent : public Component {
 public:
  binary_sensor::BinarySensor *in_call_sensor{nullptr};
  binary_sensor::BinarySensor *mute_sensor{nullptr};

  void set_in_call_sensor(binary_sensor::BinarySensor *sensor) { 
    this->in_call_sensor = sensor; 
    g_in_call_sensor = sensor;
  }
  
  void set_mute_sensor(binary_sensor::BinarySensor *sensor) { 
    this->mute_sensor = sensor; 
    g_mute_sensor = sensor;
  }

  void setup() override {
    ESP_LOGI("hid_telephony", "Native USB HID Telephony component initialized.");
  }
};

}  // namespace hid_telephony
}  // namespace esphome
