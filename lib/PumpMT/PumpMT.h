#ifndef PUMPMT_H
#define PUMPMT_H

#include <Arduino.h>

// ===== 펌프 상태 정의 =====
#define PUMP_STATE_OFF LOW     // 펌프 정지
#define PUMP_STATE_ON HIGH     // 펌프 작동

/**
 * @brief 펌프 제어 클래스
 * 
 * 릴레이를 통해 펌프를 제어합니다.
 * ON/OFF 제어, 토글, 시간 제어 기능을 제공합니다.
 */
class PumpMT {
public:
    /**
     * @brief 생성자
     * @param pin 펌프 릴레이 연결 핀 번호
     * @param name 펌프 식별 이름
     */
    PumpMT(int pin, String name);

    // ===== 기본 제어 메서드 =====
    /**
     * @brief 펌프 켜기
     */
    void turnOn();
    
    /**
     * @brief 펌프 끄기
     */
    void turnOff();
    
    /**
     * @brief 펌프 상태 토글
     */
    void toggle();
    
    // ===== 상태 확인 메서드 =====
    /**
     * @brief 펌프 작동 상태 확인
     * @return true: 켜짐, false: 꺼짐
     */
    bool isOn() const;
    
    /**
     * @brief 펌프 정지 상태 확인
     * @return true: 꺼짐, false: 켜짐
     */
    bool isOff() const;
    
    // ===== 시간 제어 메서드 =====
    /**
     * @brief 지정된 시간 동안 펌프 작동
     * @param durationMs 작동 시간 (밀리초)
     */
    void runForDuration(unsigned long durationMs);
    
    // ===== 정보 반환 메서드 =====
    /**
     * @brief 현재 펌프 상태 문자열 반환
     * @return "ON" 또는 "OFF"
     */
    String getStateString();
    
    /**
     * @brief 펌프 핀 번호 반환
     * @return 핀 번호
     */
    int getPin() const;
    
    /**
     * @brief 펌프 이름 반환
     * @return 이름
     */
    String getName() const;

private:
    int pumpPin;            // 펌프 릴레이 핀 번호
    bool pumpState;         // 펌프 상태
    String name;           // 펌프 이름
};

#endif // PUMPMT_H 