#include "ServoMT.h"

ServoMT::ServoMT(int pin, String name) 
    : servoPin(pin), currentAngle(SERVO_ANGLE_MIN), name(name) {
    pinMode(servoPin, OUTPUT);
    servo.attach(servoPin);
    servo.write(currentAngle);  // 초기 각도 설정
    Serial.println(name + " ServoMT initialized");
}

void ServoMT::setAngle(int angle) {
    // 각도 범위 제한
    if (angle < SERVO_ANGLE_MIN) angle = SERVO_ANGLE_MIN;
    if (angle > SERVO_ANGLE_MAX) angle = SERVO_ANGLE_MAX;

    servo.write(angle);
    currentAngle = angle;
}

void ServoMT::setMaxAngle() {
    setAngle(SERVO_ANGLE_MAX);
}

void ServoMT::setMinAngle() {
    setAngle(SERVO_ANGLE_MIN);
}

int ServoMT::getCurrentAngle() const {
    return currentAngle;
}

String ServoMT::getName() const {
    return name;
}
