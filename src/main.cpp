#include <Arduino.h>
#include <ServoMT.h>     
#include <FloatSW.h>
#include <StockSensor.h>
#include <PumpMT.h>
#include <SerialCommand.h>
#include <ArduinoJson.h>
#include "Pin.h" // Pin.h에 정의된 #define 상수를 사용합니다.

// ===== 하드웨어 객체 배열 (크기 5: 4개 재료 + 1개 컵) =====
ServoMT *servoMotors[5]; 
FloatSW *floatSwitches[1]; 
StockSensor *stockSensors[4];
PumpMT *pumps[1]; 
SerialCommand *serialCommand;

// ===== 타이밍 및 통신 변수 (Pin.h의 #define을 사용하므로 const 선언 삭제) =====

uint64_t lastSensorReadingTime = 0;
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
void executeCupCommand(const Command& command); 
void startCommandExecution(CommandType commandType, float duration);

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
    pumps[0] = new PumpMT(PIN_WATER_PUMP, "WaterPump");
    floatSwitches[0] = new FloatSW(PIN_WATER_FLOAT_SWITCH, "WaterFloatSwitch");

    // ===== 컵 디스펜서 서보 모터 추가 =====
    servoMotors[4] = new ServoMT(PIN_CUP_SERVO, "CupDispenser");
    
    // 시리얼 명령 핸들러 (BAUD_RATE_SERIAL 상수는 Pin.h에서 가져옴)
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

    Serial.println("CafeFirmware initialized successfully");
}

/**
 * @brief 메인 루프
 */
void loop() {
    uint64_t currentTime = millis();
    
    // ===== 센서 데이터 주기적 전송 (INTERVAL_SENSOR_READING 상수는 Pin.h에서 가져옴) =====
    if (currentTime - lastSensorReadingTime >= INTERVAL_SENSOR_READING) {
        lastSensorReadingTime = currentTime;
        sendSensorData();
    }

    if (isCommandExecuting) {
        checkCommandCompletion(currentTime);
    } else {
        processNewCommand();
    }
}

/**
 * @brief 센서 데이터 전송
 */
void sendSensorData() {
    StaticJsonDocument<256> doc; 

    doc["sugar"] = stockSensors[0]->getStockStateString();
    doc["coffee"] = stockSensors[1]->getStockStateString();
    doc["icetea"] = stockSensors[2]->getStockStateString();
    doc["greentea"] = stockSensors[3]->getStockStateString();
    doc["water"] = floatSwitches[0]->getStateString();

    serializeJson(doc, Serial);
    Serial.println(); 
}

/**
 * @brief 명령 완료 확인
 * @param currentTime 현재 시간
 */
void checkCommandCompletion(uint64_t currentTime) {
    if (currentTime - commandStartTime >= commandDuration) {
        completeCommandExecution();
    }
}

/**
 * @brief 명령 실행 완료 처리
 */
void completeCommandExecution() {
    switch (currentCommandType) {
        case COMMAND_SUGAR:
            // 닫힘 각도 30도 (파우더)
            servoMotors[0]->setAngle(20); 
            serialCommand->printSuccess("Sugar dispensing completed");
            break;
            
        case COMMAND_WATER:
            pumps[0]->turnOff(); 
            serialCommand->printSuccess("Water pumping completed");
            break;
            
        case COMMAND_COFFEE:
             // 닫힘 각도 30도 (파우더)
            servoMotors[1]->setAngle(20); 
            serialCommand->printSuccess("Coffee dispensing completed");
            break;
            
        case COMMAND_ICEDTEA:
             // 닫힘 각도 30도 (파우더)
            servoMotors[2]->setAngle(20); 
            serialCommand->printSuccess("IcedTea dispensing completed");
            break;
            
        case COMMAND_GREENTEA:
             // 닫힘 각도 30도 (파우더)
            servoMotors[3]->setAngle(20); 
            serialCommand->printSuccess("GreenTea dispensing completed");
            break;

        case COMMAND_CUP: 
            // ===== 닫힘 각도 0도로 복귀 (컵) =====
            servoMotors[4]->setAngle(0); 
            serialCommand->printSuccess("Cup dispensing completed");
            break;
            
        default:
            break;
    }
    
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
            
        case COMMAND_CUP:
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
    // 열림 각도 0도 (파우더)
    servoMotors[0]->setAngle(0);
}

/**
 * @brief 물 펌핑 명령 실행
 * @param command 물 명령
 */
void executeWaterCommand(const Command& command) {
    // --- [수정] 플로트 스위치 문제로 재고 확인 로직을 일시적으로 무시합니다. ---
    // if (floatSwitches[0]->isLiquidEmpty()) { 
    //     serialCommand->printError("Water tank is empty!");
    //     return;
    // }
    // ---------------------------------------------------------------------

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
    // 열림 각도 0도 (파우더)
    servoMotors[1]->setAngle(0); 
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
    // 열림 각도 0도 (파우더)
    servoMotors[2]->setAngle(0);
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
    // 열림 각도 0도 (파우더)
    servoMotors[3]->setAngle(0);
}

/**

 * @brief 컵 디스펜서 명령 실행
 * @param command 컵 명령
 */
void executeCupCommand(const Command& command) {
    serialCommand->printSuccess("Cup command received: " + String(command.value) + "s");
    startCommandExecution(COMMAND_CUP, command.value);
    // ===== 열림 각도 150도로 설정 (컵) =====
    servoMotors[4]->setAngle(180); 
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