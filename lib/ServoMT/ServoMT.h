#ifndef SERVOMT_H
#define SERVOMT_H

#include <Arduino.h>
#include <Servo.h>

// ===== 서보 모터 각도 제한 =====
#define SERVO_ANGLE_MIN 0
#define SERVO_ANGLE_MAX 90

/**
 * @brief 서보 모터 제어 클래스
 * 
 * Arduino Servo 라이브러리를 래핑하여 더 편리한 인터페이스를 제공합니다.
 * 각도 제한, 상태 추적, 이름 관리 기능을 포함합니다.
 */
class ServoMT {
public:
    /**
     * @brief 생성자
     * @param pin 서보 모터 연결 핀 번호
     * @param name 서보 모터 식별 이름
     */
    ServoMT(int pin, String name);

    // ===== 각도 제어 메서드 =====
    /**
     * @brief 서보 모터 각도 설정
     * @param angle 설정할 각도 (0-90도)
     */
    void setAngle(int angle);
    
    /**
     * @brief 서보 모터 최대 각도로 설정 (열림)
     */
    void setMaxAngle();
    
    /**
     * @brief 서보 모터 최소 각도로 설정 (닫힘)
     */
    void setMinAngle();
    
    // ===== 상태 조회 메서드 =====
    /**
     * @brief 현재 서보 모터 각도 반환
     * @return 현재 각도
     */
    int getCurrentAngle() const;
    
    /**
     * @brief 서보 모터 이름 반환
     * @return 서보 모터 이름
     */
    String getName() const;

private:
    int servoPin;           // 서보 모터 핀 번호
    int currentAngle;       // 현재 각도
    Servo servo;           // Arduino Servo 객체
    String name;           // 서보 모터 이름
};

#endif // SERVOMT_H
