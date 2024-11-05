// includes
#include <M5Stack.h>      // M5本体
#include <NimBLEDevice.h> // BLE
#include <WiFi.h>
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
    ssid = pCharacteristic->getValue();
};

// コールバック呼び出し側で受け取ったWiFiのPASSを保存
void WifiConnectPASSCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{
    pass = pCharacteristic->getValue();

    // flag
    if (ssid != "" && pass != "")
    {
        isRequiredFieldsFilled = true;
    };
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

// WiFi接続
IPAddress connectWifi()
{
    // TODO: 保存済みか確認する

    // 最初に値をリセット
    ssid = "";
    pass = "";
    isRequiredFieldsFilled = false; // フラグ
    int tryCount = TRY_COUNT;       // 接続試行時の秒数

    do
    {
        // どちらかが空の状態なら接続を待ち受ける
        if (ssid == "" || pass == "")
        {
            pWifiConnectService->start();
            pAdvertising->start();

            // log
            Serial.println("Waiting for input connection...");
            M5.Lcd.println("Waiting for input connection...");

            // 揃うまで待機
            while (!isRequiredFieldsFilled)
            {
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

        // 何らかの手段でセキュアに
        // std::string wifiMask = genWifiMask(pass.length());
        // for (int i = 0; i < pass.length() && i < wifiMask.length(); i++)
        //     pass[i] ^= wifiMask[i];

        // 接続
        WiFi.mode(WIFI_MODE_STA);                   // モードをクライアント接続モードに変更
        WiFi.begin(ssid, pass);                     // 接続開始 // TODO: c_str()
        Serial.println("Connecting to WiFi AP..."); // msg
        M5.Lcd.println("Connecting to WiFi AP..."); // msg
        // 結果確認
        while (WiFi.status() != WL_CONNECTED) // 未接続な間
        {
            delay(1000);
            tryCount--;
            Serial.println(".");
            M5.Lcd.println(".");

            if (tryCount <= 0 | WiFi.status() == WL_CONNECT_FAILED) // 試行回数を使い切るか接続失敗するか
                return IPAddress();                                 // 0.0.0.0
        }
    } while (true);

    // return WiFi.localIP(); // 接続IPを返却
    // // 接続してくれるまでここで止まる
    // do
    // {
    //     if (true) // 接続試行できる状態なら、
    //     {
    //         IPAddress ip = wifiMng.connectWifi(wifiMng.getSsid(), wifiMng.getPass()); // 接続試行
    //         if (ip != IPAddress())
    //         { // 値が取れたら抜ける
    //             break;
    //         }
    //     }
    // } while (true);
};
