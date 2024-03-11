#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <random>
using namespace std;

int main()
{
    // init rand seed
    random_device rd;
    mt19937 gen(rd());
    // random noise
    uniform_int_distribution<> dis(-64, 64);



    FILE* fp = fopen("res\\AIgened_gray.bmp", "rb");
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
    imgArr.resize(h, vector<unsigned char>(w));

    for (int i = 0; i < w * h; i++)
    {
        int x = i % w;
        int y = i / w;
        imgArr[x][h - y - 1] = getc(fp);
    }

    fclose(fp);

    // main parse
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            double tmp = dis(gen);
            double pix = imgArr[j][i]-tmp;
            imgArr[j][i] = (pix<0)? 0 :(pix>255)? 255 : (pix);
        }
    }

    FILE* fp_r = fopen("res\\AIgened_gray.bmp", "rb");
    FILE* fp_w = fopen("res\\AIgened_random.bmp", "wb");
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

    return 0;
}
