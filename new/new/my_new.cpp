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


	//���߿� ��ũ�� ����Ʈ �������� ��ȯ�۾� ����.
	Allocation memory[memmax];
	void *NOALLOC[NOALLOCMAX];
	int holepin;


public:
	Memory_Checker (void)
	{
		//���߿� ��ũ�� ����Ʈ �������� ��ȯ�� ����.
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
		//���α׷� ����ð��� ���ؼ� ���� �̸����� ����.
		//////////////////////
		time_t timer;
		tm t;

		timer = time (NULL);
		_localtime64_s (&t, &timer);

		t.tm_year += 1900;
		t.tm_mon += 1;

		//���� �̸�����
		sprintf_s (filename, "%d�� %d�� %d�� %d�� %d�� %d��.txt", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);


		err = fopen_s (&fp, filename, "w");

		//�ֻ����� ���� ����
		fprintf_s (fp, "[%-7s]     [%s]     [%5s] \n", "Address", " ������ ", "Size");


		//�Ҵ� �ȵ� �޸𸮸� ���� �Ϸ� �� ��� NOALLOC�޸𸮿� ����ǰ� �̸� ���
		for ( int cnt = 0; cnt < NOALLOCMAX; cnt++ )
		{
			if ( memory[cnt].p != NULL )
			{
				fprintf_s (fp, " %-7s  [%p] \n", "NOALLOC", NOALLOC[cnt]);
			}
		}

		//�迭�� ���鼭 ���� �ȵ� �޸𸮿� �߸� ������ �޸𸮸� ã�Ƽ� ���Ϸ� ���
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

	//�޸� ���� �� ��������
	Leak.memory[Leak.holepin].p = p;
	strcpy_s (Leak.memory[Leak.holepin].FILEName, File);
	Leak.memory[Leak.holepin].LINE = LINE;
	Leak.memory[Leak.holepin].Array = false;
	Leak.memory[Leak.holepin].Size = Size;
	strcpy_s(Leak.memory[Leak.holepin].ERROR, "LEAK");


	//���߿� ��ũ�� ����Ʈ �������� ��ȯ�� ����.
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

	//�޸� ���� �� ��������
	Leak.memory[Leak.holepin].p = p;
	strcpy_s (Leak.memory[Leak.holepin].FILEName, FILE);
	Leak.memory[Leak.holepin].LINE = LINE;
	Leak.memory[Leak.holepin].Array = true;
	Leak.memory[Leak.holepin].Size = Size;

	//�����ȵ� �޸𸮸� ������ ���̹Ƿ� ��� �޸𸮿� ������� LEAK�� ������ �д�.
	strcpy_s (Leak.memory[Leak.holepin].ERROR, "LEAK");


	//���߿� ��ũ�� ����Ʈ �������� ��ȯ�� ����.
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

			//�迭������ �߸��� �������
			strcpy_s (Leak.memory[cnt].ERROR, "ARRAY");
		}
	}

	//�ش� �޸� ���� ���ϱ���ü�� ���� ����
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
				//�迭������ �߸��� �������
				strcpy_s (Leak.memory[cnt].ERROR, "ARRAY");
			}
		}
	}

	//�ش� �޸� ���� ���ϱ���ü�� ���� ����
	for ( int cnt = 0; cnt < NOALLOCMAX; cnt++ )
	{
		if ( Leak.NOALLOC[cnt] == NULL )
		{
			Leak.NOALLOC[cnt] = p;
		}
	}

}