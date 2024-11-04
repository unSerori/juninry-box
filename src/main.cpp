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
#include "./nimble/wifi_manager.h"

// global
int loopCount; // ループカウンター

// setup
void setup()
{
  Serial.begin(115200); // シリアル通信初期化
  M5.begin();           // M5本体セットアップ

  // BLE関連の初期化
  initBLE();

  delay(10000);

  // WiFi接続
  IPAddress ip = connectWifi(); // WiFi接続された場合のIPを受け取る
  // TODO: 保存

  // log
  Serial.println("Setup complete.");
  M5.Lcd.println("Setup complete.");
}

// loop
void loop()
{
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
