/** 
 * "Dijkstras Philosopher problem (https://en.wikipedia.org/wiki/Dining_philosophers_problem):
 * Five silent philosophers sit at a round table with bowls of spaghetti. Forks are placed between each pair of adjacent philosophers.
 * Each philosopher must alternately think and eat. However, a philosopher can only eat spaghetti when they have both left and right forks.
 * Each fork can be held by only one philosopher and so a philosopher can use the fork only if it is not being used by another philosopher.
 * After an individual philosopher finishes eating, they need to put down both forks so that the forks become available to others.
 * A philosopher can only take the fork on their right or the one on their left as they become available and they cannot start
 * eating before getting both forks.
 * Eating is not limited by the remaining amounts of spaghetti or stomach space; an infinite supply and an infinite demand are assumed.
 * The problem is how to design a discipline of behavior (a concurrent algorithm) such that no philosopher will starve; i.e.,
 * each can forever continue to alternate between eating and thinking, assuming that no philosopher can know when others may want to eat or think."
 *
 *
 * The task is that no philosopher should starve to death and non is overfed. So they should take turns eating and thinking...
 */
#include <Arduino.h> 
#include "mbed.h"
#include "rtos.h"

using namespace mbed;
using namespace rtos;

//Semaphore to protect the Serial access
Semaphore printShare(1); 											//protect the Serial interface

//Lets create 5 forks...
//A fork is a Semaphor with count 1
//So a fork can be acquired by only one persomn at a time
//Semapgore is a mbed class (https://os.mbed.com/docs/mbed-os/v6.2/apis/semaphore.html) 
Semaphore fork_1(1);
Semaphore fork_2(1);
Semaphore fork_3(1);
Semaphore fork_4(1);
Semaphore fork_5(1);

//...and pur them in a pool of forks
Semaphore *forkPool[5] = {&fork_1, &fork_2, &fork_3, &fork_4, &fork_5};


//Let's create 5 philosophers. Their job is to think (philosophize)
//and to recharge their batteries for philosophizing (people call this part eating)
Thread philo_1;
Thread philo_2;
Thread philo_3;
Thread philo_4;
Thread philo_5;


//helper for print output
void printHelper(const char* name, const char* state)
{
    String text = String(String(name) + " " + String(state) + " two forks");
    //We are multy threading, so let us protect the Serial access with 
    //a semaphore. You can play with this code too see what happens when the
    //Seraial access is not protected
    printShare.acquire();
    Serial.println(text);
    printShare.release();
}
 
//This is the philosophers work flow
//Thinking, getting forks, eating, releaseing forks and so on... forever.
void philo_thread(const char* args)
{
  //Initialize the philo threads
  int left;
  int right;
  int mySeconds = 0;
  String thePhilo = String(args);
  if (thePhilo.equals(String("Philo_1")))
  {
    //Philo_1 uses fork 0 and 4. He shares them with Philo 2 and 4
    left = 0;
    right = 4;	
  }
  else if (thePhilo.equals(String("Philo_2")))
  {
    //Philo_2 uses fork 1 and 0. He shares them with Philo 1 and 3
    left = 1;
    right = 0;  	
  }
  else if (thePhilo.equals(String("Philo_3")))
  {
    //Philo_3 uses fork 2 and 1. He shares them with Philo 2 and 4
    left = 2;
    right = 1;  	
  }
  else if (thePhilo.equals(String("Philo_4")))
  {
    //Philo_4 uses fork 3 and 2. He shares them with Philo 3 and 5
    left = 3;
    right = 2; 	
  }
  else if (thePhilo.equals(String("Philo_5")))
  {
    //Philo_5 uses fork 4 and 3. He shares them with Philo 4 and 1
    left = 4;
    right = 3;  	
  }
  
  //Do forever... 
  while (true)
  {
    //calculate the next timeout between 2 and 5 seconds
    mySeconds = random(2, 5);
    //think for a while 
    ThisThread::sleep_for(std::chrono::seconds(mySeconds));
    //you are hungry man!
    //try to get your left fork
    //if the fork is just acquired, you wait until it is released and the you can take it
    //A semaphore manages thread access to shared resources
    //If the fork is occupied, you automatically wait until it is released.
    //and when you can fetch it, you go on      	
    forkPool[left]->acquire();
    //try to get your right fork
    //if the fork is just acquired, you wait until it is released and the you can take it
    forkPool[right]->acquire();
    //tell the world you are eating!
    printHelper(args, "acquired");
    //prepare how long you can eat
    mySeconds = random(2, 5);
    //eat!
    ThisThread::sleep_for(std::chrono::seconds(mySeconds));
    //return left fork (so your neighbour Philo can make use of it!)
    forkPool[left]->release();
    //return right fork (so your other neighbour Philo can make use of it!)
    forkPool[right]->release();
    //tell the world you returned your forkes!   
    printHelper(args, "returned");
  }
}

void setup()
{
    //initialize the Serial interface
    Serial.begin(9600);
    //Let us start our philosophers!
    philo_1.start(callback(philo_thread, "Philo_1"));
    philo_2.start(callback(philo_thread, "Philo_2")); 
    philo_3.start(callback(philo_thread, "Philo_3")); 
    philo_4.start(callback(philo_thread, "Philo_4")); 
    philo_5.start(callback(philo_thread, "Philo_5")); 
}

void loop()
{
}