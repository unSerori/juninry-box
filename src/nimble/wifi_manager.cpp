// includes
#include <M5Stack.h>      // M5本体
#include <NimBLEDevice.h> // BLE
#include <WiFi.h>
#include "config.h"
#include "wifi_manager.h"

// コールバック処理の定義

// 接続時の処理
void BLEClientConnectDisconnectCallbacks::onConnect(NimBLEServer *pServer)
{
    // log
    Serial.println("Client connected.");
    M5.Lcd.println("Client connected.");
};

// 切断時の処理
void BLEClientConnectDisconnectCallbacks::onDisconnect(NimBLEServer *pServer)
{
    // log
    Serial.println("Client disconnected.");
    M5.Lcd.println("Client disconnected.");
};

// コールバック呼び出し側で受け取ったWiFiのSSIDを保存
void WifiConnectSSIDCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{
    // 値を受け取る
    ssid = pCharacteristic->getValue();

    // log
    Serial.println("SSID: " + ssid);
    M5.Lcd.println("SSID: " + ssid);
};

// コールバック呼び出し側で受け取ったWiFiのPASSを保存
void WifiConnectPASSCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{
    // 値を受け取る
    pass = pCharacteristic->getValue();

    // flag
    if (ssid != "" && pass != "")
    {
        isRequiredFieldsFilled = true;
    };

    // log
    Serial.println("PASS: " + pass);
    M5.Lcd.println("PASS: " + pass);
};

// BLEの初期化処理
void initBLE()
{
    // デバイス名を設定
    NimBLEDevice::init("Juninry-BOX");

// トランスミットパワー（:送信出力）の設定
#ifdef ESP_PLATFORM // 規定値を参照
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
#else
    NimBLEDevice::setPower(9); // +9db
#endif

    // 通信セキュリティ
    NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_SC); // false false true // /*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC

    // サーバーとして設定
    pServer = NimBLEDevice::createServer();                           // サーバーとして初期化
    pServer->setCallbacks(new BLEClientConnectDisconnectCallbacks()); // 接続切断などのコールバック処理を設定 new NimBLEServerCallbacks()

    // サービスの設定

    // サービスの作成
    pWifiConnectService = pServer->createService(WIFI_CONNECT_SERVICE_UUID); // BLEサーバーのインスタンスから、任意のUUIDを付けたサービスを作成

    // キャラクタリスティックを作成

    // SSID受信用のキャラクタリスティック
    NimBLECharacteristic *pSSIDCharacteristic = pWifiConnectService->createCharacteristic(
        WIFI_CONNECT_SSID_CHARACTERISTIC_UUID, // このキャラクタリスティックのID
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);                                   // サポートする機能 // NIMBLE_PROPERTY::READ, NIMBLE_PROPERTY::WRITE, NIMBLE_PROPERTY::NOTIFY
    pSSIDCharacteristic->setValue("Input SSID");                       // 初期値
    pSSIDCharacteristic->setCallbacks(new WifiConnectSSIDCallbacks()); // 到達時に実行する追加のカスタム処理をコールバック処理として設定  // 先に宣言しおいて&インスタンス名みたいに入れてもいい

    // PASS受診用のキャラクタリスティック
    NimBLECharacteristic *pPASSCharacteristic = pWifiConnectService->createCharacteristic(
        WIFI_CONNECT_PASS_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);                                   // サポートする機能 // NIMBLE_PROPERTY::READ, NIMBLE_PROPERTY::WRITE, NIMBLE_PROPERTY::NOTIFY
    pPASSCharacteristic->setValue("Input PASS");                       // 初期値
    pPASSCharacteristic->setCallbacks(new WifiConnectPASSCallbacks()); // 到達時に実行する追加のカスタム処理をコールバック処理として設定

    //  広告を準備
    pAdvertising = NimBLEDevice::getAdvertising();                // インスタンスを作成
    pAdvertising->addServiceUUID(pWifiConnectService->getUUID()); // サービスを登録
    pAdvertising->setScanResponse(true);                          // レスポンス設定
    // pAdvertising->start();                                            // 開始

    // log
    Serial.println("BLE server Setup complete.");
    M5.Lcd.println("BLE server Setup complete.");
};

// マスク生成
std::string genWifiMask(size_t len)
{
    std::string mask;
    mask.reserve(len); // 要素の確保
    for (size_t i = 0; i < len; ++i)
    {
        mask += static_cast<char>(rand() % 256); // 0〜255のランダムなバイトを生成
    }
    return mask;
};

// WiFI接続（ベタ書き）
IPAddress connectWifiWithConfig()
{
    // 切断しておく
    WiFi.disconnect(true);

    // ベタ書き
    WiFi.mode(WIFI_MODE_STA);         // モードをクライアント接続モードに変更
    const char *incl_ssid = SSID;     // SSID取得
    const char *incl_pass = PASS;     // PASS取得
    WiFi.begin(incl_ssid, incl_pass); // 接続開始 // TODO: c_str()
    while (WiFi.status() != WL_CONNECTED)
    {
        // log
        Serial.print("Current status of waiting for CONNECTED: ");
        Serial.println(WiFi.status());
        M5.Lcd.print("Current status of waiting for CONNECTED: ");
        M5.Lcd.println(WiFi.status());
        // delay
        delay(1000);
    }

    return WiFi.localIP();
}

