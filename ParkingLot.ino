#include <Wire.h>
#include <LiquidCrystal_I2C.h> // A4-SDA, A5-SCL 연결, VCC 5V
#include <Servo.h>
#include <MsTimer2.h>

#define LCD1_I2C_ADDR 0x27  // 기본값, 납땜x
#define LCD2_I2C_ADDR 0x26  // A0만 납땜
#define LCD3_I2C_ADDR 0x25  // A1만 납땜
#define LCD4_I2C_ADDR 0x3b  // A2만 납땜

#define PHOTO_ENT_PIN 2     // 입구 포토센서
#define PHOTO_LOT1_PIN 3    // 1번 주차라인 포토센서
#define PHOTO_LOT2_PIN 4    // 2번 주차라인 포토센서
#define PHOTO_LOT3_PIN 5    // 3번 주차라인 포토센서
#define PHOTO_LOT4_PIN 6    // 4번 주차라인 포토센서
#define SERVO_PIN 7         // 서보모터를 7번에 연결, VCC 5V

#define LED_LOT1_PIN 8      // 1번 주차라인 LED
#define LED_LOT2_PIN 9      // 2번 주차라인 LED
#define LED_LOT3_PIN 10     // 3번 주차라인 LED
#define LED_LOT4_PIN 11     // 4번 주차라인 LED

#define EMPTY true
#define USED false

#define INTERRUPT_TIME_MS 200

bool parkingLot[4] = {EMPTY, EMPTY, EMPTY, EMPTY}; // 각 주차 공간의 입차 여부를 저장할 배열

LiquidCrystal_I2C lcd1(LCD1_I2C_ADDR, 16, 2);
LiquidCrystal_I2C lcd2(LCD2_I2C_ADDR, 16, 2);
LiquidCrystal_I2C lcd3(LCD3_I2C_ADDR, 16, 2);
LiquidCrystal_I2C lcd4(LCD4_I2C_ADDR, 16, 2);
Servo servo;

void initLCD1()
{
    lcd1.init();
    lcd1.backlight();
}

void initLCD2()
{
    lcd2.init();
    lcd2.backlight();
}

void initLCD3()
{
    lcd3.init();
    lcd3.backlight();
}

void initLCD4()
{
    lcd4.init();
    lcd4.backlight();
}

void initAllLCD()
{
    initLCD1();
    initLCD2();
    initLCD3();
    initLCD4();
}

void initServo()
{
    servo.attach(SERVO_PIN);
    servo.write(0); // 0도가 기본상태(닫힌 상태)
}

void initAllPhoto()
{
    pinMode(PHOTO_ENT_PIN, INPUT);
    pinMode(PHOTO_LOT1_PIN, INPUT);
    pinMode(PHOTO_LOT2_PIN, INPUT);
    pinMode(PHOTO_LOT3_PIN, INPUT);
    pinMode(PHOTO_LOT4_PIN, INPUT);
}

void initAllLED()
{
    pinMode(LED_LOT1_PIN, OUTPUT);
    pinMode(LED_LOT2_PIN, OUTPUT);
    pinMode(LED_LOT3_PIN, OUTPUT);
    pinMode(LED_LOT4_PIN, OUTPUT);
}

void writeLCD1(String line1, String line2)
{
    lcd1.setCursor(0,0);
    lcd1.print(line1);
    lcd1.setCursor(0,1);
    lcd1.print(line2);
}

void writeLCD2(String line1, String line2)
{
    lcd2.setCursor(0,0);
    lcd2.print(line1);
    lcd2.setCursor(0,1);
    lcd2.print(line2);
}

void writeLCD3(String line1, String line2)
{
    lcd3.setCursor(0,0);
    lcd3.print(line1);
    lcd3.setCursor(0,1);
    lcd3.print(line2);
}

void writeLCD4(String line1, String line2)
{
    lcd4.setCursor(0,0);
    lcd4.print(line1);
    lcd4.setCursor(0,1);
    lcd4.print(line2);
}

void writeAllLCD(String line1, String line2)
{
    writeLCD1(line1, line2);
    writeLCD2(line1, line2);
    writeLCD3(line1, line2);
    writeLCD4(line1, line2);
}

void writeServo(unsigned angle)
{
    servo.write(angle);
}

void setup() 
{
    initAllLCD();
    initAllPhoto();
    initAllLED();
    initServo();
    writeLCD1("", "");
    writeLCD2("", "");
    writeLCD3("", "");
    writeLCD4("", "");
    MsTimer2::set(INTERRUPT_TIME_MS, interruptFunction);
    MsTimer2::start();
}

void loop() 
{
    if(digitalRead(PHOTO_ENT_PIN) == 1) // 입구에 설치된 포토센서에서 물체가 감지되면
    {
        int parkingLotIndex = -1;
        for(int i=0; i<4; i++) // 주차 할 자리 찾기
        {
            if(parkingLot[i] == EMPTY)
            {
                parkingLotIndex = i; // 발견된 주차 공간의 인덱스
                break;
            }
        }

        if(parkingLotIndex >= 0) // 주차 공간이 있을때 차단기를 열어주고 LED에 표시(0보다 작으면 주차 공간을 찾지 못한것)
        {
            if(parkingLotIndex == 0) // 주차공간1 구역인 경우 - RED
            {
                writeAllLCD("RED", "B-1");
            }
            else if(parkingLotIndex == 1) // 주차공간2 구역인 경우 - GREEN
            {
                writeAllLCD("GREEN", "B-2");
            }
            else if(parkingLotIndex == 2) // 주차공간3 구역인 경우 - BLUE
            {
                writeAllLCD("BLUE", "A-1");
            }
            else if(parkingLotIndex == 3) // 주차공간4 구역인 경우 - YELLOW
            {
                writeAllLCD("YELLOW", "A-2");
            }
            writeServo(90); // 차단기를 90도 들어올림
            delay(5000); // 5초간 대기후
            writeServo(0); // 차단기를 닫음
        }
    }
}

void interruptFunction() // 100ms에 한번씩 수행되는 function. 주차 공간에 차량이 있는지 검사하여 LED를 제어한다.
{
    digitalWrite(LED_LOT1_PIN, digitalRead(PHOTO_LOT1_PIN) == 0 ? HIGH : LOW); // 주차공간1 LED제어
    parkingLot[0] = digitalRead(PHOTO_LOT1_PIN) == 0 ? EMPTY : USED; // 주차공간1 주차장 상태변수 제어

    digitalWrite(LED_LOT2_PIN, digitalRead(PHOTO_LOT2_PIN) == 0 ? HIGH : LOW); // 주차공간2 LED제어
    parkingLot[1] = digitalRead(PHOTO_LOT2_PIN) == 0 ? EMPTY : USED; // 주차공간2 주차장 상태변수 제어

    digitalWrite(LED_LOT3_PIN, digitalRead(PHOTO_LOT3_PIN) == 0 ? HIGH : LOW); // 주차공간3 LED제어
    parkingLot[2] = digitalRead(PHOTO_LOT3_PIN) == 0 ? EMPTY : USED; // 주차공간3 주차장 상태변수 제어

    digitalWrite(LED_LOT4_PIN, digitalRead(PHOTO_LOT4_PIN) == 0 ? HIGH : LOW); // 주차공간4 LED제어
    parkingLot[3] = digitalRead(PHOTO_LOT4_PIN) == 0 ? EMPTY : USED; // 주차공간4 주차장 상태변수 제어
}
