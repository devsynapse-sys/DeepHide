#ifndef CONSOLE_MANAGER_HPP
#define CONSOLE_MANAGER_HPP

#ifdef _WIN32
#include <windows.h>
#include <string>
#endif

class ConsoleManager {
public:
    static void set_font(const std::wstring& font_name, SHORT font_height = 14) {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;

        CONSOLE_FONT_INFOEX cfi{};
        cfi.cbSize = sizeof(cfi);
        GetCurrentConsoleFontEx(hOut, FALSE, &cfi);

        cfi.dwFontSize.Y = font_height;
        cfi.dwFontSize.X = 0;
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        wcsncpy_s(cfi.FaceName, font_name.c_str(), font_name.size());

        SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
#endif
    }

    static void set_green_text() {
#ifdef _WIN32
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
            FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif
    }

    static void reset_color() {
#ifdef _WIN32
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
    }
};

#endif // CONSOLE_MANAGER_HPP