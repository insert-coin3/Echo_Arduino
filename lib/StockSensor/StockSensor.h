#ifndef STOCKSENSOR_H
#define STOCKSENSOR_H

#include <Arduino.h>

// ===== 재고 상태 정의 =====
#define STOCK_STATE_EMPTY HIGH    // 재고 없음 (레이저 빛 감지)
#define STOCK_STATE_FULL LOW      // 재고 있음 (레이저 빛 차단)

/**
 * @brief 재고 감지 센서 제어 클래스
 * 
 * 레이저 모듈과 조도 센서를 조합하여 재고 상태를 감지합니다.
 * 레이저 빛이 차단되면 재고가 있는 것으로 판단합니다.
 */
class StockSensor {
public:
    /**
     * @brief 생성자
     * @param laserPin 레이저 모듈 연결 핀 번호
     * @param lightSensorPin 조도 센서 연결 핀 번호
     * @param name 재고 센서 식별 이름
     */
    StockSensor(int laserPin, int lightSensorPin, String name);

    // ===== 레이저 제어 메서드 =====
    /**
     * @brief 레이저 모듈 켜기
     */
    void turnOnLaser();
    
    /**
     * @brief 레이저 모듈 끄기
     */
    void turnOffLaser();
    
    /**
     * @brief 레이저 모듈 상태 확인
     * @return true: 켜짐, false: 꺼짐
     */
    bool isLaserOn() const;

    // ===== 센서 읽기 메서드 =====
    /**
     * @brief 조도 센서 상태 읽기
     * @return 센서 값 (HIGH: 빛 감지, LOW: 빛 차단)
     */
    int readLightSensor();
    
    // ===== 재고 상태 확인 메서드 =====
    /**
     * @brief 재고 존재 여부 확인
     * @return true: 재고 있음, false: 재고 없음
     */
    bool isStockPresent();
    
    /**
     * @brief 재고 부족 여부 확인
     * @return true: 재고 없음, false: 재고 있음
     */
    bool isStockEmpty();
    
    // ===== 정보 반환 메서드 =====
    /**
     * @brief 현재 재고 상태 문자열 반환
     * @return "LOW" (재고 있음) 또는 "HIGH" (재고 없음)
     */
    String getStockStateString();
    
    /**
     * @brief 조도 센서 값 반환
     * @return 현재 센서 값
     */
    int getLightSensorValue() const;
    
    /**
     * @brief 레이저 모듈 핀 번호 반환
     * @return 핀 번호
     */
    int getLaserPin() const;
    
    /**
     * @brief 조도 센서 핀 번호 반환
     * @return 핀 번호
     */
    int getLightSensorPin() const;
    
    /**
     * @brief 재고 센서 이름 반환
     * @return 이름
     */
    String getName() const;

private:
    int laserPin;           // 레이저 모듈 핀 번호
    int lightSensorPin;     // 조도 센서 핀 번호
    int currentLightValue;  // 현재 조도 센서 값
    bool laserState;        // 레이저 모듈 상태
    String name;           // 재고 센서 이름
};

#endif // STOCKSENSOR_H 