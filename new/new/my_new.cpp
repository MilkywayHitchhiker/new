#include"stdafx.h"
#include"my_new.h"
#include<malloc.h>


class Memory_Checker
{
public:
	struct Allocation
	{
		void *p;
		int Size;
		bool Array;
		char FILEName[64];
		int LINE;
		char ERROR[16];

	};


	//나중에 링크드 리스트 형식으로 변환작업 시행.
	Allocation memory[memmax];
	void *NOALLOC[NOALLOCMAX];
	int holepin;


public:
	Memory_Checker (void)
	{
		//나중에 링크드 리스트 형식으로 변환시 제거.
		holepin = 0;
		for ( int cnt = 0; cnt < memmax; cnt++ )
		{
			memory[cnt].p = NULL;
		}
		for ( int cnt = 0; cnt < memmax; cnt++ )
		{
			NOALLOC[cnt] = NULL;
		}
	}
	~Memory_Checker (void)
	{
		FOut_Log ();

	}

	void FOut_Log (void)
	{
		FILE *fp;
		char filename[64];
		errno_t err;


		///////////////////////
		//프로그램 종료시간값 구해서 파일 이름으로 적용.
		//////////////////////
		time_t timer;
		tm t;

		timer = time (NULL);
		_localtime64_s (&t, &timer);

		t.tm_year += 1900;
		t.tm_mon += 1;

		//파일 이름저장
		sprintf_s (filename, "%d년 %d월 %d일 %d시 %d분 %d초.txt", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);


		err = fopen_s (&fp, filename, "w");

		//최상위에 들어가는 목차
		fprintf_s (fp, "[%-7s]     [%s]     [%5s] \n", "Address", " 포인터 ", "Size");


		//할당 안된 메모리를 해제 하려 한 경우 NOALLOC메모리에 저장되고 이를 출력
		for ( int cnt = 0; cnt < NOALLOCMAX; cnt++ )
		{
			if ( memory[cnt].p != NULL )
			{
				fprintf_s (fp, " %-7s  [%p] \n", "NOALLOC", NOALLOC[cnt]);
			}
		}

		//배열을 돌면서 해제 안된 메모리와 잘못 해제된 메모리를 찾아서 파일로 출력
		for ( int cnt = 0; cnt < memmax; cnt++ )
		{
			if ( memory[cnt].p != NULL )
			{
				fprintf_s (fp, " %-7s  [%p] [%5d] %s : %d\n", memory[cnt].ERROR, memory[cnt].p, memory[cnt].Size, memory[cnt].FILEName, memory[cnt].LINE);
			}
		}


		fclose (fp);

	}

};




Memory_Checker Leak;

#undef new
void *operator new (size_t Size, char *File, int LINE)
{

	void *p = malloc (Size);

	//메모리 셋팅 및 내역저장
	Leak.memory[Leak.holepin].p = p;
	strcpy_s (Leak.memory[Leak.holepin].FILEName, File);
	Leak.memory[Leak.holepin].LINE = LINE;
	Leak.memory[Leak.holepin].Array = false;
	Leak.memory[Leak.holepin].Size = Size;
	strcpy_s(Leak.memory[Leak.holepin].ERROR, "LEAK");


	//나중에 링크드 리스트 형식으로 변환시 제거.
	for ( int cnt = 0; cnt < memmax; cnt++ )
	{
		if ( Leak.memory[cnt].p == NULL )
		{
			Leak.holepin = cnt;
			break;
		}
	}


	return p;
}
void *operator new[](size_t Size, char *FILE, int LINE)
{
	void *p = malloc (Size);

	//메모리 셋팅 및 내역저장
	Leak.memory[Leak.holepin].p = p;
	strcpy_s (Leak.memory[Leak.holepin].FILEName, FILE);
	Leak.memory[Leak.holepin].LINE = LINE;
	Leak.memory[Leak.holepin].Array = true;
	Leak.memory[Leak.holepin].Size = Size;

	//삭제안된 메모리만 저장할 것이므로 모든 메모리에 에러명령 LEAK을 저장해 둔다.
	strcpy_s (Leak.memory[Leak.holepin].ERROR, "LEAK");


	//나중에 링크드 리스트 형식으로 변환시 제거.
	for ( int cnt = 0; cnt < memmax; cnt++ )
	{
		if ( Leak.memory[cnt].p == NULL )
		{
			Leak.holepin = cnt;
			break;
		}
	}


	return p;
}

void operator delete(void *p, char *FILE, int LINE)
{
}
void operator delete[](void *p, char *FILE, int LINE)
{
}


void operator delete(void *p)
{
	for ( int cnt = 0; cnt < memmax; cnt++ )
	{
		if ( Leak.memory[cnt].p == p )
		{
			if ( Leak.memory[cnt].Array == false )
			{
				free (Leak.memory[cnt].p);
				Leak.memory[cnt].p = NULL;
			}

			//배열형태의 잘못된 삭제명령
			strcpy_s (Leak.memory[cnt].ERROR, "ARRAY");
		}
	}

	//해당 메모리 없음 파일구조체에 에러 전달
	for ( int cnt = 0; cnt < NOALLOCMAX; cnt++ )
	{
		if ( Leak.NOALLOC[cnt] == NULL )
		{
			Leak.NOALLOC[cnt] = p;
		}
	}

}


void operator delete[](void *p)
{
	for ( int cnt = 0; cnt < memmax; cnt++ )
	{
		if ( Leak.memory[cnt].p == p )
		{
			if ( Leak.memory[cnt].Array == true )
			{
				free (Leak.memory[cnt].p);
				Leak.memory[cnt].p = NULL;
			}
			else
			{
				//배열형태의 잘못된 삭제명령
				strcpy_s (Leak.memory[cnt].ERROR, "ARRAY");
			}
		}
	}

	//해당 메모리 없음 파일구조체에 에러 전달
	for ( int cnt = 0; cnt < NOALLOCMAX; cnt++ )
	{
		if ( Leak.NOALLOC[cnt] == NULL )
		{
			Leak.NOALLOC[cnt] = p;
		}
	}

}