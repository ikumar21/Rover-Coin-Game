#include "main.h"
typedef struct{
  uint16_t coinGenTime[2];
  uint8_t maxCoins;
  uint8_t maxGameTime;
  uint16_t targetScore;
  
}Level_Info;

Level_Info lvl1Info = {{50, 100}, 20, 60, 100};
Level_Info lvl2Info = {{50, 100}, 20, 45, 200};
Level_Info lvl3Info = {{50, 100}, 15, 60, 300};
Level_Info lvl4Info = {{50, 100}, 8, 45, 200};
Level_Info lvl5Info = {{50, 100}, 8, 60, 300};
Level_Info lvl6Info = {{50, 100}, 5, 60, 200};
Level_Info lvl7Info = {{50, 100}, 20, 60, 400};
Level_Info lvl8Info = {{50, 100}, 3, 30, 50};
Level_Info lvl9Info = {{50, 100}, 2, 45, 50};
Level_Info lvl10Info = {{50, 100}, 1, 30, 10};





