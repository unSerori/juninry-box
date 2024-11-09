#include "servo-control.h"

// コンストラクタ
// pin: サーボを接続するGPIOピン番号
// initialAngle: サーボの初期角度（デフォルトは180度）
ServoControl::ServoControl(int pin, int initialAngle)
    : pin(pin), angle(initialAngle) {}

// M5Stackおよびサーボの初期化を行うメソッド
void ServoControl::begin()
{
    M5.begin();           // M5Stackの初期化（ディスプレイ、ボタン、電源管理などの準備）
    myServo.attach(pin);  // サーボを指定したGPIOピンに接続
    myServo.write(angle); // サーボを初期角度に設定
}

// サーボモーターを指定した角度に移動させ、画面に角度を表示するメソッド
// newAngle: サーボが移動する目標角度（度数）
void ServoControl::moveToAngle(int newAngle)
{
    angle = newAngle;     // 新しい角度をメンバ変数に設定
    myServo.write(angle); // サーボを指定角度に移動
}

// ボタンの状態とポイントの状態を確認し、箱の開閉をするメソッド
void ServoControl::checkButtons()
{
    M5.update(); // ボタンの状態を更新して検出を行う

    /////////[ここから下がサーボ制御の条件分岐と実行コード]///////////////////
    // ここから条件を追記してね
    // 角度は変えないでね

    // TODO: ポイント条件を追加
    if (M5.BtnA.wasPressed())
    { // ボタンAが押された場合: 開錠
      // log
        Serial.println("Pressed btn A.");
        M5.Lcd.println("Pressed btn A.");

        moveToAngle(180); // サーボを0度に移動
    }
    else if (M5.BtnC.wasPressed())
    { // ボタンCが押された場合: 施錠
        // log
        Serial.println("Pressed btn B.");
        M5.Lcd.println("Pressed btn B.");

        moveToAngle(60); // サーボを180度に移動
    }

    /////////[ここまでサーボ制御の条件分岐と実行コード]///////////////////
}
