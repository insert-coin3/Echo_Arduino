#include "SerialCommand.h"

SerialCommand::SerialCommand(int baudRate) : baudRate(baudRate) {
}

void SerialCommand::begin() {
    ::Serial.begin(baudRate);
}

Command SerialCommand::readCommand() {
    Command cmd;
    cmd.type = COMMAND_NONE;
    cmd.value = 0.0;
    cmd.isValid = false;
    cmd.errorMessage = "";
    
    if (::Serial.available()) {
        String commandString = ::Serial.readStringUntil('\n');
        commandString.trim();
        
        if (commandString.length() == 0) {
            return cmd;
        }
        
        cmd.rawCommand = commandString;
        cmd.type = getCommandType(commandString);
        
        if (cmd.type == COMMAND_UNKNOWN) {
            cmd.errorMessage = "Unknown command: " + commandString;
            return cmd;
        }
        
        if (cmd.type != COMMAND_NONE) {
            cmd.value = extractValue(commandString);
            cmd.isValid = validateCommand(cmd);
        }
        
        // 시리얼 버퍼 비우기
        while (::Serial.available()) {
            ::Serial.read();
        }
    }
    
    return cmd;
}

bool SerialCommand::validateCommand(const Command& cmd) {
    if (cmd.type == COMMAND_NONE) {
        return false;
    }
    
    if (cmd.value < MIN_DURATION) {
        cmd.errorMessage = "Duration too short (minimum: " + String(MIN_DURATION) + "s)";
        return false;
    }
    
    if (cmd.type == COMMAND_SUGAR && cmd.value > MAX_SUGAR_DURATION) {
        cmd.errorMessage = "Sugar duration too long (maximum: " + String(MAX_SUGAR_DURATION) + "s)";
        return false;
    }
    
    if (cmd.type == COMMAND_WATER && cmd.value > MAX_WATER_DURATION) {
        cmd.errorMessage = "Water duration too long (maximum: " + String(MAX_WATER_DURATION) + "s)";
        return false;
    }
    
    return true;
}

CommandType SerialCommand::getCommandType(const String& commandString) {
    if (commandString.length() == 0) {
        return COMMAND_NONE;
    }
    
    // 첫 번째 문자 찾기 (공백 무시)
    size_t startPos = 0;
    while (startPos < commandString.length() && commandString[startPos] == ' ') {
        startPos++;
    }
    
    if (startPos >= commandString.length()) {
        return COMMAND_NONE;
    }
    
    char firstChar = commandString[startPos];
    
    if (firstChar == 'S' || firstChar == 's') {
        return COMMAND_SUGAR;
    } else if (firstChar == 'W' || firstChar == 'w') {
        return COMMAND_WATER;
    }
    
    return COMMAND_UNKNOWN;
}

float SerialCommand::extractValue(const String& commandString) {
    // 첫 번째 문자 찾기 (공백 무시)
    size_t startPos = 0;
    while (startPos < commandString.length() && commandString[startPos] == ' ') {
        startPos++;
    }
    
    if (startPos >= commandString.length()) {
        return 0.0;
    }
    
    // 명령 문자 건너뛰기
    startPos++;
    
    // 값 부분 추출
    String valueStr = commandString.substring(startPos);
    valueStr.trim(); // 값 부분의 공백 제거
    
    return valueStr.toFloat();
}

void SerialCommand::printError(const String& error) {
    ::Serial.println("ERROR: " + error);
}

void SerialCommand::printSuccess(const String& message) {
    ::Serial.println("SUCCESS: " + message);
} 