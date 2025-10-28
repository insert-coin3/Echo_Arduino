#include <Arduino.h>
#include <ServoMT.h>     
#include <FloatSW.h>
#include <StockSensor.h>
#include <PumpMT.h>
#include <SerialCommand.h>
#include <ArduinoJson.h>
#include "Pin.h"

// ===== 하드웨어 객체 배열 =====
ServoMT *servoMotors[5]; 
FloatSW *floatSwitches[1]; 
StockSensor *stockSensors[5];
PumpMT *pumps[1]; 
SerialCommand *serialCommand;

// ===== 타이밍 변수 =====
uint64_t lastSensorReadingTime = 0;

// ===== 명령 실행 상태 변수 =====
bool isCommandExecuting = false;
uint64_t commandStartTime = 0;
uint64_t commandDuration = 0;
CommandType currentCommandType = COMMAND_NONE;

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
void executeCupCommand(const Command& command); // 컵 디스펜서 함수 프로토타입 추가
void startCommandExecution(CommandType commandType, float duration);

/**
 * @brief 시스템 초기화
 */
void setup() {
    // ===== 하드웨어 객체 생성 =====
    // 서보 모터 및 재고 센서 확장 (설탕, 커피, 아이스티, 녹차)
    servoMotors[0] = new ServoMT(PIN_SUGAR_SERVO, "SugarDispenser");
    stockSensors[0] = new StockSensor(PIN_SUGAR_LASER, PIN_SUGAR_SENSOR, "SugarStock");
    
    servoMotors[1] = new ServoMT(PIN_COFFEE_SERVO, "CoffeeDispenser");
    stockSensors[1] = new StockSensor(PIN_COFFEE_LASER, PIN_COFFEE_SENSOR, "CoffeeStock");
    
    servoMotors[2] = new ServoMT(PIN_ICEDTEA_SERVO, "IcedTeaDispenser");
    stockSensors[2] = new StockSensor(PIN_ICEDTEA_LASER, PIN_ICEDTEA_SENSOR, "IcedTeaStock");
    
    servoMotors[3] = new ServoMT(PIN_GREENTEA_SERVO, "GreenTeaDispenser");
    stockSensors[3] = new StockSensor(PIN_GREENTEA_LASER, PIN_GREENTEA_SENSOR, "GreenTeaStock");
    
    // 물 펌프 및 플로트 스위치
    pumps[0] = new PumpMT(PIN_WATER_PUMP, "WaterPump");
    floatSwitches[0] = new FloatSW(PIN_WATER_FLOAT_SWITCH, "WaterFloatSwitch");

    // 컵 디스펜서 서보 모터 추가
    servoMotors[4] = new ServoMT(PIN_CUP_SERVO, "CupDispenser");
    
    // 시리얼 명령 핸들러
    serialCommand = new SerialCommand(BAUD_RATE_SERIAL);
    
    // ===== 시리얼 통신 초기화 =====
    serialCommand->begin();

    // ===== 서보 모터 초기화 대기 =====
    delay(1000);

    // ===== 레이저 모듈 활성화 =====
    for (int i = 0; i < 4; i++) {
        stockSensors[i]->turnOnLaser();
    }
    
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

    // ===== 명령 실행 상태 확인 =====
    if (isCommandExecuting) {
        checkCommandCompletion(currentTime);
    } else {
        // ===== 새로운 명령 처리 =====
        processNewCommand();
    }
}

/**
 * @brief 센서 데이터 전송
 */
void sendSensorData() {
    // ArduinoJson 라이브러리 사용
    StaticJsonDocument<256> doc; // JSON 문서 생성

    // 각 센서 상태를 JSON 문서에 추가
    doc["sugar"] = stockSensors[0]->getStockStateString();
    doc["coffee"] = stockSensors[1]->getStockStateString();
    doc["icetea"] = stockSensors[2]->getStockStateString();
    doc["greentea"] = stockSensors[3]->getStockStateString();
    doc["water"] = floatSwitches[0]->getStateString();

    // JSON 객체를 시리얼로 전송
    serializeJson(doc, Serial);
    Serial.println(); // 줄바꿈 추가
}

/**
 * @brief 명령 완료 확인
 * @param currentTime 현재 시간
 */
void checkCommandCompletion(uint64_t currentTime) {
    if (currentTime - commandStartTime >= commandDuration) {
        // 명령 실행 완료 처리
        completeCommandExecution();
    }
}

/**
 * @brief 명령 실행 완료 처리
 */

 