// WiFi接続
IPAddress connectWifi()
{
    // 切断しておく
    WiFi.disconnect(true);

    // TODO: 保存済みか確認する

    // 値を受け取るまで無限ループで待ち受けて接続
    do
    {
        // log
        Serial.println("connectWiFi do while top."); // msg
        M5.Lcd.println("connectWiFi do while top."); // msg

        Serial.print("1: ");
        Serial.println(WiFi.status()); // 最初に値をリセット
        ssid = "";
        pass = "";
        isRequiredFieldsFilled = false; // フラグ
        int tryCount = TRY_COUNT;       // 接続試行時の秒数

        // どちらかが空の状態なら接続を待ち受ける
        if (ssid == "" || pass == "")
        {
            Serial.print("2: ");
            Serial.println(WiFi.status());
            pWifiConnectService->start();
            pAdvertising->start();

            // log
            Serial.println("Waiting for input connection...");
            M5.Lcd.println("Waiting for input connection...");

            // 揃うまで待機
            while (!isRequiredFieldsFilled)
            {
                Serial.print("3: ");
                Serial.println(WiFi.status());
                delay(1000);
                Serial.println(".");
                M5.Lcd.println(".");
            } // 揃ったら抜けてくる
            // 広告は必要ないので止める
            pAdvertising->stop();

            // log
            Serial.println("\n");
            M5.Lcd.println("\n");
        }

        // ここから接続
        Serial.print("4: ");
        Serial.println(WiFi.status());
        // 何らかの手段でセキュアに？

        // 接続
        WiFi.mode(WIFI_MODE_STA);                   // モードをクライアント接続モードに変更
        WiFi.disconnect(true);                      // 切断
        WiFi.begin(ssid.c_str(), pass.c_str());     // 接続開始 // TODO: Arduinoの文字列型（String）ではなくCスタイルの文字列型（const char*）を求めてるためc_str()が必要？？
        Serial.println("Connecting to WiFi AP..."); // msg
        M5.Lcd.println("Connecting to WiFi AP..."); // msg
        // 結果確認
        while (true) // 未接続な間、抜ける（返る）条件はエラーハンドルの前でif-breakで行う
        {
            Serial.print("5: ");
            Serial.println(WiFi.status());
            delay(1000);
            tryCount--;
            Serial.println(".");
            M5.Lcd.println(".");

            // それぞれの成功失敗にハンドル
            if (WiFi.status() == WL_CONNECTED)
            {
                // 成功
                Serial.println(WiFi.status());
                // log
                Serial.print("Connected WiFi AP!!: ");
                Serial.println(WiFi.localIP());
                M5.Lcd.print("Connected WiFi AP!!: ");
                M5.Lcd.println(WiFi.localIP());

                return WiFi.localIP(); // return IPAddress(); // 0.0.0.0 // return WiFi.localIP();
            }
            else if (tryCount <= 0) // 試行回数を使い切る
            {
                // log
                Serial.println("Failed to connect to WiFi AP, tryCount.");
                M5.Lcd.println("Failed to connect to WiFi AP, tryCount.");

                break; // 失敗したためWiFi情報受付に戻る
            }
            else if (WiFi.status() == WL_CONNECT_FAILED) // 接続失敗するか
            {
                // log
                Serial.println("Failed to connect to WiFi AP, status is WL_CONNECT_FAILED."); // msg
                M5.Lcd.println("Failed to connect to WiFi AP, status is WL_CONNECT_FAILED."); // msg

                continue; // まだ失敗なためもうちょい待つ
            }
            else
            {
                // log
                Serial.print("unexpected state of affairs: ");
                Serial.println(WiFi.status());
                M5.Lcd.print("unexpected state of affairs: ");
                M5.Lcd.println(WiFi.status());

                continue; // 失敗したためもうちょい待つ
            }
        }

        // // 結果確認
        // while (WiFi.status() != WL_CONNECTED) // 未接続な間
        // {
        //     Serial.print("5: ");
        //     Serial.println(WiFi.status());
        //     delay(1000);
        //     tryCount--;
        //     Serial.println(".");
        //     M5.Lcd.println(".");

        //     // それぞれの失敗にハンドル
        //     if (tryCount <= 0) // 試行回数を使い切るか、
        //     {
        //         // log
        //         Serial.println("Failed to connect to WiFi AP, tryCount."); // msg
        //         M5.Lcd.println("Failed to connect to WiFi AP, tryCount."); // msg

        //         break; // 失敗したため接続受付に戻る
        //     }
        //     if (WiFi.status() == WL_CONNECT_FAILED) // 接続失敗するか
        //     {
        //         // log
        //         Serial.println("Failed to connect to WiFi AP, status is WL_CONNECT_FAILED."); // msg
        //         M5.Lcd.println("Failed to connect to WiFi AP, status is WL_CONNECT_FAILED."); // msg

        //         continue; // 失敗したためもうちょい待つ

        //     } // 成功
        //     else
        //     {
        //         Serial.println(WiFi.status());
        //         // log
        //         Serial.println("Connected WiFi AP!!: " + WiFi.localIP()); // msg
        //         M5.Lcd.println("Connected WiFi AP!!: " + WiFi.localIP()); // msg

        //         return WiFi.localIP(); // return IPAddress(); // 0.0.0.0
        //     }
        // }
    } while (true);
};
