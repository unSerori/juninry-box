// include
#include <M5Stack.h>      // M5本体
#include <NimBLEDevice.h> // BLE

// global
int count;

// setup
void setup()
{
  Serial.begin(115200); // シリアル通信初期化
  M5.begin();           // M5本体セットアップ

  // log
  Serial.println("Setup complete.");
  M5.Lcd.println("Setup complete.");
}

// loop
void loop()
{
  // log
  Serial.println("loop count: " + String(count));
  M5.Lcd.println("loop count: " + String(count));
  count++;
  delay(1000);
}
