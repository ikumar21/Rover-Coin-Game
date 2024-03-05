//Code in systick interrupt
#define SCHEDULE_CODE \
  countFlag3Ms++; \
  countFlag10Ms++; \
  countFlag25Ms++; \
  countFlag100Ms++; \
  countFlag500Ms++; \
  countFlag1000Ms++; \
  if(countFlag3Ms==3){ \
    flag3MsPassed=true; \
    countFlag3Ms=0; \
  } \
  if(countFlag10Ms==10){ \
    flag10MsPassed=true; \
    countFlag10Ms=0; \
  } \
  if(countFlag25Ms==25){ \
    flag25MsPassed=true; \
    countFlag25Ms=0; \
  } \
  if(countFlag100Ms==100){ \
    flag100MsPassed=true; \
    countFlag100Ms=0; \
  } \
  if(countFlag500Ms==500){ \
    flag500MsPassed=true; \
    countFlag500Ms=0; \
  } \
  if(countFlag1000Ms==1000){ \
    flag1000MsPassed=true; \
    countFlag1000Ms=0; \
  } \
  
//Var for Flags
#define SCHEDULE_VAR \
uint16_t countFlag3Ms = 0; \
uint16_t countFlag10Ms = 0; \
uint16_t countFlag25Ms = 0; \
uint16_t countFlag100Ms = 0; \
uint16_t countFlag500Ms = 0; \
uint16_t countFlag1000Ms = 0; \
 \
uint8_t flag3MsPassed = false; \
uint8_t flag10MsPassed = false; \
uint8_t flag25MsPassed = false; \
uint8_t flag100MsPassed = false; \
uint8_t flag500MsPassed = false; \
uint8_t flag1000MsPassed = false; \

  
/*
Varibles in main.c:
//Timing Flags
extern uint8_t flag3MsPassed;
extern uint8_t flag10MsPassed;
extern uint8_t flag25MsPassed;
extern uint8_t flag100MsPassed;
extern uint8_t flag500MsPassed;
extern uint8_t flag1000MsPassed;


Format in while loop:

if(flag3MsPassed){
      flag3MsPassed=false;
      //Every 3 ms Task:
            
}
if(flag10MsPassed){
      flag10MsPassed=false;
      //Every 10 ms Task:
            
}
if(flag25MsPassed){
      flag25MsPassed=false;
      //Every 25 ms Task:
            
}

if(flag100MsPassed){
      flag100MsPassed=false;
      //Every 100 ms Task:
            
}
if(flag500MsPassed){
      flag500MsPassed=false;
      //Every 500 ms Task:
            
}
if(flag1000MsPassed){
      flag1000MsPassed=false;
      //Every 1000 ms Task:
            
}

*/

