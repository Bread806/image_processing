#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

void read_img(const char *fileName, vector<vector<unsigned char>> &imgArr, int &width, int &height)
{
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL)
    {
        cout << "Error opening file" << endl;
        return;
    }

    fseek(fp, 18, SEEK_SET);
    fread(&width, 4, 1, fp);
    fread(&height, 4, 1, fp);

    fseek(fp, 54, SEEK_SET);

    imgArr.resize(height, vector<unsigned char>(width));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            imgArr[i][j] = getc(fp);
        }
    }

    fclose(fp);
}

vector<unsigned char> createHeader(int width, int height)
{
    int headSize = 1078;
    int fileSize = headSize + width * height;
    vector<unsigned char> header(headSize, 0);

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

void save_image(const string &path, const vector<vector<unsigned char>> &image, int width, int height)
{
    ofstream file(path, ios::binary);
    if (!file)
    {
        cout << "Error saving file" << endl;
        return;
    }

    vector<unsigned char> header = createHeader(width, height);
    file.write(reinterpret_cast<char *>(header.data()), header.size());

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            file.put(image[i][j]);
        }
    }

    file.close();
}

vector<vector<int>> applyKernel(const vector<vector<unsigned char>> &image, const vector<vector<int>> &kernel)
{
    int height = image.size();
    int width = image[0].size();
    int K_size = kernel.size();
    int K_half = K_size / 2;

    vector<vector<int>> result(height, vector<int>(width, 0));

    for (int i = K_half; i < height - K_half; i++)
    {
        for (int j = K_half; j < width - K_half; j++)
        {
            int sum = 0;
            for (int x = -K_half; x <= K_half; x++)
            {
                for (int y = -K_half; y <= K_half; y++)
                {
                    sum += image[i + x][j + y] * kernel[x + K_half][y + K_half];
                }
            }
            result[i][j] = sum;
        }
    }

    return result;
}

vector<vector<unsigned char>> Sobel(const vector<vector<unsigned char>> &grayImg)
{
    int height = grayImg.size();
    int width = grayImg[0].size();

    vector<vector<int>> Gx = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};

    vector<vector<int>> Gy = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    vector<vector<int>> gradX = applyKernel(grayImg, Gx);
    vector<vector<int>> gradY = applyKernel(grayImg, Gy);

    vector<vector<unsigned char>> edgeImage(height, vector<unsigned char>(width, 0));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int magnitude = sqrt(gradX[i][j] * gradX[i][j] + gradY[i][j] * gradY[i][j]);
            edgeImage[i][j] = static_cast<unsigned char>(min(255, magnitude));
        }
    }

    return edgeImage;
}

int main()
{
    string inputPath = "AIgened_gray.bmp";
    string outputSobel = "res\\save.bmp";

    int width, height;
    vector<vector<unsigned char>> grayImg;
    read_img(inputPath.c_str(), grayImg, width, height);
    cout << "Image read." << endl;

    vector<vector<unsigned char>> sobelEdgeImg = Sobel(grayImg);
    cout << "Sobel done." << endl;

    save_image(outputSobel, sobelEdgeImg, width, height);
    cout << "Sobel edge detection completed. Result saved as " << outputSobel << endl;

    return 0;
}
