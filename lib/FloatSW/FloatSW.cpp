#include "FloatSW.h"

FloatSW::FloatSW(int pin, String name) 
    : floatPin(pin), currentState(FLOAT_STATE_EMPTY), name(name) {
    pinMode(floatPin, INPUT_PULLUP);  // 내부 풀업 저항 사용
    currentState = digitalRead(floatPin);  // 초기 상태 읽기
    Serial.println(name + " FloatSW initialized");
}

int FloatSW::readState() {
    currentState = digitalRead(floatPin);
    return currentState;
}

bool FloatSW::isLiquidPresent() {
    readState();  // 현재 상태 업데이트
    return (currentState == FLOAT_STATE_FULL);
}

bool FloatSW::isLiquidEmpty() {
    readState();  // 현재 상태 업데이트
    return (currentState == FLOAT_STATE_EMPTY);
}

String FloatSW::getStateString() {
    readState();  // 현재 상태 업데이트
    if (currentState == FLOAT_STATE_FULL) {
        return "HIGH";
    } else {
        return "LOW";
    }
}

int FloatSW::getPin() const {
    return floatPin;
}

String FloatSW::getName() const {
    return name;
}
