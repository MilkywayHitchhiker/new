// new.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

