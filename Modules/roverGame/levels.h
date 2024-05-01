#include "main.h"
typedef struct{
  uint16_t coinGenTime[2];
  uint8_t maxCoins;
  uint8_t maxGameTime;
  uint16_t targetScore;
  
}Level_Info;

Level_Info lvl1Info = {{2000, 4000}, 20, 60, 25};
Level_Info lvl2Info = {{3000, 5000}, 10, 45, 50};
Level_Info lvl3Info = {{3000, 5000}, 10, 60, 300};
Level_Info lvl4Info = {{3000, 5000}, 8, 45, 200};
Level_Info lvl5Info = {{3000, 5000}, 8, 60, 300};
Level_Info lvl6Info = {{3000, 5000}, 5, 60, 200};
Level_Info lvl7Info = {{3000, 5000}, 20, 60, 400};
Level_Info lvl8Info = {{3000, 5000}, 3, 30, 50};
Level_Info lvl9Info = {{3000, 5000}, 2, 45, 50};
Level_Info lvl10Info = {{3000, 5000}, 1, 30, 10};

Level_Info *allLvlInfo[10] =  {&lvl1Info, &lvl2Info, &lvl3Info, &lvl4Info, 
                               &lvl5Info, &lvl6Info, &lvl7Info, &lvl8Info,
                               &lvl9Info, &lvl10Info};
                            



