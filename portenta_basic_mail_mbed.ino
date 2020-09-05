/*
 * Usecase: thread 1 (senderThread) sends mails to thread 2 (receiverThread)
 * The mail contains as its payload the desired led (indexed) to switch
 * The receiverThread toggles the desired output
 * All is based on the use of mbed
 *  
 */
#include "mbed.h"

using namespace mbed;
using namespace rtos;
using namespace std;

//Mail
typedef struct {
  int ledId; //ID of LED to toggle
} mail_t;

Mail<mail_t, 16> mail_box;

Thread senderThread;

Thread receiverThread;
DigitalOut redLED(LED1, 1);
DigitalOut greenLED(LED2, 1);
DigitalOut blueLED(LED3, 1);

DigitalOut *ledList[3] = {&redLED, &greenLED, &blueLED}; //array with the LEDs

void sendMail() {
  while (true) {  
    //allocate memory
    mail_t *mail = mail_box.alloc();
    //set the index of the LED to switch
    mail->ledId = random(0, 3);
    //send your mail
    mail_box.put(mail);
    //sleep for a while
    ThisThread::sleep_for(chrono::milliseconds(random(500,2000)));
  }
}

void receiveMail() {
  while (1) {
    osEvent evt = mail_box.get();
    if (evt.status == osEventMail) {
      mail_t *mail = (mail_t *)evt.value.p;
      int index = mail->ledId;
      //get the pointer to the idexed LED
      DigitalOut *ledPtr = ledList[index];
      //and toggle the indexed LED
      ledPtr->write(!ledPtr->read());
      //don't forget to free the allocated memory
      mail_box.free(mail);
    }
  }
}

void setup() {
  //start your threads: a dender and a receiver
  senderThread.start(callback(sendMail));
  receiverThread.start(callback(receiveMail));
}

void loop() {}
