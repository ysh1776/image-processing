#define _CRT_SECURE_NO_WARNINGS
#define WIDTHBYTES(bits)  (((bits)+31)/32*4) //������ 4Byte ���
#include <stdio.h>
#include <stdlib.h>

typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned long  LONG;
typedef unsigned char  BYTE;

#ifndef BI_RGB  
#define BI_RGB         0  
#endif  

#pragma pack(1)     // �е����� ����
//��Ʈ�� ����� �ʿ��� �� ����� ���� 3���� ����ü
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

	fp = fopen("test.raw", "rb");//�о�� ���� -> raw�̹�����
	out = fopen("2018253096_bmp_output2(conter-clockwise).bmp", "wb");//������ ����

	BITMAPFILEHEADER BFH; //��Ʈ�� �������
	BITMAPINFOHEADER BIH; //��Ʈ�� �������
	RGBQUAD hRGB[256]; //�ȷ�Ʈ

	BFH.bfType = 0x4D42; //���� -0x4d42�� ��Ʈ���� �ǹ���
	BFH.bfReserved1 = 0; //�̷��� ���� ����� ���� ���� 0���� ��
	BFH.bfReserved2 = 0;
	BFH.bfOffBits = sizeof(hRGB) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // ->��Ʈ�� ������ ������ġ ��Ÿ��

	BIH.biWidth = (DWORD)512; //����ũ��
	BIH.biHeight = (DWORD)512; //����ũ�� - 512*512������� ������̴� ���� �ϵ��ڵ����־���.
	BIH.biBitCount = 8; // �ȼ� �ϳ��� ǥ���� ��Ʈ ��
	BIH.biSize = sizeof(BITMAPINFOHEADER); // ��Ʈ�� ������� ũ��
	BIH.biPlanes = 1; //����ϴ� �������Ǽ� 1����
	BIH.biCompression = BI_RGB; // ������ - ��Ʈ���� �������� �����Ƿ� 0
	BIH.biSizeImage = ((((BIH.biWidth * BIH.biBitCount) + 31) & ~31) >> 3) * BIH.biHeight; //https://cbw1030.tistory.com/108 ��������
	BIH.biXPelsPerMeter = 0; //�����ػ� ������ 0
	BIH.biYPelsPerMeter = 0; //�����ػ� ������ 0
	BIH.biClrUsed = 0; //�������̺��� ���� ���Ǵ� ���� ��
	BIH.biClrImportant = 0; //��Ʈ�� ǥ���� ���� �ʿ��� ���� �ε��� ��

	BFH.bfSize = ((((BIH.biWidth * BIH.biBitCount) + 31) & ~31) >> 3) * BIH.biHeight + BFH.bfOffBits + 2;// ����ũ�⸦ ��Ÿ�� https://cbw1030.tistory.com/108 ��������

	DWORD rowSize = BIH.biWidth * BIH.biHeight;

	char* data = NULL;

	data = (char*)malloc(sizeof(char) * rowSize + 16); // �޸� �Ҵ�

	if (data) {
		fread(data, 1, rowSize, fp);

		for (int i = 0; i < 256; i++) {				//�ȷ�Ʈ ����
			hRGB[i].rgbRed = (BYTE)(i % 256);
			hRGB[i].rgbGreen = hRGB[i].rgbRed;
			hRGB[i].rgbBlue = hRGB[i].rgbRed;
			hRGB[i].rgbReserved = 0;
		}
		fwrite((char*)&BFH, 1, sizeof(BITMAPFILEHEADER), out);		//��� ���� ����� �۾�
		fwrite((char*)&BIH, 1, sizeof(BITMAPINFOHEADER), out);
		fwrite((char*)hRGB, 1, sizeof(hRGB), out);

		long count = rowSize;

		int rowsize = WIDTHBYTES(BIH.biBitCount * BIH.biWidth);

		unsigned char clock[512][512] = { 0 };

		for (int i = 0; i < 512; i++) //�ݽð�90��
		{
			for (int j = 0; j < 512; j++)
			{
				clock[511 - j][i] = data[(i * 512) + j]; //[�� : -->][�� : ||]
			}
		}
		unsigned char temp = 0;

		for (int i = 0; i < rowsize / 2; i++) // �������� �߰����� 256�ȼ��� �����ص� ��������, ���� ���� ���ִ� clock�� ������
		{
			for (int j = 0; j < 512; j++)
			{
				temp = clock[i][j];
				clock[i][j] = clock[511 - i][j]; // �ϳ��� �ٿ����鼭 �� �ٲٱ�
				clock[511 - i][j] = temp;
			}
		}


		fwrite(clock, sizeof(char), BIH.biSizeImage, out); //out���Ͽ� ����
		free(data);//�޸�����
	}


	fclose(fp);//���ϴݱ�
	fclose(out);


}