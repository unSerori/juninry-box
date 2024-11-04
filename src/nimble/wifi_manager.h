// インクルードガード
#ifndef WIFI_MANAGER
#define WIFI_MANAGER

// includes
#include <M5Stack.h>      // M5本体
#include <NimBLEDevice.h> // BLE
#include <WiFi.h>

// define
#define WIFI_CONNECT_SERVICE_UUID "6b0b"
#define WIFI_CONNECT_SSID_CHARACTERISTIC_UUID "6b0b"
#define WIFI_CONNECT_PASS_CHARACTERISTIC_UUID "ee6a"
#define TRY_COUNT 10 // 接続のループ

// global
static NimBLEServer *pServer;              // BLEサーバーのキャラクタリスティックの宣言
static NimBLEService *pWifiConnectService; // サービス
static NimBLEAdvertising *pAdvertising;    // 広告
static String ssid = "";
static String pass = "";
static bool isRequiredFieldsFilled = false;

// コールバック処理の定義

// コールバック呼び出し側で受け取ったWiFiのSSIDを保存
class WifiConnectSSIDCallbacks : public NimBLECharacteristicCallbacks
{
public:
    void onWrite(NimBLECharacteristic *pCharacteristic);
};

// コールバック呼び出し側で受け取ったWiFiのPASSを保存
class WifiConnectPASSCallbacks : public NimBLECharacteristicCallbacks
{
public:
    void onWrite(NimBLECharacteristic *pCharacteristic);
};

// BLEの初期化処理
void initBLE();

// マスク生成
std::string genWifiMask(size_t len);

// WiFi接続
IPAddress connectWifi();

#endif
