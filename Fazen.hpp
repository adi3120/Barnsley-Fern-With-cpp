#include <string>
#include <windows.h>
#include <sstream>
#include <vector>
#include <chrono>
#include <cmath>
using std::wstring;
#define redF FOREGROUND_RED | FOREGROUND_INTENSITY
#define redB BACKGROUND_RED | BACKGROUND_INTENSITY
#define blueF FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define blueB BACKGROUNDGROUND_BLUE | BACKGROUND_INTENSITY
#define greenF FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define greenB BACKGROUND_GREEN | BACKGROUND_INTENSITY
#define whiteF FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN
#define whiteB BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY
#define STOP run = false
#define QUICKTRANSLATE \
    x += center_x;     \
    y += center_y;
#define TIMER                                             \
    tp2 = std::chrono::system_clock::now();               \
    std::chrono::duration<float> elapsedTime = tp2 - tp1; \
    tp1 = tp2;                                            \
    float felapsedTime = elapsedTime.count();
#define TIMEVARS                                 \
    auto tp1 = std::chrono::system_clock::now(); \
    auto tp2 = std::chrono::system_clock::now();

const float pi = 3.141592653589793238;
//if some error related to console_font_infoEx occurs comment this part till the last #endif//
typedef struct _CONSOLE_FONT_INFOEX
{
    ULONG cbSize;
    DWORD nFont;
    COORD dwFontSize;
    UINT FontFamily;
    UINT FontWeight;
    WCHAR FaceName[LF_FACESIZE];
} CONSOLE_FONT_INFOEX, *PCONSOLE_FONT_INFOEX;

#ifdef __cplusplus
extern "C"
{
#endif
    BOOL WINAPI SetCurrentConsoleFontEx(HANDLE hConsoleOutput, BOOL bMaximumWindow, PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx);
#ifdef __cplusplus
}
#endif
class Fazen
{
protected:
    HANDLE outhnd;
    HANDLE inhnd;
    SMALL_RECT rect_win;
    CHAR_INFO *buffscreen = new CHAR_INFO[s_width * s_height];
    COORD characterPos = {0, 0};
    COORD buffersize = {short(s_width), short(s_height)};
    float center_x = 0;
    float center_y = 0;

public:
    Fazen()
    {
        outhnd = GetStdHandle(STD_OUTPUT_HANDLE);
        inhnd = GetStdHandle(STD_INPUT_HANDLE);

        rect_win = {0, 0, (short)(s_width - 1), short(s_height - 1)};

        // Enable the window and mouse input events.
    }
    float mapBounds(float value, float x1, float x2, float y1, float y2)
    {
        float m = (y2 - y1) / (x2 - x1);
        float b = y1 - m * x1;
        value = (value - b) / m;
        return value;
    }
    const void display()
    {
        WriteConsoleOutputW(outhnd, buffscreen, buffersize, characterPos, &rect_win);
    }
    const void background(short col = 0)
    {
        for (int i = 0; i < s_height * s_width; i++)
        {

            buffscreen[i].Char.UnicodeChar = ' ';
            buffscreen[i].Attributes = col;
        }
    }
    const void Plot(float x, float y, short col = whiteF, short c = 0x2588)
    {
        QUICKTRANSLATE
        if (x >= 0 && x <= s_width && y >= 0 && y <= s_height)
        {

            buffscreen[int(x) + s_width * int(y)].Char.UnicodeChar = c;
            buffscreen[int(x) + s_width * int(y)].Attributes = col;
        }
    }
    void PlotLineLow(double x0, double y0, double x1, double y1, short col = whiteF)
    {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int yi = 1;
        if (dy < 0)
        {
            yi = -1;
            dy = -dy;
        }
        int D = 2 * dy - dx;
        int y = y0;

        for (int x = x0; x <= x1; x++)
        {
            Plot(x, y, col);
            if (D > 0)
            {
                y += yi;
                D -= 2 * dx;
            }
            D += 2 * dy;
        }
    }

    void PlotLineHigh(double x0, double y0, double x1, double y1, short col = whiteF)
    {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int xi = 1;
        if (dx < 0)
        {
            xi = -1;
            dx = -dx;
        }
        int D = 2 * dx - dy;
        int x = x0;

        for (int y = y0; y <= y1; y++)
        {
            Plot(x, y, col);

            if (D > 0)
            {
                x += xi;
                D -= 2 * dy;
            }
            D += 2 * dx;
        }
    }

