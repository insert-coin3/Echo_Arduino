#ifndef FLOATSW_H
#define FLOATSW_H

#include <Arduino.h>

// ===== 플로트 스위치 상태 정의 =====
#define FLOAT_STATE_EMPTY HIGH    // 액체 없음 (플로트 내려감)
#define FLOAT_STATE_FULL LOW      // 액체 있음 (플로트 올라감)

/**
 * @brief 플로트 스위치 제어 클래스
 * 
 * 플로트 스위치를 통해 액체 레벨을 감지하고 관리합니다.
 * 내부 풀업 저항을 사용하여 안정적인 신호를 제공합니다.
 */
class FloatSW {
public:
    /**
     * @brief 생성자
     * @param pin 플로트 스위치 연결 핀 번호
     * @param name 플로트 스위치 식별 이름
     */
    FloatSW(int pin, String name);

    // ===== 상태 읽기 메서드 =====
    /**
     * @brief 플로트 스위치 상태 읽기
     * @return 현재 상태 (HIGH: 액체 없음, LOW: 액체 있음)
     */
    int readState();
    
    /**
     * @brief 액체 존재 여부 확인
     * @return true: 액체 있음, false: 액체 없음
     */
    bool isLiquidPresent();
    
    /**
     * @brief 액체 부족 여부 확인
     * @return true: 액체 부족, false: 액체 충분
     */
    bool isLiquidEmpty();
    
    // ===== 정보 반환 메서드 =====
    /**
     * @brief 현재 상태 문자열 반환
     * @return "FULL" 또는 "EMPTY"
     */
    String getStateString();
    
    /**
     * @brief 플로트 스위치 핀 번호 반환
     * @return 핀 번호
     */
    int getPin() const;
    
    /**
     * @brief 플로트 스위치 이름 반환
     * @return 이름
     */
    String getName() const;

private:
    int floatPin;           // 플로트 스위치 핀 번호
    int currentState;       // 현재 상태
    String name;           // 플로트 스위치 이름
};

#endif // FLOATSW_H
