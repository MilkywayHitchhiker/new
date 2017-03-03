#pragma once
#include"stdafx.h"
#include<string>
#include<time.h>
#define memmax 256
#define NOALLOCMAX 10


#undef new
void *operator new (size_t Size,char *File,int LINE);
void *operator new[] (size_t Size, char *File, int LINE);
void operator delete(void *p, char *FILE, int LINE);
void operator delete[] (void *p, char *FILE, int LINE);
void operator delete(void *p);
void operator delete[](void *p);
#define new new(__FILE__,__LINE__)