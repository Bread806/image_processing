#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

// 霍夫轉換函數
void houghTransform(vector<vector<unsigned char>>& imgArr, vector<vector<int>>& accumulator, int max_dist) {
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

// 輸出結果到 BMP
void writeImage(const vector<vector<unsigned char>>& imgArr, const char* filename) {
    int img_w = imgArr[0].size();
    int img_h = imgArr.size();
    vector<unsigned char> header = createHeader(img_w, img_h);

    ofstream out(filename, ios::binary);
    out.write((char*)&header[0], header.size());

    for (int i = img_h - 1; i >= 0; i--) {
        for (int j = 0; j < img_w; j++) {
            unsigned char pix = imgArr[j][i];
            out.write((char*)&pix, sizeof(pix));
        }
    }
    out.close();
}

int main() {
    const char* inputFileName = "path_to_your_image.bmp";
    const char* outputFileName = "hough_output.bmp";

    vector<vector<unsigned char>> imgArr;
    read_img(inputFileName, imgArr);

    int img_w = imgArr[0].size();
    int img_h = imgArr.size();
    int max_dist = round(sqrt(img_w * img_w + img_h * img_h));

    // 初始化累加器
    vector<vector<int>> accumulator(180, vector<int>(2 * max_dist + 1, 0));
    houghTransform(imgArr, accumulator, max_dist);

    // 標記直線
    int threshold = 100; // 根據需要調整閾值
    for (int theta = 0; theta < 180; theta++) {
        for (int ro = 0; ro < 2 * max_dist + 1; ro++) {
            if (accumulator[theta][ro] > threshold) {
                double piTheta = theta * M_PI / 180.0;
                for (int x = 0; x < img_w; x++) {
                    int y = round((ro - max_dist - x * cos(piTheta)) / sin(piTheta));
                    if (y >= 0 && y < img_h) {
                        imgArr[x][y] = 0; // 標記為黑色
                    }
                }
            }
        }
    }

    writeImage(imgArr, outputFileName);

    return 0;
}