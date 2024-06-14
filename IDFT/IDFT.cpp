#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>

using namespace std;

struct Complex {
    double real;
    double imag;
};

void IDFT(const vector<vector<Complex>>& input, vector<vector<double>>& output) {
    const int M = input.size();
    const int N = input[0].size();
    const double pi = acos(-1);

    for (int x = 0; x < M; ++x) {
        for (int y = 0; y < N; ++y) {
            double sumReal = 0.0;
            for (int u = 0; u < M; ++u) {
                for (int v = 0; v < N; ++v) {
                    double angle = 2 * pi * ((u * x / double(M)) + (v * y / double(N)));
                    sumReal += input[u][v].real * cos(angle) - input[u][v].imag * sin(angle);
                }
            }
            //+128
            output[x][y] = sumReal / (M * N) + 128;
        }
    }
}

void read_img(const char *fileName, vector<vector<unsigned char>> &imgArr) {
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL)
        return;

    unsigned char c;
    for (int i = 0; i < 10; i++) {
        c = getc(fp);
    }

    int s = 0;
    s += getc(fp);
    s += getc(fp) * 256;
    s += getc(fp) * 256 * 256;
    s += getc(fp) * 256 * 256 * 256;

    for (int i = 0; i < 4; i++) {
        c = getc(fp);
    }

    int w = 0;
    w += getc(fp);
    w += getc(fp) * 256;
    w += getc(fp) * 256 * 256;
    w += getc(fp) * 256 * 256 * 256;

    int h = 0;
    h += getc(fp);
    h += getc(fp) * 256;
    h += getc(fp) * 256 * 256;
    h += getc(fp) * 256 * 256 * 256;

    for (int i = 0; i < 8; i++) {
        c = getc(fp);
    }

    int l = 0;
    l += getc(fp);
    l += getc(fp) * 256;
    l += getc(fp) * 256 * 256;
    l += getc(fp) * 256 * 256 * 256;

    for (int i = 0; i < s - 38; i++) {
        c = getc(fp);
    }

    imgArr.resize(h, vector<unsigned char>(w));
    for (int i = 0; i < w * h; i++) {
        int x = i % w;
        int y = i / w;
        imgArr[y][x] = getc(fp); 
    }

    fclose(fp);
}

int main() {
    const char* fileName = "mask.bmp";
    vector<vector<unsigned char>> imgArray;
    read_img(fileName, imgArray);

    int M = imgArray.size();
    int N = imgArray[0].size();

    vector<vector<Complex>> input(M, vector<Complex>(N, {0, 0}));
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            input[i][j].real = imgArray[i][j] - 128; 
        }
    }

    vector<vector<double>> output(M, vector<double>(N, 0));
    IDFT(input, output);

    // Output the result or further process
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cout << output[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}