    //Bresenham's algorithm for drawing line
    void drawLine(double x0, double y0, double x1, double y1, short col = whiteF)
    {
        float angle = 0;
        angle = angle + pi;
        float rx1 = x1 * cos(angle) + y1 * sin(angle);
        float ry1 = x1 * sin(angle) - y1 * cos(angle);
        rx1 = x1;
        ry1 = y1;

        if (std::abs(ry1 - y0) < std::abs(rx1 - x0))
        {
            if (x0 > rx1)
            {
                PlotLineLow(rx1, ry1, x0, y0, col);
            }
            else
            {
                PlotLineLow(x0, y0, rx1, ry1, col);
            }
        }
        else
        {
            if (y0 > ry1)
            {
                PlotLineHigh(rx1, ry1, x0, y0, col);
            }
            else
            {
                PlotLineHigh(x0, y0, rx1, ry1, col);
            }
        }
    }

    void translate(float x, float y)
    {
        center_x = x;
        center_y = y;
    }

    const void drawTextFromInt(int x, int y, wstring s, int b, short col = whiteF)
    {
        QUICKTRANSLATE
        std::wostringstream ws;
        ws << b;

        std::wstring a = s + (ws.str());

        for (size_t i = 0; i < a.size(); i++)
        {
            buffscreen[x + i + s_width * y].Char.UnicodeChar = a[i];
            buffscreen[x + i + s_width * y].Attributes = col;
        }
    }
    const void Text(int x, int y, wstring a, short col = whiteF)
    {
        QUICKTRANSLATE
        for (size_t i = 0; i < a.size(); i++)
        {
            buffscreen[x + i + s_width * y].Char.UnicodeChar = a[i];
            buffscreen[x + i + s_width * y].Attributes = col;
        }
    }
}

;

class Fazen2d : public Fazen
{
public:
    const void make2DConsole(int fontw = 8, int fonth = 8, LPCTSTR title = "Your Title Here")
    {

        CONSOLE_FONT_INFOEX fontStructure = {0};
        fontStructure.cbSize = sizeof(fontStructure);
        fontStructure.dwFontSize.X = fontw;
        fontStructure.dwFontSize.Y = fonth;
        fontStructure.FaceName, L"Consolas";
        SetConsoleTitle(title);

        SetCurrentConsoleFontEx(outhnd, true, &fontStructure);

        SetConsoleWindowInfo(outhnd, TRUE, &rect_win);
        SetConsoleScreenBufferSize(outhnd, buffersize);

        //SetConsoleMode(inhnd, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    }

    const void drawRectangle(float x, float y, int b_width, int b_height, float angle = 0, short col = whiteF)
    {
        float tempx;
        float tempy;
        for (float i = 0; i <= b_height; i += 0.1)
        {
            for (float j = 0; j <= b_width; j += 0.1)
            {
                tempx = ((x + j) * cos(angle) - (y + i) * sin(angle));
                tempy = ((x + j) * sin(angle) + (y + i) * cos(angle));

                Plot(tempx, tempy, col);
            }
        }
    }
    const void drawCircle(float xc, float yc, float r, short bordercol = whiteF, bool filled = 0, short innerCol = whiteF)
    {

        float x = -1;
        float y = r;
        if (filled)
        {
            for (float yn = -r; yn < r; yn += 0.1)
                for (float xn = -r; xn < r; xn += 0.1)
                    if (xn * xn + yn * yn < r * r)
                        Plot(xc + xn, yc + yn, innerCol);
        }

        Plot(x + xc, y + yc, bordercol);
        float pk = 3 - 2 * r;
        while (y > x)
        {

            if (pk < 0)
            {
                pk += 4 * x + 6;
                x += 1;
                std::vector<std::pair<float, float>> coords1{{x, y}, {y, x}, {y, -x}, {x, -y}, {-x, -y}, {-y, -x}, {-y, x}, {-x, y}};
                for (auto i : coords1)
                    Plot(i.first + xc, i.second + yc, bordercol);
            }
            else
            {
                pk += 4 * (x - y) + 10;
                x = x + 1;
                y -= 1;
                std::vector<std::pair<float, float>> coords2{{x, y}, {y, x}, {y, -x}, {x, -y}, {-x, -y}, {-y, -x}, {-y, x}, {-x, y}};
                for (auto i : coords2)
                    Plot(i.first + xc, i.second + yc, bordercol);
            }
        }
    }
};
