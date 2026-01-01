/****************************************************************/
/*		FileName: ansicolor.h				*/
/*		Author: David Estes(Chrono)			*/
/*		CopyRight 2004,2005 David Estes			*/
/*		Under the GPL License				*/
/*		FOR NON-COMMERCIAL USE ONLY			*/
/****************************************************************/
//====================
//Revisement History |
//====================
//
//
//
//====================
#ifndef ANSICOLOR_H
#define ANSICOLOR_H
//ANSI COLORS
const char FOREBLACK[11] = "\033[30m";
const char FORERED[11] = "\033[31m";
const char FOREGREEN[11] = "\033[32m";
const char FOREBROWN[11] = "\033[33m";
const char FOREBLUE[11] = "\033[34m";
const char FOREMAGENTA[11] = "\033[35m";
const char FORECYAN[11] = "\033[36m";
const char FOREWHITE[11] = "\033[37m";
const char BACKBLACK[11] = "\033[40m";
const char BACKRED[11] = "\033[41m";
const char BACKGREEN[11] = "\033[42m";
const char BACKBROWN[11] = "\033[43m";
const char BACKBLUE[11] = "\033[44m";
const char BACKMAGENTA[11] = "\033[45m";
const char BACKCYAN[11] = "\033[46m";
const char BACKWHITE[11] = "\033[47m";
const char BOLD[11] = "\033[1m";
const char UNDERSCORE[11] = "\033[4m";
const char BLINK[11] = "\033[5m";
const char REVERSE[11] = "\033[7m";
const char NORMAL[11] = "\033[0m";//returns all settings to normal
/*Escape Sequence \033[3Xm For Foreground And \033[4Xm for Background
	X = 0 black    X = 1 red
        X = 2 green    X = 3 brown
        X = 4 blue     X = 5 magenta
        X = 6 cyan     X = 7 white

*/

//OTHER
const char CLEARSCREEN[11] = "\033[2J";
const char MVCURSOR_HOME[11] = "\033[H";
#endif
