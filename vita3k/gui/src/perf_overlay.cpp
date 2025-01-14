// Vita3K emulator project
// Copyright (C) 2024 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include "private.h"

#include <config/state.h>

namespace gui {
static const ImVec2 PERF_OVERLAY_PAD = ImVec2(2.f, 2.f);
static const ImVec4 PERF_OVERLAY_BG_COLOR = ImVec4(0.282f, 0.239f, 0.545f, 0.8f);

static ImVec2 get_perf_pos(ImVec2 window_size, EmuEnvState &emuenv, ImVec2 scale) {
    const auto TOP = PERF_OVERLAY_PAD.y * scale.y;
    const auto LEFT = PERF_OVERLAY_PAD.x * scale.x;
    const auto CENTER = ImGui::GetIO().DisplaySize.x / 2.0 - (window_size.x / 2.f);
    const auto RIGHT = ImGui::GetIO().DisplaySize.x - window_size.x + PERF_OVERLAY_PAD.x * scale.x;
    const auto BOTTOM = ImGui::GetIO().DisplaySize.y - window_size.y + PERF_OVERLAY_PAD.y * scale.y;

    switch (emuenv.cfg.performance_overlay_position) {
    case TOP_CENTER: return ImVec2(CENTER, TOP);
    case TOP_RIGHT: return ImVec2(RIGHT, TOP);
    case BOTTOM_LEFT: return ImVec2(LEFT, BOTTOM);
    case BOTTOM_CENTER: return ImVec2(CENTER, BOTTOM);
    case BOTTOM_RIGHT: return ImVec2(RIGHT, BOTTOM);
    case TOP_LEFT:
    default: break;
    }

    return ImVec2(LEFT, TOP);
}

void draw_perf_overlay(GuiState &gui, EmuEnvState &emuenv) {
    auto lang = gui.lang.performance_overlay;

    const auto VIEWPORT_SIZE = ImVec2(emuenv.viewport_size.x, emuenv.viewport_size.y);
    const auto RES_SCALE = ImVec2(VIEWPORT_SIZE.x / emuenv.res_width_dpi_scale, VIEWPORT_SIZE.y / emuenv.res_height_dpi_scale);

    const auto SCALE = ImVec2(RES_SCALE.x * emuenv.dpi_scale, RES_SCALE.y * emuenv.dpi_scale);
    const auto SCALED_FONT_SIZE = ImGui::GetFontSize() * (0.7f * RES_SCALE.y);
    const auto FONT_SCALE = SCALED_FONT_SIZE / ImGui::GetFontSize();

    const auto FPS_TEXT = emuenv.cfg.performance_overlay_detail == MINIMUM ? fmt::format("FPS: {}", emuenv.fps) : fmt::format("FPS: {} {}: {}", emuenv.fps, lang["avg"], emuenv.avg_fps);
    const auto MIN_MAX_FPS_TEXT = fmt::format("{}: {} {}: {}", lang["min"], emuenv.min_fps, lang["max"], emuenv.max_fps);
    const auto TOTAL_WINDOW_PADDING = ImVec2(ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetStyle().WindowPadding.y * 2);
    const auto MAX_TEXT_WIDTH_SCALED = std::max(ImGui::CalcTextSize(FPS_TEXT.c_str()).x, emuenv.cfg.performance_overlay_detail == MINIMUM ? 0.f : ImGui::CalcTextSize(MIN_MAX_FPS_TEXT.c_str()).x) * FONT_SCALE;
    const auto MAX_TEXT_HEIGHT_SCALED = SCALED_FONT_SIZE + (emuenv.cfg.performance_overlay_detail >= MEDIUM ? SCALED_FONT_SIZE + (ImGui::GetStyle().ItemSpacing.y * 2.f) : 0.f);
    const auto WINDOW_SIZE = ImVec2(MAX_TEXT_WIDTH_SCALED + TOTAL_WINDOW_PADDING.x, MAX_TEXT_HEIGHT_SCALED + TOTAL_WINDOW_PADDING.y);
    const auto MAIN_WINDOW_SIZE = ImVec2(WINDOW_SIZE.x + TOTAL_WINDOW_PADDING.x, WINDOW_SIZE.y + TOTAL_WINDOW_PADDING.y + (emuenv.cfg.performance_overlay_detail == MAXIMUM ? WINDOW_SIZE.y : 0.f));
    const auto WINDOW_POS = get_perf_pos(MAIN_WINDOW_SIZE, emuenv, SCALE);
    
    ImGui::SetNextWindowSize(MAIN_WINDOW_SIZE);
    ImGui::SetNextWindowPos(WINDOW_POS);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::Begin("##performance", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, PERF_OVERLAY_BG_COLOR);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.f * SCALE.x);
    ImGui::BeginChild("#perf_stats", WINDOW_SIZE, ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowFontScale(0.7f * RES_SCALE.y);
    ImGui::Text("%s", FPS_TEXT.c_str());
    if (emuenv.cfg.performance_overlay_detail >= PerformanceOverlayDetail::MEDIUM) {
        ImGui::Separator();
        ImGui::Text("%s", MIN_MAX_FPS_TEXT.c_str());
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    if (emuenv.cfg.performance_overlay_detail == PerformanceOverlayDetail::MAXIMUM) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
        ImGui::PlotLines("##fps_graphic", emuenv.fps_values, IM_ARRAYSIZE(emuenv.fps_values), emuenv.current_fps_offset, nullptr, 0.f, float(emuenv.max_fps), WINDOW_SIZE);
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace gui
