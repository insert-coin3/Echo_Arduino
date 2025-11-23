#include <Arduino.h>
#include <ServoMT.h>     
#include <FloatSW.h>
#include <StockSensor.h>
#include <PumpMT.h>
#include <SerialCommand.h>
#include <ArduinoJson.h>
#include "Pin.h" 

// ===== 하드웨어 객체 배열 (4개 재료 + 1개 컵) =====
ServoMT *servoMotors[5]; 
FloatSW *floatSwitches[1]; 
StockSensor *stockSensors[4];
// 🚨 [수정] 펌프 객체 배열 크기 증가: 물 펌프 1개 + DC 모터 1개 = 총 2개
PumpMT *pumps[2]; 
SerialCommand *serialCommand;

// ===== 타이밍 및 통신 변수 =====

uint64_t lastSensorReadingTime = 0;
bool isCommandExecuting = false;
uint64_t commandStartTime = 0;
uint64_t commandDuration = 0;
CommandType currentCommandType = COMMAND_NONE;

// 🚨 [추가] DC 모터 제어 관련 변수 및 상수
// DC 모터 인덱스
const int PUMP_INDEX_WATER = 0;
const int PUMP_INDEX_DC_MOTOR = 1;
// DC 모터 진동 시간 (밀리초)
const uint64_t VIBRATION_DURATION_MS = 500; // 0.5초 진동


// ===== 함수 프로토타입 =====
void sendSensorData();
void checkCommandCompletion(uint64_t currentTime);
void completeCommandExecution();
void resetCommandState();
void processNewCommand();
void executeCommand(const Command& command);
void executeSugarCommand(const Command& command);
void executeWaterCommand(const Command& command);
void executeCoffeeCommand(const Command& command);
void executeIcedTeaCommand(const Command& command);
void executeGreenTeaCommand(const Command& command);
void executeCupCommand(const Command& command); 
void startCommandExecution(CommandType commandType, float duration);
void startVibration(); // 🚨 [추가] 진동 시작 함수
void stopVibration(); // 🚨 [추가] 진동 중지 함수


/**
 * @brief 시스템 초기화
 */
void setup() {
    // ===== 하드웨어 객체 생성 =====
    // 서보 모터 및 재고 센서 (설탕, 커피, 아이스티, 녹차)
    servoMotors[0] = new ServoMT(PIN_SUGAR_SERVO, "SugarDispenser");
    stockSensors[0] = new StockSensor(PIN_SUGAR_LASER, PIN_SUGAR_SENSOR, "SugarStock");
    
    servoMotors[1] = new ServoMT(PIN_COFFEE_SERVO, "CoffeeDispenser");
    stockSensors[1] = new StockSensor(PIN_COFFEE_LASER, PIN_COFFEE_SENSOR, "CoffeeStock");
    
    servoMotors[2] = new ServoMT(PIN_ICEDTEA_SERVO, "IcedTeaDispenser");
    stockSensors[2] = new StockSensor(PIN_ICEDTEA_LASER, PIN_ICEDTEA_SENSOR, "IcedTeaStock");
    
    servoMotors[3] = new ServoMT(PIN_GREENTEA_SERVO, "GreenTeaDispenser");
    stockSensors[3] = new StockSensor(PIN_GREENTEA_LASER, PIN_GREENTEA_SENSOR, "GreenTeaStock");
    
    // 물 펌프 및 플로트 스위치
    pumps[PUMP_INDEX_WATER] = new PumpMT(PIN_WATER_PUMP, "WaterPump");
    floatSwitches[0] = new FloatSW(PIN_WATER_FLOAT_SWITCH, "WaterFloatSwitch");

    // 🚨 [추가] DC 모터 객체 생성 (PIN_DC_MOTOR는 Pin.h에 정의되어 있어야 함)
    pumps[PUMP_INDEX_DC_MOTOR] = new PumpMT(PIN_DC_MOTOR, "VibrationMotor");

    // 컵 디스펜서 서보 모터 추가
    servoMotors[4] = new ServoMT(PIN_CUP_SERVO, "CupDispenser");
    
    // 시리얼 명령 핸들러 
    serialCommand = new SerialCommand(BAUD_RATE_SERIAL);
    
    // ===== 시리얼 통신 초기화 =====
    serialCommand->begin();
    delay(1000);

    for (int i = 0; i < 4; i++) {
        stockSensors[i]->turnOnLaser();
    }

    // 파우더 디스펜서 (0~3번 인덱스): 닫힘 각도 20도
    for (int i = 0; i < 4; i++) {
        servoMotors[i]->setAngle(20); 
    }
    
    // 컵 디스펜서 (4번 인덱스): 닫힘 각도 0도
    servoMotors[4]->setAngle(0);

    // 🚨 [추가] DC 모터 초기 상태 OFF
    stopVibration();

    Serial.println("CafeFirmware initialized successfully");
}

