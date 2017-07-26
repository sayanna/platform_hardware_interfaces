/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "DefaultVehicleHal_v2_1"
#include <android/log.h>

#include <log/log.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <algorithm>

#include "EmulatedVehicleHal.h"

#define DEBUG_SOCKET    (33452)

namespace android {
namespace hardware {
namespace automotive {
namespace vehicle {
namespace V2_1 {

namespace impl {

static std::unique_ptr<Obd2SensorStore> fillDefaultObd2Frame(
        size_t numVendorIntegerSensors,
        size_t numVendorFloatSensors) {
    using V2_0::toInt;
    std::unique_ptr<Obd2SensorStore> sensorStore(new Obd2SensorStore(
            numVendorIntegerSensors, numVendorFloatSensors));

    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::FUEL_SYSTEM_STATUS,
        toInt(Obd2FuelSystemStatus::CLOSED_LOOP));
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::MALFUNCTION_INDICATOR_LIGHT_ON, 0);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::IGNITION_MONITORS_SUPPORTED,
        toInt(Obd2IgnitionMonitorKind::SPARK));
    sensorStore->setIntegerSensor(DiagnosticIntegerSensorIndex::IGNITION_SPECIFIC_MONITORS,
        Obd2CommonIgnitionMonitors::COMPONENTS_AVAILABLE |
        Obd2CommonIgnitionMonitors::MISFIRE_AVAILABLE |
        Obd2SparkIgnitionMonitors::AC_REFRIGERANT_AVAILABLE |
        Obd2SparkIgnitionMonitors::EVAPORATIVE_SYSTEM_AVAILABLE);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::INTAKE_AIR_TEMPERATURE, 35);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::COMMANDED_SECONDARY_AIR_STATUS,
        toInt(Obd2SecondaryAirStatus::FROM_OUTSIDE_OR_OFF));
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::NUM_OXYGEN_SENSORS_PRESENT, 1);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::RUNTIME_SINCE_ENGINE_START, 500);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::DISTANCE_TRAVELED_WITH_MALFUNCTION_INDICATOR_LIGHT_ON, 0);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::WARMUPS_SINCE_CODES_CLEARED, 51);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::DISTANCE_TRAVELED_SINCE_CODES_CLEARED, 365);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::ABSOLUTE_BAROMETRIC_PRESSURE, 30);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::CONTROL_MODULE_VOLTAGE, 12);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::AMBIENT_AIR_TEMPERATURE, 18);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::MAX_FUEL_AIR_EQUIVALENCE_RATIO, 1);
    sensorStore->setIntegerSensor(
        DiagnosticIntegerSensorIndex::FUEL_TYPE, V2_0::toInt(Obd2FuelType::GASOLINE));
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::CALCULATED_ENGINE_LOAD, 0.153);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::SHORT_TERM_FUEL_TRIM_BANK1, -0.16);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::LONG_TERM_FUEL_TRIM_BANK1, -0.16);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::SHORT_TERM_FUEL_TRIM_BANK2, -0.16);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::LONG_TERM_FUEL_TRIM_BANK2, -0.16);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::INTAKE_MANIFOLD_ABSOLUTE_PRESSURE, 7.5);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::ENGINE_RPM, 1250.);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::VEHICLE_SPEED, 40.);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::TIMING_ADVANCE, 2.5);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::THROTTLE_POSITION, 19.75);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::OXYGEN_SENSOR1_VOLTAGE, 0.265);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::FUEL_TANK_LEVEL_INPUT, 0.824);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::EVAPORATION_SYSTEM_VAPOR_PRESSURE, -0.373);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::CATALYST_TEMPERATURE_BANK1_SENSOR1, 190.);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::RELATIVE_THROTTLE_POSITION, 3.);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::ABSOLUTE_THROTTLE_POSITION_B, 0.306);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::ACCELERATOR_PEDAL_POSITION_D, 0.188);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::ACCELERATOR_PEDAL_POSITION_E, 0.094);
    sensorStore->setFloatSensor(
        DiagnosticFloatSensorIndex::COMMANDED_THROTTLE_ACTUATOR, 0.024);

    return sensorStore;
}

void EmulatedVehicleHal::initObd2LiveFrame(const V2_0::VehiclePropConfig& propConfig) {
    auto liveObd2Frame = createVehiclePropValue(V2_0::VehiclePropertyType::COMPLEX, 0);
    auto sensorStore = fillDefaultObd2Frame(static_cast<size_t>(propConfig.configArray[0]),
                                            static_cast<size_t>(propConfig.configArray[1]));
    sensorStore->fillPropValue("", liveObd2Frame.get());
    liveObd2Frame->prop = OBD2_LIVE_FRAME;

    mPropStore->writeValue(*liveObd2Frame);
}

void EmulatedVehicleHal::initObd2FreezeFrame(const V2_0::VehiclePropConfig& propConfig) {
    auto sensorStore = fillDefaultObd2Frame(static_cast<size_t>(propConfig.configArray[0]),
                                            static_cast<size_t>(propConfig.configArray[1]));

    static std::vector<std::string> sampleDtcs = { "P0070", "P0102" "P0123" };
    for (auto&& dtc : sampleDtcs) {
        auto freezeFrame = createVehiclePropValue(V2_0::VehiclePropertyType::COMPLEX, 0);
        sensorStore->fillPropValue(dtc, freezeFrame.get());
        freezeFrame->prop = OBD2_FREEZE_FRAME;

        mPropStore->writeValue(*freezeFrame);
    }
}

