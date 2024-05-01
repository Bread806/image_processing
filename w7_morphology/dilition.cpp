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

bool Check(vector<vector<unsigned char>> &imgArr, int j, int i)
{
    // black = 0 , white = 255
    // true if one of black around
    return !(imgArr[j - 1][i - 1] | imgArr[j - 1][i] | imgArr[j - 1][i + 1] |
             imgArr[j][i - 1] | imgArr[j][i + 1] |
             imgArr[j + 1][i - 1] | imgArr[j + 1][i] | imgArr[j + 1][i + 1]);
}

void Morphology(vector<vector<unsigned char>> &imgArr, vector<vector<unsigned char>> &copyArr, int H, int W)
{
    for (int i = 1; i < H - 1; i++)
    {
        for (int j = 1; j < W - 1; j++)
        {
            if (imgArr[j][i] == 255)
            {
                if (Check(imgArr, j, i))
                    copyArr[j][i] = 0;
            }
            else
            {
                copyArr[j][i] = 255;
            }
        }
    }
}

void Dilition(vector<vector<unsigned char>> &imgArr, vector<vector<unsigned char>> &copyArr, int H, int W)
{
    for (int i = 1; i < H - 1; i++)
    {
        for (int j = 1; j < W - 1; j++)
        {
            if (imgArr[j][i] == 0)
            {
                if (imgArr[j - 1][i + 1] == 255 || imgArr[j - 1][i] == 255 || imgArr[j][i - 1] == 255 || imgArr[j - 1][i - 1] == 255 ||
                    imgArr[j + 1][i] == 255 || imgArr[j][i + 1] == 255 || imgArr[j + 1][i + 1] == 255 || imgArr[j + 1][i - 1] == 255)
                {
                    copyArr[j][i] = 255;
                }
                else
                {
                    copyArr[j][i] = 0;
                }
            }
        }
    }
}

int main()
{
    // origin image
    vector<vector<unsigned char>> imgArr;
    read_img("res\\circles_bi_1024.bmp", imgArr);
    int img_h = imgArr.size();
    int img_w = imgArr[0].size();

    vector<vector<unsigned char>> copyArr;
    copyArr.resize(img_h, vector<unsigned char>(img_w));

    int sample_h = copyArr.size();
    int sample_w = copyArr[0].size();

    Morphology(imgArr, copyArr, sample_h, sample_w);

    FILE *fp_w = fopen("res\\dilition.bmp", "wb");

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
            putc(copyArr[j][i], fp_w);
        }
    }

    fclose(fp_w);
    return 0;
}
