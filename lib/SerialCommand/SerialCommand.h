#ifndef SERIALCOMMAND_H
#define SERIALCOMMAND_H

#include <Arduino.h>

// ===== 명령 타입 정의 =====
enum CommandType {
    COMMAND_NONE,        // 명령 없음
    COMMAND_SUGAR,       // 설탕 분배 명령
    COMMAND_WATER,       // 물 펌핑 명령
    COMMAND_COFFEE,      // 커피 분배 명령
    COMMAND_ICEDTEA,     // 아이스티 분배 명령
    COMMAND_GREENTEA,     // 녹차 분배 명령
    COMMAND_CUP, // 컵 디스펜스 명령
    COMMAND_UNKNOWN      // 알 수 없는 명령
};

// ===== 명령 구조체 =====
struct Command {
    CommandType type;        // 명령 타입
    float value;            // 명령 값 (초)
    String rawCommand;      // 원본 명령 문자열
    bool isValid;           // 명령 유효성
    mutable String errorMessage;    // 에러 메시지 (mutable로 const 함수에서도 수정 가능)
};

/**
 * @brief 시리얼 명령 처리 클래스
 * 
 * 시리얼 통신을 통해 명령을 받아 파싱하고 검증합니다.
 * 설탕 분배(S)와 물 펌핑(W) 명령을 지원합니다.
 */
class SerialCommand {
public:
    /**
     * @brief 생성자
     * @param baudRate 시리얼 통신 속도 (기본값: 9600)
     */
    SerialCommand(int baudRate = 9600);

    // ===== 초기화 메서드 =====
    /**
     * @brief 시리얼 통신 초기화
     */
    void begin();
    
    // ===== 명령 처리 메서드 =====
    /**
     * @brief 시리얼에서 명령 읽기 및 파싱
     * @return 파싱된 명령 구조체
     */
    Command readCommand();
    
    /**
     * @brief 명령 유효성 검증
     * @param cmd 검증할 명령
     * @return true: 유효, false: 무효
     */
    bool validateCommand(const Command& cmd);
    
    // ===== 정적 유틸리티 메서드 =====
    /**
     * @brief 명령 문자열에서 타입 추출
     * @param commandString 명령 문자열
     * @return 명령 타입
     */
    static CommandType getCommandType(const String& commandString);
    
    /**
     * @brief 명령 문자열에서 값 추출
     * @param commandString 명령 문자열
     * @return 추출된 값 (초)
     */
    static float extractValue(const String& commandString);
    
    // ===== 메시지 출력 메서드 =====
    /**
     * @brief 에러 메시지 출력
     * @param error 에러 메시지
     */
    void printError(const String& error);
    
    /**
     * @brief 성공 메시지 출력
     * @param message 성공 메시지
     */
    void printSuccess(const String& message);

private:
    int baudRate;                                    // 시리얼 통신 속도
    static constexpr float MAX_SUGAR_DURATION = 10.0f;  // 최대 설탕 분배 시간 (초)
    static constexpr float MAX_WATER_DURATION = 30.0f;  // 최대 물 펌핑 시간 (초)
    static constexpr float MIN_DURATION = 0.01f;        // 최소 작동 시간 (초)
};

#endif // SERIALCOMMAND_H 