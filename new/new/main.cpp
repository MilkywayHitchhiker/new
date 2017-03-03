// new.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "my_new.h"

int main()
{
	int *a;
	int *b;
	int *c;

	a = new int;
	b = new int;
	c = new int[10];

	delete b;
	delete c;


    return 0;
}

