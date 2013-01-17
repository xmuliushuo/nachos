#include "syscall.h"

int main(int argc, char *argv[])
{
	int temp[5], i;
	for (i = 0; i < 3; i++) {
		temp[i] = Exec("../test/circle");
	}
	for (i = 0; i < 3; i++) {
		Join(temp[i]);
	}
	Exit(0);
}