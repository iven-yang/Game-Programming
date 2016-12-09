
#include "Mainframe.h"

int main(int argc, char *argv[]){
	Mainframe game;

	while (!game.UpdateAndRender()){}
	return 0;
}