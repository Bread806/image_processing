#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <random>
#include <algorithm>
using namespace std;

double MF(double* arr){
    double mean=0;
    for (int i=0;i<8;i++)mean+=arr[i];
    return mean/8;
}

int main()
{
    // init rand seed
    //srand( time(NULL) );
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> dis(0.0f, 32.0f);


    FILE* fp = fopen("res\\AIgened_salt.bmp", "rb");
    unsigned char c;
    if (fp == NULL)
        return 0;
    for (int i = 0; i < 10; i++)
    {
        c = getc(fp);
    }
    int s = 0;
    s += getc(fp);
    s += getc(fp) * 256;
    s += getc(fp) * 256 * 256;
    s += getc(fp) * 256 * 256 * 256;
    cout << s << " ";

    for (int i = 0; i < 4; i++)
    {
        c = getc(fp);
    }

    int w = 0;
    w += getc(fp);
    w += getc(fp) * 256;
    w += getc(fp) * 256 * 256;
    w += getc(fp) * 256 * 256 * 256;
    cout << w << " ";

    int h = 0;
    h += getc(fp);
    h += getc(fp) * 256;
    h += getc(fp) * 256 * 256;
    h += getc(fp) * 256 * 256 * 256;
    cout << h << " ";

    for (int i = 0; i < 8; i++)
    {
        c = getc(fp);
    }

    int l = 0;
    l += getc(fp);
    l += getc(fp) * 256;
    l += getc(fp) * 256 * 256;
    l += getc(fp) * 256 * 256 * 256;
    cout << l << " " << endl;

    for (int i = 0; i < s - 38; i++)
    {
        c = getc(fp);
    }

    vector<vector<unsigned char>> imgArr;
    imgArr.resize(h, vector<unsigned char>(w, 0));

    for (int i = 0; i < w * h; i++)
    {
        int x = i % w;
        int y = i / w;
        imgArr[x][h - y - 1] = getc(fp);
    }

    fclose(fp);

    // main parse
    for (int i = 1; i < h-1; i++)
    {
        for (int j = 1; j < w-1; j++)
        {
            double arr[8] = {imgArr[j-1][i-1], imgArr[j-1][i], imgArr[j-1][i+1], imgArr[j][i-1], imgArr[j][i+1], imgArr[j+1][i-1], imgArr[j+1][i], imgArr[j+1][i+1]};
            imgArr[j][i] = MF(arr);
        }
    }

    FILE* fp_r = fopen("res\\AIgened_salt.bmp", "rb");
    FILE* fp_w = fopen("res\\AIgened_MF.bmp", "wb");
    if (fp_r == NULL || fp_w == NULL){
        printf("error while loading file.");
        return 0;
    }

    for (int i = 0; i < s; i++)
    {
        c = getc(fp_r);
        putc(c, fp_w);
    }

    for (int i = h - 1; i >= 0; i--)
    {
        for (int j = 0; j < w; j++)
        {
            putc(imgArr[j][i], fp_w);
        }
    }

    fclose(fp_r);
    fclose(fp_w);
    printf ("done");
    return 0;
}
