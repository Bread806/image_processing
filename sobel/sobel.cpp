#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

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


void read_img(const char *fileName, vector<vector<unsigned char>> &imgArr);
vector<unsigned char> createHeader(int width, int height);

void sobel(const vector<vector<unsigned char>>& image, vector<vector<unsigned char>>& edges, int width, int height) {
    vector<int> sobel_x = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    vector<int> sobel_y = {1, 2, 1, 0, 0, 0, -1, -2, -1}; // 注意方向正確性

    edges.resize(height, vector<unsigned char>(width, 0));

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0;
            int gy = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int pixel = image[y + dy][x + dx];
                    gx += pixel * sobel_x[(dy + 1) * 3 + (dx + 1)];
                    gy += pixel * sobel_y[(dy + 1) * 3 + (dx + 1)];
                }
            }

            int magnitude = sqrt(gx * gx + gy * gy);
            magnitude = magnitude > 255 ? 255 : magnitude;
            edges[y][x] = static_cast<unsigned char>(magnitude);
        }
    }
}

int main() {
    const char* fileName = "AIgened_gray.bmp";
    vector<vector<unsigned char>> image, edges;
    read_img(fileName, image);

    int width = image[0].size();
    int height = image.size();

    sobel(image, edges, width, height);

    vector<unsigned char> header = createHeader(width, height);

    // Write to new BMP file
    ofstream outputFile("output_sobel.bmp", ios::binary);
    outputFile.write(reinterpret_cast<const char*>(header.data()), header.size());

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            outputFile.put(edges[y][x]);
        }
    }

    outputFile.close();

    cout << "Sobel done" << endl;

    return 0;
}
