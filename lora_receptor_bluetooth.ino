#include <Arduino.h>
/*
  This is a simple example show the Heltec.LoRa recived data in OLED.
  The onboard OLED display is SSD1306 driver and I2C interface. In order to make the
  OLED correctly operation, you should output a high-low-high(1-0-1) signal by soft-
  ware to OLED's reset pin, the low-level signal at least 5ms.
  OLED pins to ESP32 GPIOs via this connecthin:
  OLED_SDA -- GPIO4
  OLED_SCL -- GPIO15
  OLED_RST -- GPIO16
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include "heltec.h"
//#include "images.h"

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#define BAND 868E6 //you can set band here directly,e.g. 868E6,915E6
String rssi = "RSSI --";
String packSize = "--";
String packet;
int count = 0;

void logo()
{
  Heltec.display->clear();
  //Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void LoRaData()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 15, "Received " + packSize + " bytes");
  Heltec.display->drawStringMaxWidth(0, 26, 128, packet);
  Heltec.display->drawString(0, 0, rssi);
  Heltec.display->display();
  Serial.println(packet);
}

void cbk(int packetSize)
{
  packet = "";
  packSize = String(packetSize, DEC);
  for (int i = 0; i < packetSize; i++)
  {
    packet += (char)LoRa.read();
  }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
  LoRaData();
}

void setup()
{
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

  //  LoRa.setSignalBandwidth(62.5E3);  //cambio parametro
  LoRa.setSpreadingFactor(10);     //cambio
  SerialBT.begin("LoraBluetooth"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  pinMode(25, OUTPUT);

  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();

  Heltec.display->drawString(0, 0, "Heltec.LoRa inicio exitoso!");
  Heltec.display->drawString(0, 10, "esperando datos entrada...");
  Heltec.display->display();
  delay(1000);
  //LoRa.onReceive(cbk);

  LoRa.receive();
}

void loop()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    cbk(packetSize);

    digitalWrite(25, HIGH);
  }

  if (millis() > count + 1500)
  {

    count = millis();
    digitalWrite(25, LOW);

    SerialBT.println(packet);
    SerialBT.println("      ");
  }

  delay(10);


  
}
