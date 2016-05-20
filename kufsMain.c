#include <stdio.h>
#include "kufs.h"
#include "kufs.cpp"

char buffer[1024];

int main( int argc, const char* argv[] )
{
	readKUFS(2, buffer);
	printf( "\nHello World\n\n" );
	printf(buffer);
	printf("\n");
}
