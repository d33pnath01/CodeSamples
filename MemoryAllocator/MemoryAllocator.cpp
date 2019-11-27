#include <conio.h>

extern bool HeapManager_UnitTest();

void main(int i_argc, char** i_argl)
{
	HeapManager_UnitTest();
	_getch();
}