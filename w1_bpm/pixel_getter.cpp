#include <iostream>
using namespace std;
FILE* fp = fopen("res\\W1\\onered.bmp", "rb");
unsigned char c;
void display(int x, int y){
    int W = 50 * 3 + 2; //150 + padding
    int H = 50;
    int pop = W*(H-y-1) + x*3;
    for (int i=0; i<pop ;i++){
        c = getc(fp);
    }
    unsigned char b = getc(fp); 
    unsigned char g = getc(fp); 
    unsigned char r = getc(fp); 

    printf ("RGB : %02X %02X %02X", r,g,b);
}
int main(){
    
    if (fp == NULL){
        printf("error when open a file.");
        return 0;
    }
    for (int i=0; i<10 ;i++)c = getc(fp);
    unsigned char begin = char(getc(fp)-11);
    for (int i=0 ; i<int(begin) ;i++)c = getc(fp);

    // --- //

    int x = 0, y=1; // red
    display(x, y);
    
    return 0;
}
