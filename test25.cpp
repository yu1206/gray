
#include <fstream>
#include <iostream>
#include <string>
#include <sstream> 
#include "opencv\cv.h"
#include "opencv\highgui.h"
using namespace cv;
using namespace std;
typedef unsigned char       BYTE;
#define  bpl(imwidth, deep) ((((imwidth)*(deep)<<3)+31)>>5<<2)
int g_Graied_R[256];
int g_Graied_G[256];
int g_Graied_B[256];
void init()
{
	
	const int RRadio = 30;
	const int GRadio = 59;
	const int BRadio = 11;

	for (int i = 0; i < 256; i++)
	{
		g_Graied_R[i] = i * RRadio;
		g_Graied_G[i] = i * GRadio;
		g_Graied_B[i] = i * BRadio;
	}
}
void J_Graied(const BYTE *const srcBmp,const int srcBmpWidth,const int srcBmpHeight,BYTE *dstBmp)
{
	
	
	
		//int bpl_3 = bpl(srcBmpWidth, 3);
		//int bpl_1 = bpl(srcBmpWidth, 1);
	     int bpl_3 = srcBmpWidth * 3;
		 int bpl_1 = srcBmpWidth*1;
		  
		for (int i = 0; i < srcBmpHeight; i++)
		{
			

			int i_nsrcBmp_step = i * bpl_3;
			unsigned int  j_3 = 0;
			unsigned int  i_bpl_1 = i * bpl_1;
			for (int j = 0; j < srcBmpWidth; j++)
			{
				printf("i=%d,j=%d,\n", i,j);

				BYTE R = srcBmp[j_3 + i_nsrcBmp_step + 2];
				BYTE G = srcBmp[j_3 + i_nsrcBmp_step + 1];
				BYTE B = srcBmp[j_3 + i_nsrcBmp_step + 0];

				BYTE gray = (g_Graied_R[R] + g_Graied_G[G] + g_Graied_B[B] + 50) / 100;


				dstBmp[j + i_bpl_1] = gray;
				j_3 += 3;
			}
		}
	
}
void ImageEq(unsigned char *pGry,const int width,const int height)
{

	int i, j;
	int r = 0;
	int hist[256];
	memset(hist, 0, sizeof(int) * 256);
	const int lineByte = bpl(width, 1);
	//计算hist的值
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			hist[pGry[r + j]]++;
		}
		r += lineByte;
	}

	const int N = width*height;
	const int N1 = N * 0.2;
	int vMax = 0;
	int vMin = 0;
	int sum1 = 0, sum2 = 0;
	//计算vMin的值
	for (i = 0; i < 256; i++)
	{
		sum1 += hist[i];
		if (sum1 > N1)
		{
			vMin = i;
			break;
		}
	}
	//计算vMax的值
	for (i = 255; i >= 0; i--)
	{
		sum2 += hist[i];
		if (sum2 > N1)
		{
			vMax = i;
			break;
		}
	}
	float zt = 0.0;
	//计算zt的值
	if (vMax != vMin)
	{
		zt = 255.0 / (vMax - vMin);
		// zt = 255.0f / (vMax - vMin);
	}

	unsigned char *pGry_temp = pGry;
	r = 0;
	//计算pGry 的值
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (pGry_temp[r + j] < vMin)
			{
				pGry_temp[r + j] = 0;
			}
			else if (pGry_temp[r + j] > vMax)
			{
				pGry_temp[r + j] = 255;
			}
			else
			{
				//pGry_temp[r + j] = (pGry_temp[r + j] - vMin) * zt;
				 pGry_temp[r + j] = (pGry_temp[r + j] - vMin) * zt+0.5;
			}
		}
		r += lineByte;
	}

}

//大律法二值化
int otsu(const unsigned char *pGry, const int width, const int height)
{
	const int lineByte = bpl(width, 1);
	const unsigned char *image = pGry;
	int cols = lineByte;
	int x0 = 0;
	int y0 = 0;
	int dx = width;
	int dy = height;

	const unsigned char *np; // 图像指针
	int thresholdValue = 1; // 阈值
	int ihist[256] = { 0 }; // 图像直方图，256个点

	int i = 0, j = 0, k = 0; // various counters
	int n = 0, n1 = 0, n2 = 0;
	double m1 = 0, m2 = 0, sum = 0, csum = 0, fmax = 0, sb = 0;

	// 对直方图置零...
	memset(ihist, 0, sizeof(ihist));

	// 生成直方图，计算ihist的值
	for (i = y0 + 1; i < y0 + dy - 1; i++)
	{
		np = &image[i * cols + x0 + 1];
		for (j = x0 + 1; j < x0 + dx - 1; j++)
		{
			ihist[*np]++;
			np++;
		}
	}

	// set up everything
	sum = csum = 0.0;
	n = 0;
	//计算sum和n的值
	for (k = 0; k <= 255; k++)
	{
		sum += (double)k * ihist[k]; /* x*f(x) 质量矩*/
		n += ihist[k]; /* f(x) 质量 */
	}

	if (!n)
	{
		// if n has no value, there is problems...
		//yu 修改
		printf ( "NOT NORMAL thresholdValue = 160\n");

		return (160);
	}

	// do the otsu global thresholding method
	fmax = -1.0;
	n1 = 0;
	for (k = 0; k < 255; k++)
	{
		n1 += ihist[k];
		if (!n1)
			continue;

		n2 = n - n1;
		if (n2 == 0)
			break;

		csum += (double)k * ihist[k];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = (double)n1 * n2 * (m1 - m2) * (m1 - m2);
		/* bbg: note: can be optimized. */
		if (sb > fmax)
		{
			fmax = sb;
			thresholdValue = k;
		}
	}

	// at this point we have our thresholding value
	return(thresholdValue);
}


extern int readDir(char *dirName, vector<string> &filesName);
int test25(int argc, char *argv[])
{
	string imagepath = "I:/双个车牌放在一起/all";
	string imagesave="I:/双个车牌放在一起/twoplaterresult/";
	vector<string> filesName;
	readDir((char*)imagepath.c_str(), filesName);


	init();

	for (int i = 0; i<filesName.size(); i++)
	{
		string imgPath = filesName[i];
		int pos = imgPath.find_last_of('/');
		int pos2 = imgPath.find_first_of('.');
		string name1 = imgPath.substr(pos + 1, pos2 - pos - 1);
		//string imgPath1 = imgPath.substr(0, pos);
		//pos = imgPath1.find_last_of('/');

		Mat img = imread(imgPath.c_str());
		if (img.data == NULL)
		{
			printf("img.data null:%s\n", name1.c_str());
			system("pause");
			return 0;
		}
		imwrite("1.jpg",img);
		int width = img.cols; 
		int height = img.rows;

		Mat grayImg(height, width, CV_8UC1);
		J_Graied(img.data, width, height, grayImg.data);
		string str0 = imagesave + name1 + "-0.jpg";
		imwrite(str0.c_str(),grayImg);
		ImageEq(grayImg.data, width, height);
		string str1 = imagesave + name1 + "-1.jpg";
		imwrite(str0.c_str(), grayImg);
		int T = otsu(grayImg.data, width, height);
		Mat binImg(height, width, CV_8UC1);
		const int pBin_bpl = bpl(width, 1);
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k <width; k++)
			{
				binImg.data[k + j * pBin_bpl] = grayImg.data[k + j * pBin_bpl] > T ? 255 : 0;
			}
		}

		string str2 = imagesave + name1 + "-2.jpg";
		imwrite(str2.c_str(), binImg);
	}
	


	return 0;
}