#pragma once
#include "esphome.h"
#include "Adafruit_TinyUSB.h"

namespace esphome {
namespace hid_telephony {

// HID Telephony Report Descriptor (Usage Page 0x0B)
const uint8_t desc_hid_report[] = {
    0x05, 0x0B,        // USAGE_PAGE (Telephony)
    0x09, 0x05,        // USAGE (Headset)
    0xA1, 0x01,        // COLLECTION (Application)
    
    // INPUT REPORT (Buttons to Host - 2 bits used, 6 bits padding)
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

    // OUTPUT REPORT (LED States from Windows - 2 bits used, 6 bits padding)
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

class HIDTelephonyComponent : public Component {
 public:
  Adafruit_USBD_HID usb_hid;
  binary_sensor::BinarySensor *in_call_sensor = nullptr;
  binary_sensor::BinarySensor *mute_sensor = nullptr;

  void setup() override {
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();

    // Capture incoming output reports (LED sync packets from Windows)
    usb_hid.setReportCallback(nullptr, [this](uint8_t report_id, hid_report_type_t report_type, const uint8_t *buffer, uint16_t bufsize) {
      if (report_type == HID_REPORT_TYPE_OUTPUT && bufsize > 0) {
        uint8_t led_state = buffer[0];
        bool in_call = (led_state & 0x01) != 0;
        bool muted = (led_state & 0x02) != 0;

        if (this->in_call_sensor != nullptr) {
          this->in_call_sensor->publish_state(in_call);
        }
        if (this->mute_sensor != nullptr) {
          this->mute_sensor->publish_state(muted);
        }
      }
    });
  }
};

}  // namespace hid_telephony
}  // namespace esphome
