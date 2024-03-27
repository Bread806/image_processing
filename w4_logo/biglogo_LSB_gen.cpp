#include <iostream>
#include <vector>

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
int main()
{
    vector<vector<unsigned char>> mrkMapArr;
    vector<vector<unsigned char>> imgArr;
    vector<vector<unsigned char>> markArr;
    vector<vector<unsigned char>> newImgArr;

    read_img("res\\logo_bi.bmp", markArr);
    read_img("res\\AIgened_gray.bmp", imgArr);

    int mrk_h = markArr.size();
    int mrk_w = markArr[0].size();

    int img_h = imgArr.size();
    int img_w = imgArr[0].size();
    
    int mrk_map_h = mrk_h * 5;
    int mrk_map_w = mrk_w * 5;

    mrkMapArr.resize(mrk_map_h, vector<unsigned char>(mrk_map_w));
    newImgArr.resize(img_h, vector<unsigned char>(img_w));

    bool lock = false;
    // gen a logo map
    for (int i = 0; i < mrk_map_h; i++)
    {
        for (int j = 0; j < mrk_map_w; j++)
        {
            if ((j / mrk_w + i / mrk_h) % 2)
                lock = !lock;

            if (lock)
            {
                // float tmp = (float)markArr[t][k];
                mrkMapArr[j][i] = markArr[j % 256][i % 256];
            }
            else
            {
                mrkMapArr[j][i] = 255;
            }
        }
    }

    // img print to map (LSB)
    int pointX = 24, pointY = 24;
    for (int i = 0; i < img_h; i++)
    {
        for (int j = 0; j < img_w; j++)
        {
            if (mrkMapArr[j+pointX][i+pointY]>250)
                mrkMapArr[j+pointX][i+pointY] = (imgArr[j][i] % 2 == 1) ? imgArr[j][i] : imgArr[j][i] + 1;
            else
                mrkMapArr[j+pointX][i+pointY] = (imgArr[j][i] % 2 == 0) ? imgArr[j][i] : imgArr[j][i] - 1;
        }
    }

    for (int i = 0; i < img_h; i++)
    {
        for (int j = 0; j < img_w; j++)
        {
            newImgArr[j][i] = mrkMapArr[j+pointX][i+pointY];
        }
    }  

    FILE *fp_w = fopen("res\\cover_LSB.bmp", "wb");

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
            putc(newImgArr[j][i], fp_w);
        }
    }

    fclose(fp_w);

    return 0;
}