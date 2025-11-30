#ifndef PIN_H
#define PIN_H

// ===== 핀 정의 =====
#define PIN_CUP_SERVO 3

#define PIN_SUGAR_SERVO        5
#define PIN_SUGAR_LASER        7
#define PIN_SUGAR_SENSOR       8

#define PIN_COFFEE_SERVO       6
#define PIN_COFFEE_LASER       9
#define PIN_COFFEE_SENSOR      10

#define PIN_ICEDTEA_SERVO      11
#define PIN_ICEDTEA_LASER      12
#define PIN_ICEDTEA_SENSOR     13

#define PIN_GREENTEA_SERVO     14
#define PIN_GREENTEA_LASER     15
#define PIN_GREENTEA_SENSOR    16

#define PIN_WATER_PUMP         4
#define PIN_WATER_FLOAT_SWITCH 2

#define PIN_DC_MOTOR          17

// ===== 서보 모터 각도 설정 =====
// #define SERVO_ANGLE_CLOSED 0          // 서보 모터 닫힘 각도
// #define SERVO_ANGLE_OPEN 90           // 서보 모터 열림 각도 

// ===== 타이밍 설정 =====
#define INTERVAL_SENSOR_READING 1000  // 센서 읽기 주기 (밀리초)

// ===== 시리얼 통신 설정 =====
#define BAUD_RATE_SERIAL 9600

// ===== 명령 접두사 =====
#define CMD_PREFIX_SUGAR 'S'
#define CMD_PREFIX_WATER 'W'
#define CMD_PREFIX_COFFEE 'C'
#define CMD_PREFIX_ICEDTEA 'I'
#define CMD_PREFIX_GREENTEA 'G'
#define CMD_PREFIX_DC_MOTOR  'D'

// ===== 재고 상태 문자열 (JSON 값으로 사용) =====
#define STR_STOCK_HIGH "High"
#define STR_STOCK_LOW "Low"

#endif