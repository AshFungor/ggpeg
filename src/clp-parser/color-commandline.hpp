#include <iostream>

// ConsoleColor
#ifdef _WIN32
#include <windows.h>  
#endif
void setConsoleColor(int color) 
{
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    #else
        std::cerr << "\033[" << color << "m";
    #endif
}
