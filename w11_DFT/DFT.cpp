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

float Max(float a, float b)
{
    if (a >= b)
        return a;
    else
        return b;
}

float Min(float a, float b)
{
    if (a <= b)
        return a;
    else
        return b;
}

void WriteFIle(vector<vector<float>> &arr, string fileName)
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

void InitInputData(vector<vector<unsigned char>> &imgArr, vector<vector<int>> &inputData)
{
    for (int i = 0; i < imgArr.size(); i++)
    {
        for (int j = 0; j < imgArr[0].size(); j++)
        {
            inputData[j][i] = imgArr[j][i] - '0';
        }
    }
}

void DFT(vector<vector<int>> &inputData, int N, vector<vector<float>> &realOut, vector<vector<float>> &imagOut, vector<vector<float>> &amplitudeOut)
{
    // u,v 0 ~ N
    for (int u = 0; u < N; u++)
    {
        for (int v = 0; v < N; v++)
        {
            for (int x = 0; x < N; x++)
            {
                for (int y = 0; y < N; y++)
                {
                    float sita = u * x - v * y;
                    realOut[u][v] += cos((2 * PI / N) * sita) * float(inputData[x][y]);
                    imagOut[u][v] -= sin((2 * PI / N) * sita) * float(inputData[x][y]);
                    // cout << "imagOut[u][v] : " << imagOut[u][v] <<endl;
                }
            }
            realOut[u][v] /= (N * N);
            imagOut[u][v] /= (N * N);
            amplitudeOut[u][v] = sqrt(realOut[u][v] * realOut[u][v] + imagOut[u][v] * imagOut[u][v]);

            // min max cal
            max_amp = Max(float(max_amp), amplitudeOut[u][v]);
            min_amp = Min(float(min_amp), amplitudeOut[u][v]);
        }
    }
}

void Normallize(vector<vector<float>> &amplitudeOut)
{
    int Length = amplitudeOut.size();
    for (int i = 0; i < Length; i++)
    {
        for (int j = 0; j < Length; j++)
        {
            amplitudeOut[i][j] = (amplitudeOut[i][j] - min_amp) / (max_amp - min_amp);
        }
    }
}

void GammaEnhencement(vector<vector<float>> &amplitudeOut)
{
    int Length = amplitudeOut.size();
    for (int i = 0; i < Length; i++)
    {
        for (int j = 0; j < Length; j++)
        {
            float tmp = pow(amplitudeOut[j][i] / 255.0f, 0.3f) * 255 + 0.5;
            amplitudeOut[j][i] = (tmp < 0) ? 0 : (tmp > 255) ? 255 : tmp;
            amplitudeOut[j][i] *= 255;
        }
    }
}
int main()
{
    // origin image
    vector<vector<unsigned char>> imgArr;
    read_img("res\\logo_bi.bmp", imgArr);

    int img_h = imgArr.size();
    int img_w = imgArr[0].size();

    // vector<vector<int>> inputData;
    vector<vector<int>> inputData(img_h, vector<int>(img_w, 0));

    InitInputData(imgArr, inputData);

    cout << "input data checked" << endl;

    vector<vector<float>> realOut(img_h, vector<float>(img_w, 0.0));
    vector<vector<float>> imagOut(img_h, vector<float>(img_w, 0.0));
    vector<vector<float>> amplitudeOut(img_h, vector<float>(img_w, 0.0));

    // prepare DFT real part and DFT imag part
    realOut.resize(img_h, vector<float>(img_w));
    imagOut.resize(img_h, vector<float>(img_w));

    int N = img_h;

    DFT(inputData, N, realOut, imagOut, amplitudeOut);
    Normallize(amplitudeOut);
    GammaEnhencement(amplitudeOut);

    cout << "====DFT====" << endl;
    WriteFIle(realOut, "realOut.txt");
    WriteFIle(imagOut, "imagOut.txt");

    FILE *fp_w = fopen("res\\DFT_gen.bmp", "wb");

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
            putc(amplitudeOut[j][i], fp_w);
        }
    }

    fclose(fp_w);
    return 0;
}
