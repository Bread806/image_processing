#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <queue>

using namespace std;

void read_img(const char *fileName, vector<vector<unsigned char>> &imgArr, int &width, int &height)
{
    FILE *fp = fopen(fileName, "rb");
    if (!fp)
        return;

    unsigned char c;
    for (int i = 0; i < 18; i++)
        getc(fp);

    width = 0;
    for (int i = 0; i < 4; i++)
        width += getc(fp) << (8 * i);

    height = 0;
    for (int i = 0; i < 4; i++)
        height += getc(fp) << (8 * i);

    for (int i = 0; i < 28; i++)
        getc(fp);

    imgArr.resize(height, vector<unsigned char>(width));
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            imgArr[i][j] = getc(fp);

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

void save_image(const string &path, const vector<vector<unsigned char>> &image, int width, int height)
{
    ofstream file(path, ios::binary);
    if (!file)
        return;

    vector<unsigned char> header = createHeader(width, height);
    file.write(reinterpret_cast<char *>(header.data()), header.size());

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            file.put(image[i][j]);

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
        for (int j = K_half; j < width - K_half; j++)
        {
            int sum = 0;
            for (int x = -K_half; x <= K_half; x++)
                for (int y = -K_half; y <= K_half; y++)
                    sum += image[i + x][j + y] * kernel[x + K_half][y + K_half];
            result[i][j] = sum;
        }
    return result;
}

vector<vector<unsigned char>> gaussianBlur(const vector<vector<unsigned char>> &image)
{
    vector<vector<int>> kernel = {
        {1, 4, 7, 4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1, 4, 7, 4, 1}};
    int kernelSum = 273;

    int height = image.size();
    int width = image[0].size();
    int K_size = kernel.size();
    int K_half = K_size / 2;

    vector<vector<unsigned char>> result(height, vector<unsigned char>(width, 0));
    for (int i = K_half; i < height - K_half; i++)
        for (int j = K_half; j < width - K_half; j++)
        {
            int sum = 0;
            for (int x = -K_half; x <= K_half; x++)
                for (int y = -K_half; y <= K_half; y++)
                    sum += image[i + x][j + y] * kernel[x + K_half][y + K_half];
            result[i][j] = sum / kernelSum;
        }
    return result;
}

vector<vector<double>> nonMaxSuppression(const vector<vector<int>> &gradient, const vector<vector<double>> &angles)
{
    int height = gradient.size();
    int width = gradient[0].size();
    vector<vector<double>> result(height, vector<double>(width, 0));

    for (int i = 1; i < height - 1; i++)
        for (int j = 1; j < width - 1; j++)
        {
            double angle = angles[i][j];
            int q = 255, r = 255;

            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180))
            {
                q = gradient[i][j + 1];
                r = gradient[i][j - 1];
            }
            else if (angle >= 22.5 && angle < 67.5)
            {
                q = gradient[i + 1][j - 1];
                r = gradient[i - 1][j + 1];
            }
            else if (angle >= 67.5 && angle < 112.5)
            {
                q = gradient[i + 1][j];
                r = gradient[i - 1][j];
            }
            else if (angle >= 112.5 && angle < 157.5)
            {
                q = gradient[i - 1][j - 1];
                r = gradient[i + 1][j + 1];
            }

            if (gradient[i][j] >= q && gradient[i][j] >= r)
                result[i][j] = gradient[i][j];
        }
    return result;
}

void doubleThresholdAndEdgeTracking(vector<vector<unsigned char>> &edges, double lowThresh, double highThresh)
{
    int height = edges.size();
    int width = edges[0].size();
    vector<vector<bool>> strongEdge(height, vector<bool>(width, false));

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            if (edges[i][j] >= highThresh)
            {
                edges[i][j] = 255;
                strongEdge[i][j] = true;
            }
            else if (edges[i][j] < lowThresh)
                edges[i][j] = 0;
            else
                edges[i][j] = 128;
        }

    queue<pair<int, int>> q;
    for (int i = 1; i < height - 1; i++)
        for (int j = 1; j < width - 1; j++)
            if (strongEdge[i][j])
                q.push({i, j});

    while (!q.empty())
    {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++)
                if (edges[x + i][y + j] == 128)
                {
                    edges[x + i][y + j] = 255;
                    q.push({x + i, y + j});
                }
    }

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (edges[i][j] != 255)
                edges[i][j] = 0;
}

vector<vector<unsigned char>> Canny(const vector<vector<unsigned char>> &grayImg, double lowThresh, double highThresh)
{
    int height = grayImg.size();
    int width = grayImg[0].size();

    vector<vector<unsigned char>> blurredImg = gaussianBlur(grayImg);

    vector<vector<int>> Gx = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};

    vector<vector<int>> Gy = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    vector<vector<int>> gradX = applyKernel(blurredImg, Gx);
    vector<vector<int>> gradY = applyKernel(blurredImg, Gy);

    vector<vector<int>> gradient(height, vector<int>(width, 0));
    vector<vector<double>> angles(height, vector<double>(width, 0));
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            gradient[i][j] = sqrt(gradX[i][j] * gradX[i][j] + gradY[i][j] * gradY[i][j]);
            angles[i][j] = atan2(gradY[i][j], gradX[i][j]) * 180 / M_PI;
            if (angles[i][j] < 0)
                angles[i][j] += 180;
        }

    vector<vector<double>> nonMaxImg = nonMaxSuppression(gradient, angles);

    vector<vector<unsigned char>> edges(height, vector<unsigned char>(width, 0));
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            edges[i][j] = static_cast<unsigned char>(nonMaxImg[i][j]);

    doubleThresholdAndEdgeTracking(edges, lowThresh, highThresh);

    return edges;
}

int main()
{
    string inputPath = "ninja.bmp";
    string outputCanny = "res\\ninja_canny_30-100_.bmp";

    int width, height;
    vector<vector<unsigned char>> grayImg;
    read_img(inputPath.c_str(), grayImg, width, height);
    cout << "image readed." << endl;

    // vector<vector<unsigned char>> sobelEdgeImg = Sobel(grayImg);
    // cout << "Sobel done." << endl;

    // save_image(outputSobel, sobelEdgeImg, width, height);
    // cout << "Sobel edge detection completed. Result saved as " << outputSobel << endl;

    vector<vector<unsigned char>> cannyEdgeImg = Canny(grayImg, 30, 100);
    cout << "Canny done." << endl;

    save_image(outputCanny, cannyEdgeImg, width, height);
    cout << "Canny edge detection completed. Result saved as " << outputCanny << endl;

    return 0;
}
