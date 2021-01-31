/*
- BinPack
- Pack *.bin archive files from (DS, 3DS) HM Series
- Support Grand Bazzar, The Tale of Two Towns
----------------How To Use
- Just Drag And Drop Directory To BinPack.exe
- Directory Must Be Made From BinUnpack.exe
-
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <shlwapi.h>
#include "tinydir.h"
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "shlwapi.lib")
#define EXIT(text) { printf(text); exit(EXIT_FAILURE); }
void Pack(char* filename);
void Usage(void);
char* Mid(char txt[], int start, int count);
void BubbleSort(int *list[], int Count)
{
	for (int i = 0; i < Count - 1; i++)
	{
		for (int j = 0; j < Count - 1 - i; j++)
		{
			if (list[j] > list[j + 1])
			{

				int temp = list[j];
				list[j] = list[j + 1];
				list[j + 1] = temp;
			}
		}
	}
	return;
}
int* SubFilePointerArray;
int* SubFileSizeArray;
int** FileArray;



int main(int argc, char** argv[])
{
	if (argc == 2) Pack(argv[1]);
	else Usage();

	printf("\nDone\n");


	return 0;
}



void Pack(char* Dirname)
{
	/*
		Dirname ���丮��
		FileName ������� ���� ��(=���丮 ��)
	*/
	tinydir_dir dir;
	FILE* SaveFile;
	int FileCount = 0;
	int k = 0;
	int *FileList;
	int TempBinary = 0;
	long StartAdd;
	long SubStartAdd;
	long SubEndAdd; //���� ����� ���� ����
	long RealSubEndAdd; //���� ���� ����� ���� ����(���� ������ 4�� ����� ����Ǿ����.
	char* RealFileOrd[10] = { 0, };
	char* TempDir = (char*)malloc(sizeof(char) * strlen(Dirname));
	char* FileName[MAX_PATH];
	char* SaveFileName[MAX_PATH];
	strcpy(TempDir, Dirname);
	strcpy(FileName, Dirname);
	PathStripPathA(FileName);
	printf("File Name : %s\n", FileName);
	
	//----------------���� ���� ���
	tinydir_open(&dir, TempDir);
	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (!file.is_dir) FileCount++;
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	printf("File Count : %d\n\n\n\n", FileCount);

	//----------------���� ������ �迭 ����� �����ϱ�
	FileList = malloc(sizeof(int) * FileCount);
	tinydir_open(&dir, TempDir);
	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (!file.is_dir)
		{
			strncpy(RealFileOrd, (file.name) + strlen(FileName)+1, strlen(file.name) - (strlen(FileName))); //���� �κи� ����
			FileList[k] = atoi(RealFileOrd); //�迭�� ���ںκи� ����
			k++;
		}
		
		tinydir_next(&dir);
	}
	tinydir_close(&dir);

	BubbleSort(FileList, FileCount); //�������� ����
	strcpy(SaveFileName, FileName);
	strcat(SaveFileName, ".bin");

	//------------------ ���̳ʸ��� ����� �̸� �ֱ�--------
	SaveFile = fopen(SaveFileName, "wb+, ccs=UTF-16LE"); //���� �о����

	fwrite(&FileCount, sizeof(int), 1, SaveFile);
	for (int i = 0; i < FileCount*2; i++)
	{
		fwrite(&TempBinary , sizeof(int), 1, SaveFile); // �� ó�� ��ϵ� ��Ʈ���� ���� ��������� �� ��
	}
	StartAdd = ftell(SaveFile); //StartAdd�� SaveFile �ּҰ� ����
	SubEndAdd = ftell(SaveFile); //0��° �ε����� Seek�ϱ����ؼ� ����
	fclose(SaveFile);



	for (int i = 0; i < FileCount; i++)
	{
		FILE* ReadFile;
		int SubFileSize;
		long Write1;
		long Write2;
		char* OpenDir = (char*)malloc(sizeof(char) * (strlen(Dirname) + strlen(FileName) + 10));
		unsigned int* FileStream;


		SaveFile = fopen(SaveFileName, "rb+");
		fseek(SaveFile, 0, SEEK_END); //���� �� ���� ����ϱ����� ���� ������ �̵�
		SubStartAdd = ftell(SaveFile);
		sprintf(OpenDir, "%s\\%s_%d", Dirname, FileName,FileList[i]);
		//printf("%s\n", OpenDir);
		ReadFile = fopen(OpenDir, "rb"); //���� �о����


		fseek(ReadFile, 0, SEEK_END);    // ���� �����͸� ������ ������ �̵���Ŵ
		SubFileSize = ftell(ReadFile);          // ���� �������� ���� ��ġ�� ����
		
		//--------------���� ������ ���� �� ���� ���-------------
		
		//printf("FileSize : %x\n", SubFileSize);
		FileStream = (int*)malloc(SubFileSize);
		fseek(ReadFile, 0, SEEK_SET);
		fread(FileStream, SubFileSize, 1, ReadFile);
		fwrite(FileStream, SubFileSize, 1, SaveFile);
		SubEndAdd = ftell(SaveFile);
		RealSubEndAdd = ftell(ReadFile);
		
		while (RealSubEndAdd % 4 != 0) // ������ 4����Ʈ�� ����� ����Ǿ���ϹǷ� �߰������� 0 �ٿ���
		{
			fwrite(&TempBinary, 1, 1, SaveFile);
			RealSubEndAdd = ftell(SaveFile);
		}
		
		fclose(SaveFile);

		//---------------������ ����--------------
		SaveFile = fopen(SaveFileName, "rb+, ccs=UTF-16LE"); //���� �о����
		fseek(SaveFile, 4 + (8 * i), SEEK_SET);
		Write1 = SubStartAdd - StartAdd;
		Write2 = SubEndAdd - SubStartAdd;
		fwrite(&Write1, sizeof(long), 1, SaveFile);
		fwrite(&Write2, sizeof(long), 1, SaveFile); //������ 2�� ���

		fclose(SaveFile);
		fclose(ReadFile);


		//Sleep(10000);
	}

	
}
void Usage(void)
{
	EXIT(
		"Usage: BinPack [DIR_NAME] \n"
	);
	return;
}