/**
 * @brief 메인 루프
 */
void loop() {
    uint64_t currentTime = millis();
    
    // ===== 센서 데이터 주기적 전송 =====
    if (currentTime - lastSensorReadingTime >= INTERVAL_SENSOR_READING) {
        lastSensorReadingTime = currentTime;
        sendSensorData();
    }

    // 🚨 [추가] 명령 미실행 중일 때 DC 모터 진동 시간 확인
    if (!isCommandExecuting && currentCommandType == COMMAND_UNKNOWN) {
        if (currentTime - commandStartTime >= VIBRATION_DURATION_MS) {
            stopVibration();
            resetCommandState(); // 상태를 COMMAND_NONE으로 초기화
        }
    }

    if (isCommandExecuting) {
        checkCommandCompletion(currentTime);
    } else {
        processNewCommand();
    }
}

// ... (sendSensorData, checkCommandCompletion, completeCommandExecution, resetCommandState 함수는 그대로 유지) ...

// completeCommandExecution 함수는 그대로 유지

// resetCommandState 함수는 그대로 유지

/**
 * @brief 새로운 명령 처리
 */
void processNewCommand() {
    Command command = serialCommand->readCommand();
    
    if (command.type != COMMAND_NONE) {
        if (!command.isValid) {
            serialCommand->printError(command.errorMessage);
            return;
        }
        
        // 🚨 [추가] 새로운 명령 수신 시 DC 모터 짧게 작동
        if (command.type != COMMAND_CUP) { // 컵 분배 시에는 진동 불필요
            startVibration();
        }

        executeCommand(command);
    }
}

// ... (executeCommand 함수는 그대로 유지) ...

// ... (executeSugarCommand, executeWaterCommand, executeCoffeeCommand, executeIcedTeaCommand, executeGreenTeaCommand 함수는 그대로 유지) ...


// executeCupCommand 함수는 그대로 유지

/**
 * @brief 명령 실행 시작
 * @param commandType 명령 타입
 * @param duration 실행 시간 (초)
 */
void startCommandExecution(CommandType commandType, float duration) {
    isCommandExecuting = true;
    currentCommandType = commandType;
    commandStartTime = millis();
    commandDuration = (uint64_t)(duration * 1000);
}

// 🚨 [추가] DC 모터 제어 함수

/**
 * @brief DC 모터 (진동 장치) 작동 시작
 */
void startVibration() {
    // 펌프 배열 인덱스 1이 DC 모터임
    pumps[PUMP_INDEX_DC_MOTOR]->turnOn(); 
    
    // 진동 시간을 재기 위해 명령 실행 상태를 일시적으로 사용
    // isCommandExecuting을 사용하지 않고, COMMAND_UNKNOWN 타입으로 시간만 기록
    currentCommandType = COMMAND_UNKNOWN;
    commandStartTime = millis();
    // commandDuration을 사용하지 않고, loop()에서 VIBRATION_DURATION_MS를 사용
    
    Serial.println("Vibration motor started for 500ms");
}

/**
 * @brief DC 모터 (진동 장치) 작동 중지
 */
void stopVibration() {
    pumps[PUMP_INDEX_DC_MOTOR]->turnOff();
    Serial.println("Vibration motor stopped");
}