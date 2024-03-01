#include <iostream>
#include <cmath>
using namespace std;
#define min_f(a, b, c)  (fminf(a, fminf(b, c)))
#define max_f(a, b, c)  (fmaxf(a, fmaxf(b, c)))
float matrix[3][3] = 
    {
        { 0.299,   0.587,  0.114},
        {-0.169, -0.331,  0.5},
        { 0.5,    -0.419, -0.081}
    };

float matrixToRBG[3][3] = 
    {
        { 1,   -0.00093,  1.401687},
        { 1,   -0.3437,  -0.71417},
        { 1,    1.77216,  0.00099}
    };

void rgb2yuv(const unsigned char &src_r, const unsigned char &src_g, const unsigned char &src_b, unsigned char &dst_y, unsigned char &dst_u, unsigned char &dst_v)
{
    float RGB[3] = {src_r, src_g , src_b };
    float YUV[3] = {0.0,0.0,0.0};

    for (int i=0; i<3; i++){ //矩陣相乘
        for (int j=0; j<3; j++){
            YUV[i] += matrix[i][j]*RGB[j];
        }
    }
    YUV[0]+=0;YUV[1]+=128;YUV[2]+=128;

    float y = (YUV[0]); 
    if (y<0)y=0;else if (y>255)y=255; dst_y = (unsigned char)(y); 
    float u = (YUV[1]); 
    if (u<0)u=0;else if (u>255)u=255; dst_u = (unsigned char)(u); 
    float v = (YUV[2]); 
    if (v<0)v=0;else if (v>255)v=255; dst_v = (unsigned char)(v); 

}

void yuv2rgb(const unsigned char &src_y, const unsigned char &src_u, const unsigned char &src_v, unsigned char &dst_r, unsigned char &dst_g, unsigned char &dst_b){
    float YUV[3] = {src_y , src_u , src_v};
    float RGB[3] = {0.0,0.0,0.0};
    YUV[0]+=0;YUV[1]-=128;YUV[2]-=128;

    for (int i=0; i<3; i++){  //矩陣相乘
        for (int j=0; j<3; j++){
            RGB[i] += matrixToRBG[i][j]*YUV[j];
        }
    }
    float r = (RGB[0]); if (r<0)r=0;else if (r>255)r=255;
    float g = (RGB[1]); if (g<0)g=0;else if (g>255)g=255;
    float b = (RGB[2]); if (b<0)b=0;else if (b>255)b=255;
    dst_r = (unsigned char)(r);  
    dst_g = (unsigned char)(g); 
    dst_b = (unsigned char)(b); 
}


int main(){
    unsigned char r,g,b,y,u,v;
    r=255; g=174; b=201;
    y=0;   u=0;   v=0;
    rgb2yuv(r,g,b,y,u,v);
    printf ("rgb2yuv : %d , %d , %d\n",y,u,v);

    r=0;g=0;b=0;
    y=255;u=174;v=201;
    yuv2rgb(y,u,v,r,g,b);
    printf ("yuv2rgb : %d , %d , %d",r,g,b);

    return 0;
}
