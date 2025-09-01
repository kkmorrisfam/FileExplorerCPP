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
    ImGui::SetCursorPosY(ImGui::GetWindowHeight()-100.0F);
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
    if (fs::is_directory(selectedEntry))
        ImGui::Text("Selected dir: %s", selectedEntry.string().c_str());
    else if (fs::is_regular_file(selectedEntry))
        ImGui::Text("Selected file: %s", selectedEntry.string().c_str());
    else
    {
        ImGui::Text("Nothing selected!");
    }

    ImGui::SameLine();

    if (fs::is_regular_file(selectedEntry) && ImGui::Button("Open"))
        openFileWithDefaultEditor();

    if (ImGui::Button("Rename"))
    {
        renameDialogOpen = true;
        ImGui::OpenPopup("Rename File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete"))
    {
        deleteDialogOpen = true;
        ImGui::OpenPopup("Delete File");
    }


    renameFilePopup();
    deleteFilePopup();
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

void WindowClass::openFileWithDefaultEditor()
{
    //from stackoverflow for different op systems
    #ifdef _WIN32
        const auto command = "start \"\" \"" + selectedEntry.string() + "\"";
    #elif __APPLE__
        const auto command = "open \"" + selectedEntry.string() + "\"";
    #else
        const auto command = "xdg-open \"" + selectedEntry.string() + "\"";
    #endif

        std::system(command.c_str());
}

void WindowClass::renameFilePopup()
{
    //static auto dialog_opened = false;
    //name must match what's passed when method called
    if (ImGui::BeginPopupModal("Rename File", &renameDialogOpen))
    {
        // variable to store the new file name in text box
        static char buffer_name[512] = {'\0'};

        ImGui::Text("New name: ");
        ImGui::InputText("###newName", buffer_name, sizeof(buffer_name));

        if (ImGui::Button("Rename"))
        {
            auto new_path = selectedEntry.parent_path() / buffer_name;
            if (renameFile(selectedEntry, new_path))
            {
                renameDialogOpen = false;
                selectedEntry = new_path;
                std::memset(buffer_name, 0, sizeof(buffer_name));
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            renameDialogOpen = false;

        ImGui::EndPopup();
    }

}

void WindowClass::deleteFilePopup()
{
    if (ImGui::BeginPopupModal("Delete File", &deleteDialogOpen))
    {
        //replace %s with string value of filename
        ImGui::Text("Are you sure you want to delete %s?",
            selectedEntry.filename().string().c_str());

        if (ImGui::Button("Yes"))
        {
            if (deleteFile(selectedEntry))
                selectedEntry.clear();
            deleteDialogOpen = false;    //reset flag
        }

        ImGui::SameLine();

        if (ImGui::Button("No"))
        {
            deleteDialogOpen = false;   //reset flag
        }

        ImGui::EndPopup();
    }
}


bool WindowClass::renameFile(const fs::path &old_path, const fs::path &new_path)
{
    try
    {
        fs::rename(old_path, new_path);
        return true;
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }

}

bool WindowClass::deleteFile(const fs::path &path)
{
    try
    {
        fs::remove(path);
        return true;
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
