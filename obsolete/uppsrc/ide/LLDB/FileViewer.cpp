#include "LLDB.h"


Opt<int> FileViewer::Render()
{
	TODO; // what the hell is this?
    const Index<int>* const bps = 0;
        /*(m_breakpoints.GetCount() && m_breakpoints != m_breakpoint_cache.end())
            ? &m_breakpoints
            : nullptr;*/

    ImGuiContext& g = *GImGui;
    auto& style = g.Style;
    ImGuiWindow* window = g.CurrentWindow;

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style.Colors[ImGuiCol_TitleBg]);
    Defer(ImGui::PopStyleColor());
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, style.Colors[ImGuiCol_TitleBg]);
    Defer(ImGui::PopStyleColor());

    Opt<int> clicked_line = {};

    String line_buffer;
    for (size_t i = 0; i < m_lines.GetCount(); i++) {
        const size_t line_number = i + 1;

        line_buffer = Format("   %d  %s\n", (int)line_number, (String)m_lines[i]);

        bool selected = false;
        if (m_highlighted_line.has_value() &&
            line_number == static_cast<size_t>(*m_highlighted_line)) {
            selected = true;
            if (m_highlight_line_needs_focus) {
                //ImGui::SetScrollHere();
                m_highlight_line_needs_focus = false;
            }
        }

        ImGui::Selectable(line_buffer, selected);
        if (ImGui::IsItemClicked()) {
            clicked_line = line_number;
        }

        if (bps && bps->Find(line_number) >= 0) {
            ImVec2 pad = style.FramePadding;
            ImVec2 pos = window->DC.CursorPos;
            ImVec2 txt = ImGui::CalcTextSize("X");
            pos.x += 1.3f * txt.x;
            pos.y -= (txt.y + 2.f * pad.y) / 2.f;
            window->DrawList->AddCircleFilled(pos, txt.y / 3.f, IM_COL32(255, 0, 0, 255));
        }

        line_buffer.Clear();
    }

    return clicked_line;
}

void FileViewer::SynchronizeBreakpointCache(lldb::SBTarget target)
{
    for (auto& bps : m_breakpoint_cache) bps.Clear();

    for (int i = 0; i < target.GetNumBreakpoints(); i++) {
        lldb::SBBreakpoint bp = target.GetBreakpointAtIndex(i);
        lldb::SBBreakpointLocation location = bp.GetLocationAtIndex(0);

        if (!location.IsValid()) {
            LOG("error: Invalid breakpoint location encountered by LLDB.");
        }

        lldb::SBAddress address = location.GetAddress();

        if (!address.IsValid()) {
            LOG("error: Invalid lldb::SBAddress for breakpoint encountered.");
        }

        lldb::SBLineEntry line_entry = address.GetLineEntry();

        if (!line_entry.IsValid()) {
            LOG("error: Invalid lldb::SBLineEntry for breakpoint encountered.");
            continue;
        }

        // auto file_spec = line_entry.GetFileSpec();

        const String bp_filepath = Format("%s/%s",
			(String)line_entry.GetFileSpec().GetDirectory(),
			(String)line_entry.GetFileSpec().GetFilename());

        const auto maybe_handle = FileHandle::Create(bp_filepath);
        if (!maybe_handle) {
            LOG("error: Invalid filepath found for breakpoint: " << bp_filepath);
            continue;
        }
        const FileHandle handle = *maybe_handle;

        m_breakpoint_cache.GetAdd(handle).FindAdd(line_entry.GetLine());
    }
}

void FileViewer::Show(FileHandle handle)
{
    m_lines <<= handle.GetContents();

    if (int i = m_breakpoint_cache.Find(handle); i >= 0) {
        m_breakpoints = &m_breakpoint_cache[i];
    }
    else {
        m_breakpoints = {};
    }
}
