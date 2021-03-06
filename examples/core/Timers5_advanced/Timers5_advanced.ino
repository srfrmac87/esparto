/*
 MIT License

Copyright (c) 2019 Phil Bowles <esparto8266@gmail.com>
                      blog     https://8266iot.blogspot.com     
                support group  https://www.facebook.com/groups/esp8266questions/
                
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 *    Demonstrates Esparto's wide variety of timing functions and the ability to chain, mix and match
 *    to build up complex timing sequences without the need for delay() millis() or other sub-standard 
 *    techniques *    
 */
#include <ESPArto.h>
ESPArto  Esparto;

char* genres[]={
  "disco",
  "punk",
  "jazz",
  "folk",
  "blues",
  "hip-hop",
  "house",
  "garage",
  "indie"
};
//
void everyMinute(){
  Serial.printf("T=%d 1 min tick FH=%d\n",millis()/1000,ESP.getFreeHeap());
}
void argue(const char* a,const char* b){
  Serial.printf("T=%d %s was %s\n",millis()/1000,a,b);
}
void pulsePin(uint8_t pin,unsigned int ms,bool active){
  digitalWrite(pin,active);
  Esparto.once(ms,bind([](uint32_t pin,uint8_t hilo){ digitalWrite(pin,hilo); },pin,active ? LOW:HIGH));
}

#define DIT 80
#define DAH 200
#define GAP DIT*2.5
#define PAUSE 3*DAH

void SOS(){
  Esparto.every(5000,[](){
    Esparto.nTimes( 3, GAP, bind(pulsePin,LED_BUILTIN,DIT,LOW),
//    onComplete...
      [](){
        Esparto.once(PAUSE,[](){
          Esparto.nTimes( 3, GAP*2, bind(pulsePin,LED_BUILTIN,DAH,LOW),
  //      onComplete...
          [](){
            Esparto.once(PAUSE,[](){
              Esparto.nTimes( 3, GAP, bind(pulsePin,LED_BUILTIN,DIT,LOW));
            });
          });
        });
      });
    });
}

