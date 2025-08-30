#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "render.hpp"

void WindowClass::Draw(std::string_view label)
{
    constexpr static auto window_flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    constexpr static auto window_size = ImVec2(1280.0F, 720.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    //initialize window & begin
    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);
    ImGui::Begin(label.data(), nullptr, window_flags);

    DrawMenu();
    ImGui::Separator();
    DrawContent();
    ImGui::Separator();
    DrawActions();
    ImGui::Separator();
    DrawFilter();

    ImGui::End();
}

void WindowClass::DrawMenu()
{
    if (ImGui::Button("Go Up"))
    {
        if(currentPath.has_parent_path())
        {
            currentPath = currentPath.parent_path();
        }
    }
    ImGui::SameLine();

    ImGui::Text("Current directory: %s", currentPath.string().c_str()); //returns correct pointer type
}

void WindowClass::DrawContent()
{
    /**
     * @brief Construct a new for object
     * For each filesystem directory entry in the directory at currentPath,
     * give me a read-only reference named entry. I don’t need to know or write
     * out the full type, just let the compiler deduce it.
     *
     * @param fs::directory_iterator
     */
    // creates an iterator to go through file directory like i++ loop, or foreach
    //for (const std::filesystem::directory_entry &entry : fs::directory_iterator(currentPath))
    for (const auto &entry : fs::directory_iterator(currentPath))  // & is reference, read-only
    {
        const auto is_directory = entry.is_directory();
        const auto is_file = entry.is_regular_file();

        auto entry_name = entry.path().filename().string();
        if(is_directory)
            entry_name = "[D] " + entry_name;
        else if(is_file)
            entry_name = "[F] " + entry_name;

        ImGui::Text("%s", entry_name.c_str());
    }
}

void WindowClass::DrawActions()
{
    ImGui::Text(" Draw Actions");
}

void WindowClass::DrawFilter()
{
    ImGui::Text(" Draw Filter");
}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
