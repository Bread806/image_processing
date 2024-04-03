#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#define PI 3.1415926
using namespace std;

void read_img(const char *fileName, vector<vector<unsigned char>> &imgArr)
{
    FILE *fp = fopen(fileName, "rb");

    unsigned char c;
    if (fp == NULL)
        return;
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

    imgArr.resize(h, vector<unsigned char>(w));

    for (int i = 0; i < w * h; i++)
    {
        int x = i % w;
        int y = i / w;
        imgArr[x][h - y - 1] = getc(fp);
    }

    fclose(fp);
}

vector<unsigned char> createHeader(int width, int height)
{
    int headSize = 1078;
    int fileSize = headSize + width * height;
    vector<unsigned char> header(headSize);

    header[0] = 'B';
    header[1] = 'M';
    header[2] = fileSize;
    header[3] = fileSize >> 8;
    header[4] = fileSize >> 16;
    header[5] = fileSize >> 24;
    header[10] = headSize;
    header[11] = headSize >> 8;
    header[12] = headSize >> 16;
    header[13] = headSize >> 24;
    header[14] = 40;
    header[18] = width;
    header[19] = width >> 8;
    header[20] = width >> 16;
    header[21] = width >> 24;
    header[22] = height;
    header[23] = height >> 8;
    header[24] = height >> 16;
    header[25] = height >> 24;
    header[26] = 1;
    header[28] = 8;

    for (int i = 0; i < 256; i++)
    {
        header[54 + i * 4] = (unsigned char)i;
        header[54 + i * 4 + 1] = (unsigned char)i;
        header[54 + i * 4 + 2] = (unsigned char)i;
    }

    return header;
}

template <typename T>
unsigned char Truncate(T pix)
{
    unsigned char ans = 0;
    ans = ((pix < 0) ? 0 : (pix > 255) ? 255
                                       : pix);
    return ans;
}

bool Check(int fi, int fj, int ci, int cj, int img_h, int img_w)
{
    bool ck_fi = (fi < 0) ? false : (fi < img_h) ? true
                                                 : false;
    bool ck_fj = (fj < 0) ? false : (fj < img_w) ? true
                                                 : false;
    bool ck_ci = (ci < 0) ? false : (ci < img_h) ? true
                                                 : false;
    bool ck_cj = (cj < 0) ? false : (cj < img_w) ? true
                                                 : false;
    return (ck_fi & ck_fj & ck_ci & ck_cj);
}

void Transpose(vector<vector<unsigned char>> &imgArr, vector<vector<unsigned char>> &newImg, int img_h, int img_w)
{
    for (int i = 0; i < img_h; i++)
    {
        for (int j = 0; j < img_w; j++)
        {
            double value = imgArr[j][i];
            double newI = cos(10.f * PI / 180.0f) * i - sin(10.0f * PI / 180.f) * j;
            double newJ = cos(10.f * PI / 180.0f) * i + cos(10.0f * PI / 180.f) * j;

            int fi = newI, fj = newJ, ci = fi + 1, cj = fj + 1;
            if (Check(fi, fj, ci, cj, img_h, img_w))
            {
                double lu = (ci - newI) * (cj - newJ) * value;
                double ru = (ci - newI) * (newJ - fj) * value;
                double ld = (newI - fi) * (cj - newJ) * value;
                double rd = (newI - fi) * (newJ - fj) * value;

                newImg[fj][fi] = Truncate(lu + newImg[fj][fi]);
                newImg[cj][fi] = Truncate(ru + newImg[cj][fi]);
                newImg[fj][ci] = Truncate(ld + newImg[fj][ci]);
                newImg[cj][ci] = Truncate(rd + newImg[cj][ci]);
            }
        }
    }
}
int main()
{
    // origin image
    vector<vector<unsigned char>> imgArr;
    read_img("res\\AIgened_gray.bmp", imgArr);
    int img_h = imgArr.size();
    int img_w = imgArr[0].size();

    // down scale image
    vector<vector<unsigned char>> newImg;
    newImg.resize(img_h, vector<unsigned char>(img_w));

    // process
    Transpose(imgArr, newImg, img_h, img_w);

    int sample_h = newImg.size();
    int sample_w = newImg[0].size();

    FILE *fp_w = fopen("res\\traspose.bmp", "wb");

    if (fp_w == NULL)
        return 0;

    vector<unsigned char> my_Header = createHeader(sample_w, sample_h);
    for (int i = 0; i < 1078; i++)
    {
        putc(my_Header[i], fp_w);
    }

    for (int i = sample_h - 1; i >= 0; i--)
    {
        for (int j = 0; j < sample_w; j++)
        {
            putc(newImg[j][i], fp_w);
        }
    }

    fclose(fp_w);
    return 0;
}
