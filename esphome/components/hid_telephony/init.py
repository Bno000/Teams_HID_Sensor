import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import binary_sensor
from esphome.const import CONF_ID

AUTO_LOAD = ["binary_sensor"]

hid_telephony_ns = cg.esphome_ns.namespace("hid_telephony")
HIDTelephonyComponent = hid_telephony_ns.class_("HIDTelephonyComponent", cg.Component)

CONF_IN_CALL = "in_call"
CONF_MUTE = "mute"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HIDTelephonyComponent),
    cv.Optional(CONF_IN_CALL): binary_sensor.binary_sensor_schema(),
    cv.Optional(CONF_MUTE): binary_sensor.binary_sensor_schema(),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_IN_CALL in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_IN_CALL])
        cg.add(var.set_in_call_sensor(sens))

    if CONF_MUTE in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_MUTE])
        cg.add(var.set_mute_sensor(sens))
