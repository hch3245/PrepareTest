#pragma once
#include <string>
#include <iostream>
#include <format>
#include <vector>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

struct ConsoleLogEntry
{
    std::string Category; // 로그 카테고리 (예: "LogTemp")
    std::string Message;  // 로그 메시지
    int Verbosity;        // 로그 레벨 (0: Verbose, 1: Log, 2: Warning, 3: Error)
};

struct ExampleAppConsole {
    char                  InputBuf[256];
    // 로그 엔트리를 저장하는 벡터 (ImVector 대신 std::vector 사용 가능)
    std::vector<ConsoleLogEntry> Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       textFilter;
    ImGuiTextFilter       categoryFilter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("", 1, "Welcome to Dear ImGui!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            ImGui::MemFree(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        Items.clear();
    }

    // 개선된 AddLog 함수: 카테고리와 Verbosity를 추가 인자로 받습니다.
    void AddLog(const char* Category, int Verbosity, const char* fmt, ...) IM_FMTARGS(4)
    {
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);

        // 새로운 로그 엔트리 생성 및 Items 벡터에 추가
        ConsoleLogEntry entry;
        entry.Category = Category;
        entry.Verbosity = Verbosity;
        entry.Message = buf;
        Items.push_back(entry);
    }

    void    Draw(const char* title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Debug Text")) { AddLog("LogTemp", 0, "Verbosity 0 text", Items.size()); AddLog("LogTemp", 1, "Verbosity 1 text"); AddLog("LogTemp", 2, "Verbosity 2 Text!"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Add Debug Error")) {
            AddLog("LogTemp", 2, "something went wrong");
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear")) { ClearLog(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }
        // Options, Filter
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        textFilter.Draw("Text Filter", 180);
        ImGui::SameLine();
        categoryFilter.Draw("Category Filter", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear")) ClearLog();
                ImGui::EndPopup();
            }

            // Display every line as a separate entry so we can change their color or add custom widgets.
            // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
            // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
            // to only process visible items. The clipper will automatically measure the height of your first item and then
            // "seek" to display only items in the visible area.
            // To use the clipper we can replace your standard loop:
            //      for (int i = 0; i < Items.Size; i++)
            //   With:
            //      ImGuiListClipper clipper;
            //      clipper.Begin(Items.Size);
            //      while (clipper.Step())
            //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            // - That your items are evenly spaced (same height)
            // - That you have cheap random access to your elements (you can access them given their index,
            //   without processing all the ones before)
            // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
            // We would need random-access on the post-filtered list.
            // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
            // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
            // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
            // to improve this example code!
            // If your items are of variable height:
            // - Split them into same height items would be simpler and facilitate random-seeking into your list.
            // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            if (copy_to_clipboard)
                ImGui::LogToClipboard();
            for (const auto& entry : Items)
            {
                if (textFilter.IsActive() && categoryFilter.IsActive()) {
                    if (!textFilter.PassFilter(entry.Message.c_str()) && !categoryFilter.PassFilter(entry.Category.c_str()))
                        continue;
                }
                else if (textFilter.IsActive()) {
                    if (!textFilter.PassFilter(entry.Message.c_str()))
                        continue;
                }
                else if (categoryFilter.IsActive()) {
                    if (!categoryFilter.PassFilter(entry.Category.c_str()))
                        continue;
                }

                // Verbosity 값에 따라 색상 빨개짐
                if (entry.Verbosity <= 0)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
                else if (entry.Verbosity <= 1)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                else if (entry.Verbosity <= 2) // 2: Warning, 3: Error
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.647f, 0.0f, 1.0f));
                else
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));

                // Verbosity 변환
                std::string verbosityStr;

                switch (entry.Verbosity)
                {
                case 0: verbosityStr = ""; break;
                case 1: verbosityStr = "Log"; break;
                case 2: verbosityStr = "Warning"; break;
                case 3: verbosityStr = "Error"; break;
                case 4: verbosityStr = "Display"; break;
                case 5: verbosityStr = "Fatal"; break;
                case 6: verbosityStr = "Verbose"; break;
                case 7: verbosityStr = "VeryVerbose"; break;
                default: verbosityStr = "Unknown"; break;
                }
                std::string out = "";

                if (entry.Category != "") {
                    out += entry.Category + ": ";
                }

                if (entry.Verbosity != 0) {
                    out += entry.Verbosity + ": ";
                }

                // 최종 출력할 문자열 조합: "Category: Verbosity Message"
                out += entry.Message;
                ImGui::TextUnformatted(out.c_str());

                ImGui::PopStyleColor();
            }
            if (copy_to_clipboard)
                ImGui::LogFinish();
            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();


        // Command-line
        bool reclaim_focus = false;
        bool showRelatedWindow = false;
        // 후보 목록을 담을 ImVector 생성 (ImGui에서 사용하는 컨테이너)
        ImVector<const char*> suggestions;

        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;

            Strtrim(s);
            if (s[0]) {
                ExecCommand(s);
            }
            strcpy_s(s, IM_ARRAYSIZE(InputBuf), "");
            reclaim_focus = true;
        }
        else {
            // 사용자가 입력 중이라면 InputBuf가 비어있지 않을 것입니다.
            // 입력된 텍스트와 접두사가 일치하는 명령어 후보를 찾습니다.

            if (strlen(InputBuf) > 0)
            {
                for (int i = 0; i < Commands.Size; i++)
                {
                    // ImStrnicmp는 대소문자 구분 없이 문자열 비교합니다.
                    // InputBuf가 명령어의 앞부분과 일치하면 후보 목록에 추가합니다.
                    if (ImStrnicmp(InputBuf, Commands[i], (int)strlen(InputBuf)) == 0)
                    {
                        suggestions.push_back(Commands[i]);
                    }
                }
            }

            if (suggestions.Size > 0)
            {
                showRelatedWindow = true;

                // InputText 위에 창을 띄우기 위해 InputText의 위치를 가져옵니다.
                ImVec2 pos = ImGui::GetItemRectMin();

                float lineHeight = ImGui::GetTextLineHeight();
                float lineHeightWithSpacing = ImGui::GetTextLineHeightWithSpacing();
                float totalHeight = lineHeightWithSpacing * (suggestions.Size - 1) + lineHeight;

                pos.y -= totalHeight + 22;

                ImGui::SetNextWindowPos(pos);
                ImGui::BeginTooltip();
                for (int i = 0; i < suggestions.Size; i++)
                {
                    ImGui::Text("%s", suggestions[i]);
                }
                ImGui::EndTooltip();
            }
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("", 0, "# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                ImGui::MemFree(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("", 1, "Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("", 1, "- %s", Commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("", 1, "%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("", 2, "Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < Commands.Size; i++)
                if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0)
            {
                // No match
                AddLog("", 2, "No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
            }
            else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                // Multiple matches. Complete as much as we can..
                // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                AddLog("", 1, "Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("", 1, "- %s\n", candidates[i]);
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos)
            {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}