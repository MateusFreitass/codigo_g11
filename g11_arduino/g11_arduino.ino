#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <dht.h>
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 9
#define GREEN_LED_PIN 6
#define RED_LED_PIN 2
#define BUZZER_PIN 4
const int LDR_PIN = A1;
const int BLUE_LED_PIN = A3;

dht DHT;
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal lcd(7, 8, 5, 3, A2, A4);

boolean tagAutorizadaLida = false;

unsigned long previousMillisLCD = 0;
const long intervalLCD = 100;  // Intervalo em milissegundos para atualizar o LCD

unsigned long previousMillisRFID = 0;
const long intervalRFID = 100;  // Intervalo em milissegundos para realizar leitura RFID

void setup() 
{
  Serial.begin(9600);
  pinMode(A5, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  lcd.clear();
  digitalWrite(A5, LOW);
}

void loop() 
{
  unsigned long currentMillis = millis();

  // Realizar leitura RFID a cada intervaloRFID de tempo
  if (currentMillis - previousMillisRFID >= intervalRFID) 
  {
    previousMillisRFID = currentMillis;
    leituraRFID();
  }

  // Verificar condições de luminosidade e controlar o LED azul
  

  // Atualizar LCD apenas se a tag autorizada foi lida
  if (tagAutorizadaLida) 
  {
    // Atualizar LCD a cada intervaloLCD de tempo
    if (currentMillis - previousMillisLCD >= intervalLCD) 
    {
      previousMillisLCD = currentMillis;
      atualizaLCD();
      controleLED();
    }
    
  }
  else
  {
      lcd.clear();
      digitalWrite(A5, LOW);
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(BLUE_LED_PIN, LOW);
  }
}

void atualizaLCD() 
{
  noTone(BUZZER_PIN);

  int chk = DHT.read11(A0);
  float Vtemperatura = DHT.temperature;
  float Vhumidade = DHT.humidity;

  //  LCD

  digitalWrite(A5, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(Vtemperatura);
  lcd.setCursor(0, 1);
  lcd.print("Humidade: ");
  lcd.print(Vhumidade);
  lcd.print("%");

  // Verifica temperatura
  if (Vtemperatura >= 22) 
  {
    digitalWrite(RED_LED_PIN, HIGH);
  } 
  else 
  {
    digitalWrite(RED_LED_PIN, LOW);
  }
}

void leituraRFID() 
{
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) 
  {
    String tagID = "";
    for (byte i = 0; i < rfid.uid.size; i++) 
    {
      tagID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      tagID += String(rfid.uid.uidByte[i], HEX);
    }

    tagID.toUpperCase();
    Serial.println("TAG lida: " + tagID);

    if (tagID.equals("43D842BD")) 
    {
      digitalWrite(GREEN_LED_PIN, HIGH);
      delay(2000);
      digitalWrite(GREEN_LED_PIN, LOW);
      
      // Atualizar o estado da tag autorizada
      tagAutorizadaLida = !tagAutorizadaLida;
    } 
    else 
    {
      digitalWrite(GREEN_LED_PIN, LOW);
      tone(BUZZER_PIN, 10); //liga o som do buzzer
      delay(1000);
      noTone(BUZZER_PIN); //desliga o som do buzzer
    }
  }
}

void controleLED() 
{
  int luminosidade = analogRead(LDR_PIN);

  // Defina um valor de limiar com base na leitura do LDR
  int limiarLuminosidade = 10;

  // Verificar se a luminosidade é menor que o limiar
  if (luminosidade < limiarLuminosidade) 
  {
    digitalWrite(BLUE_LED_PIN, HIGH);
  } 
  else 
  {
    digitalWrite(BLUE_LED_PIN, LOW);
  }
}
