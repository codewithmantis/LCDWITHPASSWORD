#include <LiquidCrystal.h>
#include <dht_nonblocking.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 5;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

int buzzer = 2;
int buttonApin = 3;
int buttonBpin = 4;

String password = "1000000000000000"; 
String input = "";

int col = 0;
int row = 1;

float temperature = 0;
float humidity = 0;

bool unlocked = false;

static bool measure_environment(float *temperature, float *humidity)
{
  static unsigned long measurement_timestamp = millis();

  if (millis() - measurement_timestamp > 3000ul)
  {
    if (dht_sensor.measure(temperature, humidity) == true)
    {
      measurement_timestamp = millis();
      return true;
    }
  }
  return false;
}

void setup() {

  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.print("ENTER PASSWORD");

  pinMode(buzzer, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);
}

void loop() {

  if (!unlocked) {

    if (digitalRead(buttonApin) == LOW) {
      writeBit(0);
      delay(200);
    }

    if (digitalRead(buttonBpin) == LOW) {
      writeBit(1);
      delay(200);
    }

  } else {

    if (measure_environment(&temperature, &humidity)) {

      lcd.setCursor(0,0);
      
      lcd.print(humidity);

      lcd.setCursor(11,0);
      lcd.print((temperature * 1.8) + 32,1);
      lcd.print("F   ");
      lcd.setCursor(0,1);
      lcd.print("WELCOME ADMIN");
    }
  }
}

void writeBit(int bit) {

  lcd.setCursor(col,row);
  lcd.print(bit);

  digitalWrite(buzzer,HIGH);
  delay(40);
  digitalWrite(buzzer,LOW);

  input += String(bit);
  col++;

  if (input.length() == 16) {

    lcd.clear();

    if (input == password) {

      lcd.print("ACCESS GRANTED");
      delay(1500);

      lcd.clear();
      unlocked = true;

    } else {

      lcd.print("ACCESS DENIED");

       if (measure_environment(&temperature, &humidity)) {
        lcd.setCursor(0,1);
        lcd.print("TEMP:");
        lcd.print((temperature*1.8)+32,1);
        lcd.print("F");
      }

  
      delay(1000);
      lcd.clear();
      lcd.print("ENTER PASSWORD");

      input = "";
      col = 0;
    }
  }
}