void setupHardware() {
  static uint32_t edwin;
  static uint32_t marvin;
  static uint32_t heartbeat;
  
  static uint32_t j=10;
  Serial.begin(74880);
  Serial.printf("Esparto %s\n",__FILE__);

  Esparto.Output(LED_BUILTIN);
  
  Serial.println("Let's get it on: https://www.youtube.com/watch?v=x6QZn9xiuOE");

  heartbeat=Esparto.every(1000,bind(pulsePin,LED_BUILTIN,100,LOW));
  edwin=Esparto.every(1000,bind(argue,"Edwin Starr","the best"));
  Esparto.onceRandom(25000,15000,[](){
    Serial.printf("T=%d '25 Miles' is the best ever: https://www.youtube.com/watch?v=hFredbE3goM\n",millis()/1000);
    Serial.printf("T=%d Not forgetting: 'Stop her on Sight (S.O.S)'! https://www.youtube.com/watch?v=RwjLGqQYBBk\n",millis()/1000);
    Esparto.cancel(edwin);
    Esparto.cancel(heartbeat); // stop the heartbeat flashing
    SOS(); // and flash S-O-S
    Serial.printf("T=%d Check the LED it's flashing S-O-S in morse code every 5 seconds...\n",millis()/1000);
  });
  
    Esparto.everyRandom(5000,10000,[](){Serial.printf("T=%d (annoying %s music every 5-10 seconds)\n",millis()/1000,genres[random(0,8)]); }); 
    Esparto.once(10000,[](){
        Serial.printf("T=%d Stop arguing! They were both great, but Edwin WAS better...\n",millis()/1000);
        Esparto.cancel(marvin);
        },
      [](){ 
        Serial.printf("T=%d I still say Marvin had the better voice...\n",millis()/1000);
        Serial.printf("T=%d ..listen to 'Heard it through the grapevine' https://www.youtube.com/watch?v=hajBdDM2qdg\n",millis()/1000);
        } // "chain" function - called when "once" completes
      );
    // What "one second ticker" ? we haven't created one yet!
    // It's important to realise that none of the called functions run NOW
    Esparto.queueFunction([](){ Serial.println("...unless we do THIS..."); });
    // so the order in which you create the timers is largely irrelevant...
    // ...but the order in which the timers "fire" IS important!
    // try it yourself - rearrange all "the top-level" calls in any order you
    // like and the sketch output will still be the same
    Esparto.every(60000,everyMinute);
    // THAT "one second ticker" that will get cancelled by the "10 second one-shot" a few lines ago...   
    marvin=Esparto.every(1000,bind(argue,"Marvin Gaye","better"));
    // Stop the 1-second timer after 10 secs      
    // at 57 to 59 seconds after T0, we will start some other timers...
    // (no reason why a timer routine can't "chain in" other timers!)
    // 
    // Let's call this "New T0" - we can't predict when it will actually occur...
    // It will start another timer which will run 5 times with a second in between each, it wil run from New T0 -> New T0+5
    // ...somewhere round the third or fourth "tick" - it will get mixed in with the 1 minute timer we started earlier
    // It will also start another timer which will begin somewhere in the middle of the previous one
    // i.e. the outputs from the two will be mixed up (with the minute ticker too as mentioned)
    // ...not forgetting that at any time that annoying ping could also pop up! 
    Esparto.onceRandom(57000,59000,[](){
      Serial.printf("T=%d between 57 and 59 seconds after startup...New T0\n",millis()/1000);
      Esparto.nTimes(3,1000,[](){
        static int count=1;
        Serial.printf("T=%d Motown Chartbusters Volume %d\n",millis()/1000,count++);
        },
        [](){ // "chain functions - runs when Esparto.nTimes(3... finishes
        Serial.printf("T=%d ...or maybe 'Chain of Fools' by Aretha Franklin...https://www.youtube.com/watch?v=hrcUNChhOP0\n",millis()/1000);
        // and can run other functions that can chain in other functions that can...and so on...
        Esparto.onceRandom(2000,3000,[](){ Serial.printf("T=%d Was that an album or just a single?\n",millis()/1000); },
          bind(argue,"Now, Aretha...SHE","the greatest!")
          );
        }
      );
      
      Serial.printf("T=%d 2-3 sec after New T0 (i.e. at 59 < T < 62), we discuss the greatest album of all time\n",millis()/1000);
      Serial.printf("T=%d while Edwin Starr sings '25 miles'\n",millis()/1000);
      Esparto.onceRandom(2000,3000,[](){
        Esparto.nTimes(10,1000,bind(
          [](uint32_t n) {// 10x, 1sec apart
            uint32_t& remaining=(*reinterpret_cast<uint32_t*>(n));
            Serial.printf("T=%d %d! (%d More miles...)\n",millis()/1000,remaining,remaining--);
          }
          ,(uint32_t) &j));      
      });
    });
    // That final "count from 0 to 9" timer will start between 59 and 62 seconds and run for another 10 secs
    // it should finish then between 69 and 72 seconds
    // ...except that it won't get that far because of what we are about to do next...
    // between 65 and 70 seconds (right in the middle of counting from 0 to 9...we stop ALL timers
    // ..including the one flashing the builtin in led
    Esparto.onceRandom(65000,70000,[](){
      Serial.println("No more, please!");
      Esparto.queueFunction([](){ Serial.printf("Silence is golden - until the next time! FH=%d\n",ESP.getFreeHeap()); });
    // and FINALLY(?) after a couple of minutes silence, let's run this whole thing again....
      Esparto.cancelAll([](){ 
        j=10; // or Edwin Starr will be walking backwards!
        Esparto.once(90000,setupHardware); 
        });
  });
}
