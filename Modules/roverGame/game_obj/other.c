void printMatrix(uint32_t * matrix, uint8_t row, uint8_t col){
    for (uint8_t i =0; i<row;i++){
        printf("{"); 
        for(uint8_t j = 0; j<col;j++){
            // if((matrix[i*col+j]>>16)<9)
            //     printf("0");
            // printf("%x,",(matrix[i*col+j]>>16));

            printf("%d,",(matrix[i*col+j]));
        }
        printf("},\n");
    }
}

//int main2(){
//    uint16_t angle = 45;
//    Obj_Disp spider1 = {0,0,0,0,0,0,angle,0,0,0,0,0,SPIDER,0,0};
//
//    allObjects[0]=spider1;
//    InitializeObjDisp(1);
//    // updateObj(&spider1);
//    SetObjectColor(objPixelSpace, &spider1);
//    printMatrix(objPixelSpace,spider1.height,spider1.width);
//
//    return 0;
//}

uint8_t spiderStraightColorType[15][10] = {
{3,3,3,0,0,3,3,3,3,3},
{3,3,3,0,3,3,0,0,3,3},
{0,0,3,0,3,3,0,3,3,3},
{0,3,0,0,3,3,0,3,0,0},
{0,0,0,0,0,0,0,0,0,3},
{3,0,0,1,1,0,0,0,3,3},
{0,0,0,1,1,0,0,3,3,3},
{0,0,0,0,0,0,0,2,3,3},
{0,0,0,1,1,0,0,3,3,3},
{3,0,0,1,1,0,0,0,3,3},
{0,0,0,0,0,0,0,0,0,3},
{0,3,0,0,3,3,0,3,0,0},
{0,0,3,0,3,3,0,3,3,3},
{3,3,3,0,3,3,0,0,3,3},
{3,3,3,0,0,3,3,3,3,3}};

uint8_t spiderAngledColorType[12][12] = {
{3,3,0,0,3,0,0,3,3,3,3,3},
{3,3,0,3,3,0,3,0,0,3,3,3},
{3,3,0,0,3,0,3,0,3,3,3,3},
{0,3,0,0,0,0,0,0,2,3,0,3},
{0,3,0,1,1,0,0,0,0,0,0,3},
{0,0,0,1,1,0,0,0,0,3,3,0},
{3,3,0,0,0,0,0,0,0,0,0,0},
{3,3,3,0,0,0,1,1,0,3,3,3},
{3,3,3,0,0,0,1,1,0,0,3,0},
{3,3,3,3,3,0,0,0,0,0,0,0},
{3,3,3,3,3,3,0,3,3,3,3,3},
{3,3,3,3,3,3,0,0,0,3,3,3}};


uint8_t rockStraightColorType[5][5] = {
{1,1,1,1,1},
{1,1,1,1,1},
{1,1,1,1,1},
{1,1,1,1,1},
{1,1,1,1,1}};


//0xF8F8F8 is background color
uint32_t spiderColors[4] = {0x040404,0xFCFC00,0x2C2C2C,0xF8F8F8};
uint32_t laserColors[1] = {0x040404};
uint32_t webColors[1] = {0x040404};
uint32_t rockColors[2] = {0x040404,0xF8F8F8};