#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#define PI 3.1415926

using namespace std;
void read_img(const char *fileName, vector<vector<unsigned char>> &imgArr, int &width, int &heigh)
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

void save_image(const string &path, const vector<vector<unsigned char>> &image, int width, int height)
{
    ofstream file(path, ios::binary);
    if (!file)
    {
        cout << "error file save";
        return;
    }
    vector<unsigned char> header = createHeader(width, height);
    file.write(reinterpret_cast<char *>(header.data()), header.size());

    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = 0; j < width; j++)
        {
            file.put(image[i][j]);
        }
    }

    file.close();
}

void Filter(vector<vector<double>>& thinArr)
{
    int upper_bound = 100;
    int lower_bound = 30;
    for (int i = 0; i < thinArr.size(); i++)
    {
        for (int j = 0; j < thinArr[0].size(); j++)
        {
            if (int(thinArr[j][i]) >= upper_bound){
                thinArr[j][i] = 255;
            }
            else if (int(thinArr[j][i]) <= lower_bound){
                thinArr[j][i] = 0;
            }
            else {
                thinArr[j][i] = 100;
            }
        }
    }
}
vector<vector<unsigned char>> convertToUnsignedChar(const vector<vector<double>> &thinArr)
{
    int height = thinArr.size();
    int width = thinArr[0].size();
    vector<vector<unsigned char>> image(height, vector<unsigned char>(width));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = static_cast<unsigned char>(thinArr[i][j]);
        }
    }

    return image;
}


int main()
{
    // read img
    string inputPath = "AIgened_gray.bmp";
    int img_w, img_h;
    vector<vector<unsigned char>> imgArr;
    read_img(inputPath.c_str(), imgArr, img_w, img_h);
    cout << "image readed." << endl;

    vector<vector<unsigned char>> paddingArr;
    paddingArr.resize(img_w + 4, vector<unsigned char>(img_h + 4));

    vector<vector<unsigned char>> outArr;
    outArr.resize(img_w + 4, vector<unsigned char>(img_h + 4));

    for (int i = 0; i < img_h; i++)
    {
        for (int j = 0; j < img_w; j++)
        {
            paddingArr[j + 2][i + 2] = imgArr[j][i];
        }
    }

    // put img on a padding map
    vector<vector<unsigned char>> tmpArr;
    tmpArr.resize(img_w, vector<unsigned char>(img_h));
    for (int i = 2; i < img_h + 2; i++)
    {
        for (int j = 2; j < img_w + 2; j++)
        {
            double tmp = 2 * paddingArr[j - 2][i - 2] + 4 * paddingArr[j - 1][i - 2] + 5 * paddingArr[j][i - 2] + 4 * paddingArr[j + 1][i - 2] + 2 * paddingArr[j + 2][i - 2] + 4 * paddingArr[j - 2][i - 1] + 9 * paddingArr[j - 1][i - 1] + 12 * paddingArr[j][i - 1] + 9 * paddingArr[j + 1][i - 1] + 4 * paddingArr[j + 2][i - 1] + 5 * paddingArr[j - 2][i] + 12 * paddingArr[j - 1][i] + 15 * paddingArr[j][i] + 12 * paddingArr[j + 1][i] + 5 * paddingArr[j + 2][i] + 4 * paddingArr[j - 2][i + 1] + 9 * paddingArr[j - 1][i + 1] + 12 * paddingArr[j][i + 1] + 9 * paddingArr[j + 1][i + 1] + 4 * paddingArr[j + 2][i + 1] + 2 * paddingArr[j - 2][i + 2] + 4 * paddingArr[j - 1][i + 2] + 5 * paddingArr[j][i + 2] + 4 * paddingArr[j + 1][i + 2] + 2 * paddingArr[j + 2][i + 2];
            tmpArr[j - 2][i - 2] = Truncate(tmp / 159 + 0.5);
        }
    }

    vector<vector<unsigned char>> degreeArr;
    degreeArr.resize(img_w, vector<unsigned char>(img_h));
    vector<vector<unsigned char>> energyArr;
    energyArr.resize(img_w, vector<unsigned char>(img_h));
    for (int i = 1; i < img_h - 1; i++)
    {
        for (int j = 1; j < img_w - 1; j++)
        {
            double sobel_h = -1 * tmpArr[j - 1][i - 1] - 2 * tmpArr[j - 1][i] - tmpArr[j - 1][i + 1] + tmpArr[j + 1][i - 1] + 2 * tmpArr[j + 1][i] + tmpArr[j + 1][i + 1];
            double sobel_v = -1 * tmpArr[j - 1][i - 1] - 2 * tmpArr[j][i - 1] - tmpArr[j + 1][i - 1] + tmpArr[j - 1][i + 1] + 2 * tmpArr[j][i + 1] + tmpArr[j + 1][i + 1];
            double eng_edge = sqrt(sobel_v * sobel_v + sobel_h * sobel_h);
            energyArr[j][i] = eng_edge;

            if (sobel_v != 0 || sobel_h != 0)
            {
                double degree = atan2(sobel_v, sobel_h) * 180.0 / PI;
                if (degree < 0)
                {
                    degree += 180;
                }
                else if (degree >= 180)
                {
                    degree -= 180;
                }
                degreeArr[j][i] = (int)(degree / 45 + 0.5) * 45;
            }
            else
            {
                degreeArr[j][i] = 255;
            }

            if (energyArr[j][i] > 128)
            {
                outArr[j][i] = degreeArr[j][i];
            }
        }
    }

    vector<vector<double>> thinArr;
    thinArr.resize(img_w, vector<double>(img_h));
    for (int i = 1; i < img_h - 1; i++)
    {
        for (int j = 1; j < img_w - 1; j++)
        {
            thinArr[j][i] = 0;
            if (degreeArr[j][i] == 0 || degreeArr[j][i] == 180)
            {
                if (energyArr[j][i] >= energyArr[j - 1][i] && energyArr[j][i] >= energyArr[j + 1][i])
                {
                    thinArr[j][i] = energyArr[j][i];
                }
            }
            else if (degreeArr[j][i] == 45)
            {
                if (energyArr[j][i] >= energyArr[j - 1][i - 1] && energyArr[j][i] >= energyArr[j + 1][i + 1])
                {
                    thinArr[j][i] = energyArr[j][i];
                }
            }
            else if (degreeArr[j][i] == 90)
            {
                if (energyArr[j][i] >= energyArr[j][i - 1] && energyArr[j][i] >= energyArr[j][i + 1])
                {
                    thinArr[j][i] = energyArr[j][i];
                }
            }
            else if (degreeArr[j][i] == 135)
            {
                if (energyArr[j][i] >= energyArr[j - 1][i + 1] && energyArr[j][i] >= energyArr[j + 1][i - 1])
                {
                    thinArr[j][i] = energyArr[j][i];
                }
            }
        }
    }
    Filter(thinArr);
    vector<vector<unsigned char>> image = convertToUnsignedChar(thinArr);
    int w = image[0].size();
    int h = image.size();
    save_image("thinArr.bmp", image, w, h);


    return 0;
}