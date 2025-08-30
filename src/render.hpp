#pragma once

#include <cstdint>
#include <string_view>

class WindowClass
{
public:
    void Draw(std::string_view label);

private:
    void DrawMenu();
    void DrawContent();
    void DrawActions();
    void DrawFilter();
};

void render(WindowClass &window_obj);
