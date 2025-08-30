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
    for (const auto &entry : fs::directory_iterator(currentPath))
    {
        const auto is_selected = entry.path() == selectedEntry;
        const auto is_directory = entry.is_directory();
        const auto is_file = entry.is_regular_file();

        auto entry_name = entry.path().filename().string();
        if(is_directory)
            entry_name = "[D] " + entry_name;
        else if(is_file)
            entry_name = "[F] " + entry_name;

        //if entry selected = true
        if (ImGui::Selectable(entry_name.c_str(), is_selected))
        {
            if (is_directory)
                currentPath /= entry.path().filename(); //appends the subdirectory name

            // becomes full path just clicked
            selectedEntry = entry.path();
        }
    }
}

void WindowClass::DrawActions()
{
    ImGui::Text(" Draw Actions");
}

void WindowClass::DrawFilter()
{
    static char extention_filter[16] = {"\0"};

    ImGui::Text("Filter by extention");
    ImGui::SameLine();
    ImGui::InputText("###inFilter", extention_filter, sizeof(extention_filter)); //"###" makes it hidden label

    if (std::strlen(extention_filter) == 0)
        return;

    auto filtered_file_count = std::size_t{0};

    for (const auto &entry : fs::directory_iterator(currentPath))
    {
        if (!fs::is_regular_file(entry))  //if not file extention
            continue; //skip this iteration

        if (entry.path().extension().string() == extention_filter)
            ++filtered_file_count;
    }
}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
