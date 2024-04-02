#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

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

template<typename T>
unsigned char Truncate(T pix){
    unsigned char ans = 0;
    ans = ((pix<0)?0:(pix>255)?255:pix);
    return ans;
}

void DownSampling(vector<vector<unsigned char>> &imgArr, vector<vector<unsigned char>> &downScale, int img_h, int img_w){
    for (int i=0;i<img_h;i++){
        for (int j=0;j<img_w;j++){
            if (i%2 == 0 && j%2 == 0){
                downScale[j/2][i/2] = imgArr[j][i];
            }
        }
    }
}
void UpSampling(vector<vector<unsigned char>> &downScale, vector<vector<unsigned char>> &upScale, int img_h, int img_w){
    for (int i=0;i<img_h-1;i++){
        for (int j=0;j<img_w-1;j++){
            int x = j/2, y=i/2;
            // case 1 四點填充
            if(i%2==0 && j%2==0){
                upScale[j][i] = downScale[x][y];
            }

            // case 2 垂直相加
            else if(i%2==1 && j%2==0){
                float u = downScale[x][y];
                float d = downScale[x][y+1];
                upScale[j][i] = Truncate(u*0.5 + d*0.5 + 0.5);
            }

            // case 3 水平相加
            else if(i%2==0 && j%2==1){
                float l = downScale[x][y];
                float r = downScale[x+1][y];
                upScale[j][i] = Truncate(l*0.5 + r*0.5 + 0.5);
            }

            // case 4 四點中點
            else{
                float lu = downScale[x][y];
                float ru = downScale[x+1][y];
                float ld = downScale[x][y+1];
                float rd = downScale[x+1][y+1];
                upScale[j][i] = Truncate(lu*0.5 + ru*0.5 + ld*0.5 + rd*0.5 +0.5);
            }
        }
    }
}

int main()
{
    // origin image
    vector<vector<unsigned char>> imgArr;
    read_img("res\\AIgened_gray.bmp", imgArr);
    int img_h = imgArr.size();
    int img_w = imgArr[0].size();

    // down scale image
    vector<vector<unsigned char>> downScale;
    downScale.resize(img_h/2, vector<unsigned char>(img_w/2));

    // up scale image
    vector<vector<unsigned char>> upScale;
    upScale.resize(img_h, vector<unsigned char>(img_w));

    // process
    DownSampling(imgArr,downScale,img_h,img_w);
    UpSampling(downScale,upScale,img_h,img_w);
    
    int sample_h = upScale.size();
    int sample_w = upScale[0].size();

    // sample_h = downScale.size();
    // sample_w = downScale[0].size();

    FILE *fp_w = fopen("res\\down_simpling.bmp", "wb");

    if (fp_w == NULL)
        return 0;

    vector<unsigned char> my_Header = createHeader(sample_w, sample_h);
    for (int i = 0; i < 1078; i++)
    {
        putc(my_Header[i], fp_w);
    }

    for (int i = sample_h - 1; i >= 0; i--)
    {
        for (int j = 0; j < sample_w; j++)
        {
            putc(upScale[j][i], fp_w);
        }
    }

    fclose(fp_w);
    return 0;
}
