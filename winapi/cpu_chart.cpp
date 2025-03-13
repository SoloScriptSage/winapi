#include "imgui.h"
#include "implot.h"
#include <vector>
#include <thread>
#include <windows.h>
#include <iostream>

// Store CPU usage history (each core has its own history)
constexpr int MAX_HISTORY = 100;
std::vector<std::vector<float>> cpuUsageHistory;

void InitializeCPUHistory(int numCores) {
    cpuUsageHistory.resize(numCores, std::vector<float>(MAX_HISTORY, 0.0f));
}

// Function to update CPU usage history
void UpdateCPUUsageHistory(const std::vector<double>& cpuUsage) {
    for (size_t i = 0; i < cpuUsage.size(); i++) {
        cpuUsageHistory[i].erase(cpuUsageHistory[i].begin()); // Remove oldest value
        cpuUsageHistory[i].push_back(static_cast<float>(cpuUsage[i])); // Add new value
    }
}

// Function to display the ImGui window
void ShowCPUUsagePlot() {
    ImGui::Begin("CPU Usage Monitor");

    if (ImPlot::BeginPlot("CPU Usage Over Time")) {
        for (size_t i = 0; i < cpuUsageHistory.size(); i++) {
            std::string label = "Core " + std::to_string(i);
            ImPlot::PlotLine(label.c_str(), cpuUsageHistory[i].data(), MAX_HISTORY);
        }
        ImPlot::EndPlot();
    }

    ImGui::End();
}
