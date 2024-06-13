#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <algorithm>
#define PI 3.1415926
int min_amp = 0;
int max_amp = 65536;

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

void WriteFIle(vector<vector<double>> &arr, string fileName)
{
    int row = 32, col = 32;
    ofstream outFile(fileName);
    if (!outFile.is_open())
    {
        cerr << "error " << fileName << " file writing" << endl;
        return;
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            outFile << arr[i][j] << "\t";
        }
        outFile << endl;
    }

    outFile.close();
}

void DCT(vector<vector<double>> block, vector<vector<double>> dct)
{
    for (int u = 0; u < 8; u++)
    {
        for (int v = 0; v < 8; v++)
        {
            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    dct[u][v] += block[x][y] * cos(u * (x + 0.5) * PI / 8.0) * cos(v * (y + 0.5) * PI / 8.0);
                }
            }

            if (u == 0 && v == 0)
            {
                dct[u][v] *= 0.125;
            }
            else if (u == 0 || v == 0)
            {
                dct[u][v] *= sqrt(0.25) * sqrt(0.125);
            }
            else
            {
                dct[u][v] *= 0.25;
            }
        }
    }
}

vector<vector<double>> Spilt8(vector<vector<int>> &inputData)
{
    int h = inputData.size();
    int w = inputData[0].size();
    vector<vector<double>> outputData;
    outputData.resize(w, vector<double>(h));
    for (int i = 0; i < h; i += 8)
    {
        for (int j = 0; j < w; j += 8)
        {
            vector<vector<double>> block;
            block.resize(8, vector<double>(8));

            vector<vector<double>> dct;
            dct.resize(8, vector<double>(8));

            for (int t = 0; t < 8; t++)
            {
                for (int k = 0; k < 8; k++)
                {
                    block[t][k] = inputData[j + t][i + k];
                }
            }
            DCT(block, dct);
            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    outputData[j+x][i+y] = dct[x][y];
                }
            }
        }
    }
    return outputData;
}

template <typename T>
unsigned char Truncate(T pix) {
    return (pix < 0 ? 0 : (pix > 255 ? 255 : static_cast<unsigned char>(pix)));
}


int main() {
    vector<vector<unsigned char>> imgArr;
    read_img("Algened_gray.bmp", imgArr);

    int img_w = imgArr.size();
    int img_h = imgArr[0].size();

    vector<vector<double>> LL, LH, HL, HH;
    LL.resize(img_w / 2, vector<double>(img_h / 2));
    LH.resize(img_w / 2, vector<double>(img_h / 2));
    HL.resize(img_w / 2, vector<double>(img_h / 2));
    HH.resize(img_w / 2, vector<double>(img_h / 2));

    vector<vector<unsigned char>> outArr(img_w, vector<unsigned char>(img_h));
    for (int i = 0; i < img_h / 2; i++) {
        for (int j = 0; j < img_w / 2; j++) {
            double a = (LL[i][j] + HL[i][j] + LH[i][j] + HH[i][j]) / 2;
            double b = (LL[i][j] - LH[i][j] + HL[i][j] - HH[i][j]) / 2;
            double c = (LL[i][j] - HL[i][j] + LH[i][j] - HH[i][j]) / 2;
            double d = (LL[i][j] + HH[i][j] - HL[i][j] - LH[i][j]) / 2;

            outArr[2 * i][2 * j] = Truncate(a);
            outArr[2 * i][2 * j + 1] = Truncate(b);
            outArr[2 * i + 1][2 * j] = Truncate(c);
            outArr[2 * i + 1][2 * j + 1] = Truncate(d);
        }
    }

    FILE *fp_w = fopen("res\\gen.bmp", "wb");
    if (fp_w == NULL) return 0;

    vector<unsigned char> my_Header = createHeader(img_w, img_h);
    for (int i = 0; i < 1078; i++) {
        putc(my_Header[i], fp_w);
    }

    for (int i = img_h - 1; i >= 0; i--) {
        for (int j = 0; j < img_w; j++) {
            putc(outArr[j][i], fp_w);
        }
    }

    fclose(fp_w);

    return 0;
}