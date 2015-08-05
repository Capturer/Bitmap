#define _CRT_SECURE_NO_DEPRECATE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#include "wingdi.h"
#include "BmpHeader.h"



BYTE *pNewBmpbuf,*pBmpbuf,*buf,*buf1,*buf2;
LONG newLinebyte;
LONG I0, L0;
LONG i, j, k;

int bmpWidth;
int bmpHeight;
int biBitcount;
RGBQUAD *pColorTable;
LONG desWidth;
LONG desHeight;

int blendBmp(const char* bmp1Name,const char* bmp2Name,float alpha) {//位图混合函数

	FILE *fp1 = fopen(bmp1Name, "rb");
	FILE *fp2 = fopen(bmp2Name, "rb");
	FILE *fp3 = fopen("bmp3.bmp","wb");
	if (fp1 == NULL) {
		printf_s("cannot open bmp1\n");
		return -1;
	}
	if (fp2 == NULL) {
		printf_s("cannot open bmp2\n");
		return -1;
	}
	if (fp3 == NULL){
		printf_s("cannot open bmp2\n");
		return -1;
	}

	//fseek(fp1, sizeof(BITMAPFILEHEADER), 0);
	BITMAPFILEHEADER fileHead1;
	BITMAPINFOHEADER infoHead1;
	BITMAPFILEHEADER fileHead2;
	BITMAPINFOHEADER infoHead2;
	fread(&fileHead1,sizeof(BITMAPFILEHEADER), 1,fp1);//读取图片1的文件头
	fread(&infoHead1, sizeof(BITMAPINFOHEADER), 1,fp1);//读取图片1的信息头
	fwrite(&fileHead1, sizeof(BITMAPFILEHEADER), 1, fp3);//写入图片3的文件头
	fwrite(&infoHead1, sizeof(BITMAPINFOHEADER), 1, fp3);//写入文件3的信息头

	fread(&fileHead2,sizeof(BITMAPFILEHEADER),1,fp2);
	fread(&infoHead2,sizeof(BITMAPINFOHEADER),1,fp2);

	int bmpSize = infoHead1.biWidth*infoHead1.biHeight * 3;
	buf = (BYTE*)malloc(bmpSize * sizeof(BYTE));
	buf1 = (BYTE*)malloc(bmpSize * sizeof(BYTE));
	buf2 = (BYTE*)malloc(bmpSize * sizeof(BYTE));
	
	memset(buf1, 0, bmpSize);
	memset(buf2, 0, bmpSize);
	memset(buf, 0, bmpSize);
	fread(buf2, bmpSize, 1, fp2);
	fread(buf1, bmpSize, 1, fp1);

	for (int i = 0;i < bmpSize;i++)
	{
		buf[i] = buf1[i]*alpha + buf2[i]*(1 - alpha);//对每一个像素点做处理
	}
	fwrite(buf,1, bmpSize, fp3);

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	return 0;
}


int readBmp(const char* bmpName) {//读取图片信息
	FILE *fp = fopen(bmpName, "rb");
	if (fp == NULL) {
		printf("cannot open file\n");
		return -1;
	}
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	bmpWidth = head.biWidth;
	bmpHeight = head.biHeight;
	biBitcount = head.biBitCount;
	int lineByte = (bmpWidth*biBitcount / 8 + 3) / 4 * 4;
	pBmpbuf = (BYTE*)malloc(sizeof(BYTE)*lineByte*bmpHeight);
	fread(pBmpbuf, 1, lineByte*bmpHeight, fp);
	fclose(fp);
	return 0;
}

int saveBmp(const char* bmpName, BYTE* imgBuf, int width, int height, int biBitcount, RGBQUAD *pColorTable) {//保存图片信息
	if (!imgBuf)
		return -1;
	int colorTablesize = 0;
	int lineByte = (width*biBitcount / 8 + 3) / 4 * 4;
	FILE *fp = fopen(bmpName, "wb");
	if (fp == NULL) {
		return -1;
	}

	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4d42;
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte*height;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	fileHead.bfOffBits = 54 + colorTablesize;
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	BITMAPINFOHEADER infoHead;
	infoHead.biSize = 40;
	infoHead.biBitCount = biBitcount;
	infoHead.biPlanes = 1;
	infoHead.biSizeImage = lineByte*height;
	infoHead.biWidth = width;
	infoHead.biHeight = height;
	infoHead.biXPelsPerMeter = 0;
	infoHead.biYPelsPerMeter = 0;
	infoHead.biClrImportant = 0;
	infoHead.biClrUsed = 0;
	infoHead.biCompression = 0;
	fwrite(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(imgBuf, height*lineByte, 1, fp);

	fclose(fp);
	return 0;
}


void zoomBmp(const char* srcBmp,const char* desBmp,float rate) {//图片缩放
	readBmp(srcBmp);
	desWidth = (LONG)(bmpWidth*rate + 0.5);
	desHeight = (LONG)(bmpHeight*rate + 0.5);
	newLinebyte = (desWidth * biBitcount / 8 + 3) / 4 * 4;
	pNewBmpbuf = (BYTE *)malloc(sizeof(BYTE)*newLinebyte*desHeight);

	int lineByte = (bmpWidth*biBitcount / 8 + 3) / 4 * 4;
	for (i = 0;i < desHeight;i++) {
		for (j = 0;j < desWidth;j++) 
			for (k = 0;k < 3;k++) {
				I0 = (LONG)(i / rate + 0.5);
				L0 = (LONG)(j / rate + 0.5);
				if ((L0 >= 0) && (L0 < bmpWidth) && (I0 >= 0) && (I0 < bmpHeight)) {
					*(pNewBmpbuf + i*newLinebyte + j * 3 + k) = *(pBmpbuf + I0*lineByte + L0 * 3 + k);
				}
				else {
					*(pNewBmpbuf + i*newLinebyte + j * 3 + k) = 255;
				}
			}
	}

	saveBmp(desBmp, pNewBmpbuf, desWidth, desHeight, biBitcount, pColorTable);
	free(pNewBmpbuf);
	//memset(pNewBmpbuf, 0, sizeof(BYTE)*newLinebyte*desHeight);
	free(pColorTable);
}

int main() {
	float alpha;
	float rate;
	const char* bmp1Name = "mybitmap.bmp";
	const char* bmp2Name = "mybitmap-1.bmp";
	const char* bmp3Name = "test1.bmp";

	printf("请输入缩放倍数(结束:-1):\n");
	while (scanf("%f",&rate)) {
		getchar();
		if (rate == -1)
			break;
		zoomBmp(bmp2Name, bmp3Name, rate);
		printf("--------缩放完成--------\n");
		printf("请输入缩放倍数(结束:-1):\n");
	}

	printf("请输入Alpha值(0-1)(结束:-1):\n");
	while (1) {
		scanf("%f", &alpha);
		if (((alpha < 0) || (alpha>1))&&(alpha != -1)) {
			printf("不合法的输入值!\n");
			printf("请输入Alpha值(0-1)(结束:-1):\n");
			continue;
		}
		if (alpha == -1)
			break;
		blendBmp(bmp1Name, bmp2Name, alpha);
		printf("---------混合完成---------\n");
		printf("请输入Alpha值(0-1)(结束:-1):\n");
	}
	return 0;
}