#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main()
{
	unsigned char Image[512][512];
	FILE* fp;
	fp = fopen("test.raw", "wb");

    for (int i = 0; i < 512; i++) 
    {
        for (int j = 0; j < 100; j++)
            Image[i][j] = (char)120;

        for (int j = 100; j < 200; j++)
            Image[i][j] = (char)(j * 3 / 20 + 105);

        for (int j = 200; j < 280; j++)
            Image[i][j] = (char)(j * 9 / 8 - 90);

        for (int j = 280; j < 300; j++)
            Image[i][j] = (char)(j * 3 / 4 + 15);

        for (int j = 300; j < 512; j++)
            Image[i][j] = (char)240;
    }

	fwrite(Image,sizeof(char),512 * 512,fp);
	fclose(fp);

    return 0;
}