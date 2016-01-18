// отправка данных через uart на  mqtt. галка "Disable send name topic" снята

#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

unsigned long loopTime;
#define RECV_PIN 5
#define REL1 3 // Реле 1
#define outpin 2 // Реле 2
IRrecv irrecv(RECV_PIN);
decode_results results;
Adafruit_BMP085 bmp;
#define interval 10000 // интервал отправки данных с датчиков
#define readbufsize 100 // максимальный размер буфера 
 
char *buf;
 
void setup()
{
  pinMode(REL1, OUTPUT);
  digitalWrite(REL1, LOW);
  pinMode(outpin, OUTPUT);
  digitalWrite(outpin, LOW);
  pinMode(A3, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
Serial.begin(115200); // задаём скорость UART
   irrecv.enableIRIn(); // Start the receiver
   !bmp.begin();
   buf = new char[readbufsize];
}

void loop()
{
unsigned long currentTime = millis();           // считываем время, прошедшее с момента запуска программы
  if (currentTime >= (loopTime + interval)) {    // сравниваем текущий таймер с переменной loopTime

// чтение датчиков и отправка данных в UART  
  int gas = analogRead(A7);
  int flameOrig = analogRead(A6);
  int rainOrig = analogRead(A3);
  byte flame = 0;
  byte rain = 0;
  flame = map(flameOrig, 0, 1023, 1023, 0);
  rain = map(rainOrig, 0, 1023, 1023, 0);
 /*
  Serial.print("kotel/uptime ");
  Serial.println(millis()/1000);//Пишет на странице время работы в минутах
*/  
  Serial.print("kotel/gas ");
  Serial.println(gas, DEC);
  Serial.print("kotel/flame ");
  Serial.println(flame, DEC);
  Serial.print("kotel/rain ");
  Serial.println(rain, DEC);
  Serial.print("kotel/bmpt ");
  Serial.println(bmp.readTemperature());
  Serial.print("kotel/bmph ");
  Serial.println(bmp.readPressure() / 133.3);
  Serial.print("kotel/REL1 ");
  Serial.println(digitalRead(REL1));
  Serial.print("kotel/outpin ");
  Serial.println(digitalRead(outpin));
        // IR Reciver
        if (irrecv.decode(&results)) {
          Serial.print("kotel/irrecv ");
          Serial.println(results.value, HEX);
         irrecv.resume(); // Receive the next value
         }
      loopTime = currentTime;
    }
 // прием команд с парсингом с вЫключенной опцией "Disable send name topic" в ESP
// с поддержкой atoi - может быть любое число
int i = 0;
  while (Serial.available() && (i < readbufsize)) {
    buf[i++] = Serial.read();
    delay(2);
  }
  
  if (i) { // в UART что-то пришло. обрабатываем
char *pdata = NULL;
char lwt[30];

// пишем в топик email/kotel/gpioset 2 для установки 2 пина в единицу
// не поддерживается mqtt flymon
pdata = NULL;
    sprintf(lwt,"kotel/gpioset ");
        pdata = (char *)strstr(buf,lwt);
 
        if (pdata != NULL) {
        pdata+= strlen(lwt);
uint8_t pin = atoi(pdata);
  digitalWrite(pin, HIGH);
     }
     
// пишем в топик email/kotel/gpioclear 2 для установки 2 пина в ноль
// не поддерживается mqtt flymon
pdata = NULL;
    sprintf(lwt,"kotel/gpioclear ");
        pdata = (char *)strstr(buf,lwt);
 
        if (pdata != NULL) {
        pdata+= strlen(lwt);
uint8_t pin = atoi(pdata);
 digitalWrite(pin, LOW); 
     }
 
 // пишем в топик email/kotel/gpio3 вкл/выкл REL1
 pdata = NULL;
    sprintf(lwt,"kotel/gpio3 ");
        pdata = (char *)strstr(buf,lwt);
 
        if (pdata != NULL) {
        pdata+= strlen(lwt);
uint8_t pin = atoi(pdata);
 digitalWrite(REL1,pin); 
     }    
 // пишем в топик email/kotel/gpio2 вкл/выкл outpin
 pdata = NULL;
    sprintf(lwt,"kotel/gpio2 ");
        pdata = (char *)strstr(buf,lwt);
 
        if (pdata != NULL) {
        pdata+= strlen(lwt);
uint8_t pin = atoi(pdata);
 digitalWrite(outpin,pin); 
     }
      
    memset(buf, 0, readbufsize);
    i = 0;
  } 
}
