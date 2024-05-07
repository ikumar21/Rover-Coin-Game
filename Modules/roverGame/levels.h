#include "main.h"
typedef struct{
  uint16_t coinGenTime[2];
  uint8_t maxCoins;
  uint8_t maxGameTime;
  uint16_t targetScore;
  
}Level_Info;

//                      min, max, maxCoin, gameTime, targetScore
Level_Info lvl1Info = {{100, 200}, 20, 60, 50};
Level_Info lvl2Info = {{100, 200}, 15, 45, 45};
Level_Info lvl3Info = {{400, 600}, 20, 60, 75};
Level_Info lvl4Info = {{300, 500}, 8, 45, 35};
Level_Info lvl5Info = {{1000, 2000}, 7, 60, 30};
Level_Info lvl6Info = {{1000, 1500}, 5, 60, 35};
Level_Info lvl7Info = {{200, 300}, 20, 60, 125};
Level_Info lvl8Info = {{50, 100}, 1, 15, 8};
Level_Info lvl9Info = {{500, 600}, 2, 45, 15};
Level_Info lvl10Info = {{50, 100}, 1, 30, 10}; 

Level_Info *allLvlInfo[10] =  {&lvl1Info, &lvl2Info, &lvl3Info, &lvl4Info, 
                               &lvl5Info, &lvl6Info, &lvl7Info, &lvl8Info,
                               &lvl9Info, &lvl10Info};
                            



