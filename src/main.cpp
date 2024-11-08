/******************************************************************************
 * @file    main.h
 * @brief   実行開始ファイル
 * @author  unSerori
 * @date    20241103
 * @par
 * Copyright (c) 2024 unSerori All rights reserved.
 ******************************************************************************/

// include
#include <M5Stack.h>      // M5本体
#include <NimBLEDevice.h> // BLE
#include <ESPping.h>
#include "./nimble/wifi_manager.h"
#include "servoc-control/servo-control.h"
#include "./screen/screen.h"

// ImageDisplayオブジェクトを作成
ImageDisplay imageDisplay;
// GPIOピン2、初期角度180度でサーボ制御オブジェクトを作成
ServoControl servoControl(2, 180);
// global
int loopCount; // ループカウンター

// setup
void setup()
{
  imageDisplay.begin();        // 初期化
  imageDisplay.displayImage(); // 初期画像を表示

  servoControl.begin();//サーボ制御オブジェクトの初期化

  // log
  Serial.println("Setup started.");
  M5.Lcd.println("Setup started.");

  // シリアル通信初期化
  Serial.begin(115200);
  // M5本体セットアップ
  M5.begin();

  // BLE関連の初期化
  initBLE();

  Serial.println("BLE init complete.");
  M5.Lcd.println("BLE init complete.");

  ///////////////////////////////////////////////////////////////////////////

  // // WiFi接続をどちらかでおこなう
  // IPAddress ip = connectWifi(); // WiFi接続された場合のIPを受け取る
  // // IPAddress ip = connectWifiWithConfig(); // あらかじめ設定してあるIPで接続
  // Serial.println(ip);
  // if (Ping.ping("8.8.8.8"))
  // {
  //   Serial.println("ping ok!");
  //   M5.Lcd.println("ping ok!");
  // }
  // else
  // {
  //   Serial.println("Failed ping."); // msg
  //   M5.Lcd.println("Failed ping."); // msg
  // }

  ////////////////////////////////////////////////////////////////////////////////////////

  // TODO: 保存

  // log
  Serial.println("Setup complete.");
  M5.Lcd.println("Setup complete.");
}

// loop
void loop()
{
  servoControl.checkButtons(); // ボタンを確認してサーボを制御

  // TODO: ロングポーリング

  // log
  Serial.println("loop count: " + String(loopCount));
  M5.Lcd.println("loop count: " + String(loopCount));
  loopCount++;
  delay(1000);
}

// 関数コメントのさんぷる

/******************************************************************************
 * @fn      関数名
 * @brief   関数の処理
 * @param   引数名: 説明
 * @return  返り値の型
 * @detail  説明
 ******************************************************************************/
