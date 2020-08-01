#include <Arduino.h>
#include "mbed.h"

using namespace mbed;
using namespace rtos;
using namespace std;

DigitalOut LD1(PK_5); // red
DigitalOut LD2(PK_6); // green
DigitalOut LD3(PK_7); // blue

Thread blinkerThread1;
Thread blinkerThread2;
Thread blinkerThread3;

EventFlags event_flags;

Mutex stdio_mutex;
Mutex eventFlagsMutex;

void blinkerR()
{
  int actFlag;
  while (true)
  {
    LD1 = 0;   // red on
    stdio_mutex.lock();
    Serial.println("LED_RED ON");
    stdio_mutex.unlock();
    ThisThread::sleep_for(std::chrono::milliseconds(250));
    LD1 = 1;
    stdio_mutex.lock();
    Serial.println("LED_RED OFF");
    stdio_mutex.unlock();    
    eventFlagsMutex.lock();
    actFlag = event_flags.get();
    event_flags.set(actFlag | 1);
    eventFlagsMutex.unlock();
    event_flags.wait_all(4);
  }
}

void blinkerG()
{
  uint32_t actFlag;
  while (true)
  {
    event_flags.wait_all(1);
    LD2 = !LD2;   // red on
    stdio_mutex.lock();
    Serial.println("LED_GREEN ON");
    stdio_mutex.unlock();
    ThisThread::sleep_for(std::chrono::milliseconds(500));
    LD2 = 1;
    stdio_mutex.lock();
    Serial.println("LED_GREEN OFF");
    stdio_mutex.unlock();
    eventFlagsMutex.lock();
    actFlag = event_flags.get();
    event_flags.set(actFlag | 2);
    eventFlagsMutex.unlock();
  }
}

void blinkerB()
{
  int actFlag;
  while (true)
  {
    event_flags.wait_all(2);
    LD3 = !LD3;   // red on
    stdio_mutex.lock();
    Serial.println("LED_BLUE ON");
    stdio_mutex.unlock();
    ThisThread::sleep_for(std::chrono::milliseconds(1000));
    LD3 = 1;
    stdio_mutex.lock();
    Serial.println("LED_BLUE OFF");
    stdio_mutex.unlock();
    eventFlagsMutex.lock();
    actFlag = event_flags.get();
    event_flags.set(actFlag | 4);
    eventFlagsMutex.unlock();
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  LD1 = 1;
  LD2 = 1;
  LD3 = 1;
  blinkerThread1.start(blinkerR);
  blinkerThread2.start(blinkerG);
  blinkerThread3.start(blinkerB);
}

void loop()
{
  stdio_mutex.lock();
  Serial.println("LOOP Pass");
  stdio_mutex.unlock();
  digitalWrite(7, HIGH);   
  ThisThread::sleep_for(std::chrono::milliseconds(1000)); 
  digitalWrite(7, LOW); 
  ThisThread::sleep_for(std::chrono::milliseconds(4000)); 
}
