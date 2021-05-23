#define _CRT_SECURE_NO_WARNINGS
#define WIDTHBYTES(bits)  (((bits)+31)/32*4) //가로줄 4Byte 배수
#include <stdio.h>
#include <stdlib.h>

typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned long  LONG;
typedef unsigned char  BYTE;

#ifndef BI_RGB  
#define BI_RGB         0  
#endif  

#pragma pack(1)     // 패딩단위 설정
//비트맵 헤더에 필요한 각 헤더를 위한 3개의 구조체
typedef struct tagBITMAPFILEHEADER
{
	WORD   bfType;
	DWORD  bfSize;
	WORD   bfReserved1;
	WORD   bfReserved2;
	DWORD  bfOffBits;
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD   biSize;
	LONG    biWidth;
	LONG    biHeight;
	WORD    biPlanes;
	WORD    biBitCount;
	DWORD   biCompression;
	DWORD   biSizeImage;
	LONG    biXPelsPerMeter;
	LONG    biYPelsPerMeter;
	DWORD   biClrUsed;
	DWORD   biClrImportant;
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
}RGBQUAD;

#pragma pack()  


int main() {


	FILE* fp;
	FILE* out;

	fp = fopen("test.raw", "rb");//읽어올 파일 -> raw이미지값
	out = fopen("2018253096_bmp_output2(conter-clockwise).bmp", "wb");//쓰기할 파일

	BITMAPFILEHEADER BFH; //비트맵 파일헤더
	BITMAPINFOHEADER BIH; //비트맵 정보헤더
	RGBQUAD hRGB[256]; //팔레트

	BFH.bfType = 0x4D42; //형식 -0x4d42는 비트맵을 의미함
	BFH.bfReserved1 = 0; //미래를 위해 예약된 공간 보통 0으로 둠
	BFH.bfReserved2 = 0;
	BFH.bfOffBits = sizeof(hRGB) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // ->비트맵 데이터 시작위치 나타냄

	BIH.biWidth = (DWORD)512; //가로크기
	BIH.biHeight = (DWORD)512; //세로크기 - 512*512사이즈로 만들것이니 값을 하드코딩해주었음.
	BIH.biBitCount = 8; // 픽셀 하나를 표현할 비트 수
	BIH.biSize = sizeof(BITMAPINFOHEADER); // 비트맵 정보헤더 크기
	BIH.biPlanes = 1; //사용하는 색상판의수 1고정
	BIH.biCompression = BI_RGB; // 압축방식 - 비트맵은 압축하지 않으므로 0
	BIH.biSizeImage = ((((BIH.biWidth * BIH.biBitCount) + 31) & ~31) >> 3) * BIH.biHeight; //https://cbw1030.tistory.com/108 공식참고
	BIH.biXPelsPerMeter = 0; //가로해상도 보통은 0
	BIH.biYPelsPerMeter = 0; //세로해상도 보통은 0
	BIH.biClrUsed = 0; //색상테이블에서 실제 사용되는 색상 수
	BIH.biClrImportant = 0; //비트맵 표현을 위해 필요한 색상 인덱스 수

	BFH.bfSize = ((((BIH.biWidth * BIH.biBitCount) + 31) & ~31) >> 3) * BIH.biHeight + BFH.bfOffBits + 2;// 파일크기를 나타냄 https://cbw1030.tistory.com/108 공식참고

	DWORD rowSize = BIH.biWidth * BIH.biHeight;

	char* data = NULL;

	data = (char*)malloc(sizeof(char) * rowSize + 16); // 메모리 할당

	if (data) {
		fread(data, 1, rowSize, fp);

		for (int i = 0; i < 256; i++) {				//팔레트 세팅
			hRGB[i].rgbRed = (BYTE)(i % 256);
			hRGB[i].rgbGreen = hRGB[i].rgbRed;
			hRGB[i].rgbBlue = hRGB[i].rgbRed;
			hRGB[i].rgbReserved = 0;
		}
		fwrite((char*)&BFH, 1, sizeof(BITMAPFILEHEADER), out);		//헤더 파일 만드는 작업
		fwrite((char*)&BIH, 1, sizeof(BITMAPINFOHEADER), out);
		fwrite((char*)hRGB, 1, sizeof(hRGB), out);

		long count = rowSize;

		int rowsize = WIDTHBYTES(BIH.biBitCount * BIH.biWidth);

		unsigned char clock[512][512] = { 0 };

		for (int i = 0; i < 512; i++) //반시계90도
		{
			for (int j = 0; j < 512; j++)
			{
				clock[511 - j][i] = data[(i * 512) + j]; //[행 : -->][열 : ||]
			}
		}
		unsigned char temp = 0;

		for (int i = 0; i < rowsize / 2; i++) // 맨위에서 중간까지 256픽셀만 수행해도 뒤집어짐, 돌린 값이 들어가있는 clock을 뒤집기
		{
			for (int j = 0; j < 512; j++)
			{
				temp = clock[i][j];
				clock[i][j] = clock[511 - i][j]; // 하나씩 줄여가면서 값 바꾸기
				clock[511 - i][j] = temp;
			}
		}


		fwrite(clock, sizeof(char), BIH.biSizeImage, out); //out파일에 쓰기
		free(data);//메모리해제
	}


	fclose(fp);//파일닫기
	fclose(out);


}