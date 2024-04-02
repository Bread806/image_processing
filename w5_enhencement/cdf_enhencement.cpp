#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
using namespace std;
int cdf_table[255] = {0};
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
float M(float r1, float s1, float r2, float s2)
{
    return (s2 - s1) / (r2 - r1);
}

float First(int x)
{
    return x / 2;
}
float Second(int x)
{
    // x=100 y=50
    return 3.1 * (x - 100) + 50;
}
float Third(int x)
{
    // x=150 y=205
    return 0.45 * (x - 150) + 205;
}

float H(float f)
{
    int v = f;
    return round(float(float(cdf_table[v] - cdf_table[0]) / float(cdf_table[254] - cdf_table[0])) * 255);
}

int main()
{
    vector<vector<unsigned char>> imgArr;
    read_img("res\\AIgened_gray.bmp", imgArr);

    int img_h = imgArr.size();
    int img_w = imgArr[0].size();

    for (int i = 0; i < img_h; i++)
    {
        for (int j = 0; j < img_w; j++)
        {
            int gary = imgArr[j][i];
            cdf_table[gary]++;
        }
    }

    // prefix sum
    for (int i = 1; i < 255; i++)
    {
        cdf_table[i] = cdf_table[i - 1] + cdf_table[i];
    }

    for (int i = 0; i < img_h; i++)
    {
        for (int j = 0; j < img_w; j++)
        {
            imgArr[j][i] = H(int(imgArr[j][i]));
        }
    }

    // txt wb 
    ofstream ofs;ofs.open("output.txt");
    if (!ofs.is_open()) {
        cout << "Failed to open file.\n";
        return 1; // EXIT_FAILURE
    }
    for (int i = 0; i < img_h; i++)
    {
        for (int j = 0; j < img_w; j++)
        {
            ofs<<float(imgArr[j][i])<<" ";
        }
        ofs <<"\n";
    }

    ofs << "This is img file output\n";
    ofs.close();

    FILE *fp_w = fopen("res\\enhencement_cdf_img.bmp", "wb");

    if (fp_w == NULL)
        return 0;

    vector<unsigned char> my_Header = createHeader(img_w, img_h);
    for (int i = 0; i < 1078; i++)
    {
        putc(my_Header[i], fp_w);
    }

    for (int i = img_h - 1; i >= 0; i--)
    {
        for (int j = 0; j < img_w; j++)
        {
            putc(imgArr[j][i], fp_w);
        }
    }

    fclose(fp_w);
    return 0;
}