V2_0::StatusCode EmulatedVehicleHal::fillObd2FreezeFrame(
        const V2_0::VehiclePropValue& requestedPropValue,
        V2_0::VehiclePropValue* outValue) {
    if (requestedPropValue.value.int64Values.size() != 1) {
        ALOGE("asked for OBD2_FREEZE_FRAME without valid timestamp");
        return V2_0::StatusCode::INVALID_ARG;
    }
    auto timestamp = requestedPropValue.value.int64Values[0];
    auto freezeFrame = mPropStore->readValueOrNull(OBD2_FREEZE_FRAME, 0, timestamp);
    if(freezeFrame == nullptr) {
        ALOGE("asked for OBD2_FREEZE_FRAME at invalid timestamp");
        return V2_0::StatusCode::INVALID_ARG;
    }
    outValue->prop = OBD2_FREEZE_FRAME;
    outValue->value.int32Values = freezeFrame->value.int32Values;
    outValue->value.floatValues = freezeFrame->value.floatValues;
    outValue->value.bytes = freezeFrame->value.bytes;
    outValue->value.stringValue = freezeFrame->value.stringValue;
    outValue->timestamp = freezeFrame->timestamp;
    return V2_0::StatusCode::OK;
}

V2_0::StatusCode EmulatedVehicleHal::clearObd2FreezeFrames(const V2_0::VehiclePropValue& propValue) {
    if (propValue.value.int64Values.size() == 0) {
        mPropStore->removeValuesForProperty(OBD2_FREEZE_FRAME);
        return V2_0::StatusCode::OK;
    } else {
        for(int64_t timestamp: propValue.value.int64Values) {
            auto freezeFrame = mPropStore->readValueOrNull(OBD2_FREEZE_FRAME, 0, timestamp);
            if(freezeFrame == nullptr) {
                ALOGE("asked for OBD2_FREEZE_FRAME at invalid timestamp");
                return V2_0::StatusCode::INVALID_ARG;
            }
            mPropStore->removeValue(*freezeFrame);
        }
    }
    return V2_0::StatusCode::OK;
}

V2_0::StatusCode EmulatedVehicleHal::fillObd2DtcInfo(V2_0::VehiclePropValue* outValue) {
    std::vector<int64_t> timestamps;
    for(const auto& freezeFrame: mPropStore->readValuesForProperty(OBD2_FREEZE_FRAME)) {
        timestamps.push_back(freezeFrame.timestamp);
    }
    outValue->value.int64Values = timestamps;
    outValue->prop = OBD2_FREEZE_FRAME_INFO;
    return V2_0::StatusCode::OK;
}

void EmulatedVehicleHal::onCreate() {
    V2_0::impl::EmulatedVehicleHal::onCreate();

    initObd2LiveFrame(*mPropStore->getConfigOrDie(OBD2_LIVE_FRAME));
    initObd2FreezeFrame(*mPropStore->getConfigOrDie(OBD2_FREEZE_FRAME));
}

void EmulatedVehicleHal::initStaticConfig() {
    for (auto&& cfg = std::begin(kVehicleProperties); cfg != std::end(kVehicleProperties); ++cfg) {
        V2_0::VehiclePropertyStore::TokenFunction tokenFunction = nullptr;

        switch (cfg->prop) {
            case OBD2_FREEZE_FRAME: {
                tokenFunction = [] (const V2_0::VehiclePropValue& propValue) {
                    return propValue.timestamp;
                };
                break;
            }
            default:
                break;
        }

        mPropStore->registerProperty(*cfg, tokenFunction);
    }
}

EmulatedVehicleHal::VehiclePropValuePtr EmulatedVehicleHal::get(
        const V2_0::VehiclePropValue& requestedPropValue,
        V2_0::StatusCode* outStatus) {

    auto propId = requestedPropValue.prop;
    VehiclePropValuePtr v = nullptr;
    auto& pool = *getValuePool();

    switch (propId) {
    case OBD2_FREEZE_FRAME:
        v = pool.obtainComplex();
        *outStatus = fillObd2FreezeFrame(requestedPropValue, v.get());
        return v;
    case OBD2_FREEZE_FRAME_INFO:
        v = pool.obtainComplex();
        *outStatus = fillObd2DtcInfo(v.get());
        return v;
    default:
        return V2_0::impl::EmulatedVehicleHal::get(requestedPropValue, outStatus);
    }
}

V2_0::StatusCode EmulatedVehicleHal::set(const V2_0::VehiclePropValue& propValue) {
    auto propId = propValue.prop;
    switch (propId) {
    case OBD2_FREEZE_FRAME_CLEAR:
        return clearObd2FreezeFrames(propValue);
    case VEHICLE_MAP_SERVICE:
        // Placeholder for future implementation of VMS property in the default hal. For now, just
        // returns OK; otherwise, hal clients crash with property not supported.
        return V2_0::StatusCode::OK;
    default:
        return V2_0::impl::EmulatedVehicleHal::set(propValue);
    }
}

}  // impl

}  // namespace V2_1
}  // namespace vehicle
}  // namespace automotive
}  // namespace hardware
}  // namespace android
