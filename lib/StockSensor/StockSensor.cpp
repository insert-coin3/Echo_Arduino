#include "StockSensor.h"

StockSensor::StockSensor(int laserPin, int lightSensorPin, String name) 
    : laserPin(laserPin), lightSensorPin(lightSensorPin), currentLightValue(STOCK_STATE_EMPTY), laserState(false), name(name) {
    
    pinMode(laserPin, OUTPUT);
    pinMode(lightSensorPin, INPUT);
    
    // 레이저 초기 상태: OFF
    digitalWrite(laserPin, LOW);
    laserState = false;
    
    Serial.println(name + " StockSensor initialized");
} 

void StockSensor::turnOnLaser() {
    digitalWrite(laserPin, HIGH);
    laserState = true;
}

void StockSensor::turnOffLaser() {
    digitalWrite(laserPin, LOW);
    laserState = false;
}

bool StockSensor::isLaserOn() const {
    return laserState;
}

int StockSensor::readLightSensor() {
    currentLightValue = digitalRead(lightSensorPin);
    return currentLightValue;
}

bool StockSensor::isStockPresent() {
    readLightSensor();  // 현재 상태 업데이트
    return (currentLightValue == STOCK_STATE_FULL);
}

bool StockSensor::isStockEmpty() {
    readLightSensor();  // 현재 상태 업데이트
    return (currentLightValue == STOCK_STATE_EMPTY);
}

String StockSensor::getStockStateString() {
    readLightSensor();  // 현재 상태 업데이트
    if (currentLightValue == STOCK_STATE_FULL) {
        return "LOW";   // 재고 있음 (레이저 빛 차단)
    } else {
        return "HIGH";  // 재고 없음 (레이저 빛 감지)
    }
}

int StockSensor::getLightSensorValue() const {
    return currentLightValue;
}

int StockSensor::getLaserPin() const {
    return laserPin;
}

int StockSensor::getLightSensorPin() const {
    return lightSensorPin;
}

String StockSensor::getName() const {
    return name;
} 