void completeCommandExecution() {
    switch (currentCommandType) {
        case COMMAND_SUGAR:
            servoMotors[0]->setMinAngle(); // 서보 모터 닫기
            serialCommand->printSuccess("Sugar dispensing completed");
            break;
            
        case COMMAND_WATER:
            pumps[0]->turnOff(); // 펌프 정지
            serialCommand->printSuccess("Water pumping completed");
            break;
            
        case COMMAND_COFFEE:
            servoMotors[1]->setMinAngle();
            serialCommand->printSuccess("Coffee dispensing completed");
            break;
            
        case COMMAND_ICEDTEA:
            servoMotors[2]->setMinAngle();
            serialCommand->printSuccess("IcedTea dispensing completed");
            break;
            
        case COMMAND_GREENTEA:
            servoMotors[3]->setMinAngle();
            serialCommand->printSuccess("GreenTea dispensing completed");
            break;

        case COMMAND_CUP: // 컵 디스펜서 명령 완료 처리
            servoMotors[4]->setMinAngle();
            serialCommand->printSuccess("Cup dispensing completed");
            break;
            
        default:
            break;
    }
    
    // 명령 실행 상태 초기화
    resetCommandState();
}

/**
 * @brief 명령 실행 상태 초기화
 */
void resetCommandState() {
    isCommandExecuting = false;
    currentCommandType = COMMAND_NONE;
    commandDuration = 0;
}

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
        
        executeCommand(command);
    }
}

/**
 * @brief 명령 실행
 * @param command 실행할 명령
 */
void executeCommand(const Command& command) {
    switch (command.type) {
        case COMMAND_SUGAR:
            executeSugarCommand(command);
            break;
            
        case COMMAND_WATER:
            executeWaterCommand(command);
            break;
            
        case COMMAND_COFFEE:
            executeCoffeeCommand(command);
            break;
            
        case COMMAND_ICEDTEA:
            executeIcedTeaCommand(command);
            break;
            
        case COMMAND_GREENTEA:
            executeGreenTeaCommand(command);
            break;

        case COMMAND_CUP: // 컵 디스펜서 명령 실행
            executeCupCommand(command);
            break;
            
        case COMMAND_UNKNOWN:
            serialCommand->printError("Unknown command: " + command.rawCommand);
            break;
            
        default:
            break;
    }
}

/**
 * @brief 설탕 분배 명령 실행
 * @param command 설탕 명령
 */
void executeSugarCommand(const Command& command) {
    if (stockSensors[0]->isStockEmpty()) {
        serialCommand->printError("Sugar stock is empty!");
        return;
    }
    serialCommand->printSuccess("Sugar command received: " + String(command.value) + "s");
    startCommandExecution(COMMAND_SUGAR, command.value);
    servoMotors[0]->setMaxAngle();
}

/**
 * @brief 물 펌핑 명령 실행
 * @param command 물 명령
 */
void executeWaterCommand(const Command& command) {
    if (floatSwitches[0]->isLiquidEmpty()) {
        serialCommand->printError("Water tank is empty!");
        return;
    }
    serialCommand->printSuccess("Water command received: " + String(command.value) + "s");
    startCommandExecution(COMMAND_WATER, command.value);
    pumps[0]->turnOn();
}

/**
 * @brief 커피 분배 명령 실행
 * @param command 커피 명령
 */
void executeCoffeeCommand(const Command& command) {
    if (stockSensors[1]->isStockEmpty()) {
        serialCommand->printError("Coffee stock is empty!");
        return;
    }
    serialCommand->printSuccess("Coffee command received: " + String(command.value) + "s");
    startCommandExecution(COMMAND_COFFEE, command.value);
    servoMotors[1]->setMaxAngle();
}

/**
 * @brief 아이스티 분배 명령 실행
 * @param command 아이스티 명령
 */
void executeIcedTeaCommand(const Command& command) {
    if (stockSensors[2]->isStockEmpty()) {
        serialCommand->printError("IcedTea stock is empty!");
        return;
    }
    serialCommand->printSuccess("IcedTea command received: " + String(command.value) + "s");
    startCommandExecution(COMMAND_ICEDTEA, command.value);
    servoMotors[2]->setMaxAngle();
}

/**
 * @brief 녹차 분배 명령 실행
 * @param command 녹차 명령
 */
void executeGreenTeaCommand(const Command& command) {
    if (stockSensors[3]->isStockEmpty()) {
        serialCommand->printError("GreenTea stock is empty!");
        return;
    }
    serialCommand->printSuccess("GreenTea command received: " + String(command.value) + "s");
    startCommandExecution(COMMAND_GREENTEA, command.value);
    servoMotors[3]->setMaxAngle();
}

/**
 * @brief 컵 디스펜서 명령 실행
 * @param command 컵 명령
 */
void executeCupCommand(const Command& command) {
    serialCommand->printSuccess("Cup command received: " + String(command.value) + "s");
    startCommandExecution(COMMAND_CUP, command.value);
    servoMotors[4]->setMaxAngle(); // 컵 디스펜서 서보 모터 작동
}

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