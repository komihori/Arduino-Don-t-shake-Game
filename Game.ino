#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd{4, 6, 10, 11, 12, 13};

int16_t oax, oay, oaz;

int BUTTON[] = {14, 15, 16, 17};
int randomDecrease[] = {0, 0, 0, 0};
int BUTTONNUM;
bool key[] = {false, false, false, false};

int initTime = 600;

int gameScore = 600;

bool gameOver = false;

float upperNum = 0.4;

void setup() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SetUp");

    BUTTONNUM = sizeof(BUTTON) / sizeof(BUTTON[0]);

    for (int i = 0; i < BUTTONNUM; i++) {
        pinMode(BUTTON[i], INPUT_PULLUP);
        randomDecrease[i] = random(5, 50);        
    }

    Wire.begin();

    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 14, true);

    oax = Wire.read() << 8 | Wire.read();
    oay = Wire.read() << 8 | Wire.read();
    oaz = Wire.read() << 8 | Wire.read();

    for (int i = 3; i >= 0; i--){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(i);
        delay(1000);
    }
}

void loop() {
    lcd.clear();
    lcd.setCursor(0, 0);

    if (gameOver){
        lcd.setCursor(2, 0);
         lcd.print("You Are Dead!");
         lcd.setCursor(6, 1);
         lcd.print("(*.*)");
         if(digitalRead(BUTTON[0]) == 0 && digitalRead(BUTTON[3]) == 0){
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("Reset now.");
             gameOver = false;
             gameScore = initTime;
             for (int i = 0; i < BUTTONNUM; i++)
                randomDecrease[i] = random(5, 50);
             delay(500);
             for (int i = 3; i >= 0; i--){
                 lcd.clear();
                 lcd.setCursor(0, 0);
                 lcd.print(i);
                 delay(1000);
             }
             return;
         }
         delay(100);
         return;
    }


    lcd.print((float)gameScore / 10.0);
    lcd.print(" ");
    for (int i = 0; i < BUTTONNUM; i++) {
        if (digitalRead(BUTTON[i]) == 0) {
            if (!key[i]) {
                key[i] = true;
                gameScore -= randomDecrease[i];
            }
        } else {
            key[i] = false;
        }
    }

    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 14, true);

    int16_t ax, ay, az;

    ax = Wire.read() << 8 | Wire.read();
    ay = Wire.read() << 8 | Wire.read();
    az = Wire.read() << 8 | Wire.read();

    float ogX, ogY, ogZ;
    ogX = abs((ax - oax) / 16384.0);
    ogY = abs((ay - oay) / 16384.0);
    ogZ = abs((az - oaz) / 16384.0);

    lcd.setCursor(0, 1);
    lcd.print("Pow:");
    lcd.print(max(max(ogX, ogY), ogZ));

    if (ogX > upperNum || ogY > upperNum || ogZ > upperNum || gameScore <= 0)
        gameOver = true;
    
    if(gameScore < 0)
        gameScore = 0;

    gameScore--;

    delay(100);
}