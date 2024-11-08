#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <M5Stack.h>       // M5Stackライブラリ（ディスプレイとボタン機能の制御に使用）
#include <ESP32Servo.h>    // ESP32サーボライブラリ（ESP32でのサーボ制御に使用）

// ServoControlクラス
// M5Stackのボタン操作でサーボを制御するためのクラス
class ServoControl {
public:
    // コンストラクタ
    // pin: サーボを接続するGPIOピン番号
    // initialAngle: サーボの初期角度（デフォルトは180度）
    ServoControl(int pin, int initialAngle = 180);

    // M5Stackとサーボの初期化メソッド
    // M5Stackのディスプレイとボタンの初期化、サーボのピン接続と初期角度設定を行う
    void begin();

    // サーボモーターを指定角度へ移動させるメソッド
    // newAngle: サーボが移動する目標角度
    void moveToAngle(int newAngle);

    // ボタンの状態を確認し、特定の角度にサーボを移動させるメソッド
    // ボタンAで0度、ボタンCで180度に移動
    void checkButtons();

private:
    Servo myServo;         // サーボオブジェクト
    int angle;             // 現在のサーボ角度
    int pin;               // サーボを接続するGPIOピン番号
};

#endif // SERVO_CONTROL_H
