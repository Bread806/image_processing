#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

template <typename T>
unsigned char Truncate(T pix)
{
    unsigned char ans = 0;
    ans = ((pix < 0) ? 0 : (pix > 255) ? 255
                                       : pix);
    return ans;
}

void Gamma(vector<vector<unsigned char>>& imgArr, double gamma) {
    vector<unsigned char> lut(256);
    for (int i = 0; i < 256; ++i) {
        lut[i] = Truncate(pow((i / 255.0), gamma) * 255.0);
    }

    for (auto &row : imgArr) {
        for (auto &pix : row) {
            pix = lut[pix];
        }
    }
}


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

// hough
void houghTransform(const vector<vector<unsigned char>>& imgArr, vector<vector<int>>& accumulator, int max_dist) {
    int img_w = imgArr[0].size();
    int img_h = imgArr.size();

    for (int i = 0; i < img_h; i++) {
        for (int j = 0; j < img_w; j++) {
            if (imgArr[j][i] == 255) {
                for (int theta = 0; theta < 180; theta++) {
                    double piTheta = theta * M_PI / 180.0;
                    int ro = round(j * cos(piTheta) + i * sin(piTheta)) + max_dist;
                    accumulator[theta][ro]++;
                }
            }
        }
    }
}

// to pic
void writeOutput(const vector<vector<unsigned char>>& img, const char* fileName) {
    int height = img.size();
    int width = img[0].size();

    vector<unsigned char> header = createHeader(width, height);
    ofstream outFile(fileName, ios::binary);
    if (!outFile) {
        cerr << "Error opening output file." << endl;
        return;
    }

    
    outFile.write((char*)&header[0], header.size());

    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            outFile.put(img[i][j]);
        }
    }
    outFile.close();
}

int main() {
    const char* inputFileName = "lines.bmp";
    const char* outputFileName = "hough_output.bmp";

    vector<vector<unsigned char>> imgArr;
    read_img(inputFileName, imgArr);

    int img_w = imgArr[0].size();
    int img_h = imgArr.size();
    int max_dist = round(sqrt(img_w * img_w + img_h * img_h));

    vector<vector<int>> accumulator(180, vector<int>(2 * max_dist + 1, 0));
    houghTransform(imgArr, accumulator, max_dist);

    vector<vector<unsigned char>> outputImg(180, vector<unsigned char>(2 * max_dist + 1, 0));
    for (int i = 0; i < 180; ++i) {
        for (int j = 0; j < 2 * max_dist + 1; ++j) {
            outputImg[i][j] = static_cast<unsigned char>(255 * min(1.0, accumulator[i][j] / 100.0)); // [0, 255]
        }
    }

    Gamma(outputImg, 0.5);

    writeOutput(outputImg, outputFileName);

    return 0;
}
