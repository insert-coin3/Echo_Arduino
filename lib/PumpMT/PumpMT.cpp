#include "PumpMT.h"

PumpMT::PumpMT(int pin, String name) 
    : pumpPin(pin), pumpState(false), name(name) {
    pinMode(pumpPin, OUTPUT);
    digitalWrite(pumpPin, PUMP_STATE_OFF);  // 초기 상태: 펌프 OFF
    Serial.println(name + " PumpMT initialized");
}

void PumpMT::turnOn() {
    digitalWrite(pumpPin, PUMP_STATE_ON);
    pumpState = true;
}

void PumpMT::turnOff() {
    digitalWrite(pumpPin, PUMP_STATE_OFF);
    pumpState = false;
}

void PumpMT::toggle() {
    if (pumpState) {
        turnOff();
    } else {
        turnOn();
    }
}

bool PumpMT::isOn() const {
    return pumpState;
}

bool PumpMT::isOff() const {
    return !pumpState;
}

void PumpMT::runForDuration(unsigned long durationMs) {
    if (durationMs > 0) {
        turnOn();
        delay(durationMs);
        turnOff();
    }
}

String PumpMT::getStateString() {
    if (pumpState) {
        return "ON";
    } else {
        return "OFF";
    }
}

int PumpMT::getPin() const {
    return pumpPin;
}

String PumpMT::getName() const {
    return name;
} 