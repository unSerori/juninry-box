// include
#include <M5Stack.h>

// global
int count;

void setup()
{
  Serial.begin(115200); // シリアル通信初期化
  M5.begin();           // M5本体セットアップ

  // log
  Serial.println("Setup complete.");
  M5.Lcd.println("Setup complete.");
}

void loop()
{
  Serial.println("aaa");
  Serial.println("あああ");

  Serial.println(count);
  M5.Lcd.println(count);
  count++;
  delay(1000);
}
