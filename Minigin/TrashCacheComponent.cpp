#include "TrashCacheComponent.h"
#include <imgui.h>
#include <chrono>
#include <numeric>
#include <algorithm>

namespace dae
{
    struct Transform
    {
        float matrix[16] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };
    };

    struct GameObject3D
    {
        Transform transform{};
        int ID{};
    };

    struct GameObject3DAlt
    {
        Transform* transform{};
        int ID{};
    };

    TrashCacheComponent::TrashCacheComponent(GameObject* pOwner) : Component(pOwner)
    {
        for (int step = 1; step <= 1024; step *= 2)
        {
            m_XData.push_back(static_cast<float>(step));
        }
    }

    void TrashCacheComponent::Update(float /*deltaTime*/)
    {
    }

    void TrashCacheComponent::Render() const
    {
        ImGui::Begin("Programming 4 Assignment");

        ImGui::InputInt("# samples", &m_Samples);
        if (m_Samples < 1) m_Samples = 1;

        // Exercise 1
        if (ImGui::Button("Trash the cache (Int)"))
        {
            RunExercise1();
        }
        if (!m_TimingsInt.empty())
        {
            float maxVal = *std::max_element(m_TimingsInt.begin(), m_TimingsInt.end());
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); // Orange
            ImGui::PlotLines("##IntPlot", m_TimingsInt.data(), static_cast<int>(m_TimingsInt.size()),
                0, "Exercise 1", 0.0f, maxVal * 1.1f, ImVec2(400, 150));
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        // Exercise 2
        if (ImGui::Button("Trash the cache with GameObject3D"))
        {
            RunExercise2();
        }
        if (!m_TimingsGO3D.empty())
        {
            float maxVal = *std::max_element(m_TimingsGO3D.begin(), m_TimingsGO3D.end());
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f)); // Blue
            ImGui::PlotLines("##GO3DPlot", m_TimingsGO3D.data(), static_cast<int>(m_TimingsGO3D.size()),
                0, "Exercise 2", 0.0f, maxVal * 1.1f, ImVec2(400, 150));
            ImGui::PopStyleColor();
        }

        if (ImGui::Button("Trash the cache with GameObject3DAlt"))
        {
            RunExercise2Alt();
        }
        if (!m_TimingsGO3DAlt.empty())
        {
            float maxVal = *std::max_element(m_TimingsGO3DAlt.begin(), m_TimingsGO3DAlt.end());
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.5f, 1.0f)); // Green
            ImGui::PlotLines("##GO3DAltPlot", m_TimingsGO3DAlt.data(), static_cast<int>(m_TimingsGO3DAlt.size()),
                0, "Exercise 2 Alt", 0.0f, maxVal * 1.1f, ImVec2(400, 150));
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        // COMBINED GRAPH
        if (!m_TimingsGO3D.empty() && !m_TimingsGO3DAlt.empty())
        {
            ImGui::Text("Combined:");

            float maxGO3D = *std::max_element(m_TimingsGO3D.begin(), m_TimingsGO3D.end());
            float maxGO3DAlt = *std::max_element(m_TimingsGO3DAlt.begin(), m_TimingsGO3DAlt.end());
            float globalMax = std::max(maxGO3D, maxGO3DAlt);

            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
            ImGui::PlotLines("##CombinedPlot1", m_TimingsGO3D.data(), static_cast<int>(m_TimingsGO3D.size()),
                0, "", 0.0f, globalMax * 1.1f, ImVec2(400, 150));
            ImGui::PopStyleColor();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 154);

            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.5f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
            ImGui::PlotLines("##CombinedPlot2", m_TimingsGO3DAlt.data(), static_cast<int>(m_TimingsGO3DAlt.size()),
                0, "", 0.0f, globalMax * 1.1f, ImVec2(400, 150));
            ImGui::PopStyleColor(2);
        }

        ImGui::End();
    }

    void TrashCacheComponent::RunExercise1() const
    {
        const size_t bufferSize = 1 << 26;
        int* arr = new int[bufferSize] {};
        m_TimingsInt.clear();

        for (int step = 1; step <= 1024; step *= 2)
        {
            std::vector<long long> currentTimings;
            for (int s = 0; s < m_Samples; ++s)
            {
                auto start = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bufferSize; i += step)
                {
                    arr[i] *= 2;
                }
                auto end = std::chrono::high_resolution_clock::now();
                currentTimings.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
            }

            std::sort(currentTimings.begin(), currentTimings.end());
            long long sum = 0;
            int count = 0;

            for (size_t i = 1; i < currentTimings.size() - 1; ++i)
            {
                sum += currentTimings[i];
                count++;
            }
            float avgMicroseconds = count > 0 ? static_cast<float>(sum) / count : static_cast<float>(currentTimings[0]);

            m_TimingsInt.push_back(avgMicroseconds / 1000.f);
        }

        delete[] arr;
    }

    void TrashCacheComponent::RunExercise2() const
    {
        const size_t bufferSize = 1 << 22;
        GameObject3D* arr = new GameObject3D[bufferSize]{};
        m_TimingsGO3D.clear();

        for (int step = 1; step <= 1024; step *= 2)
        {
            std::vector<long long> currentTimings;
            for (int s = 0; s < m_Samples; ++s)
            {
                auto start = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bufferSize; i += step)
                {
                    arr[i].ID *= 2;
                }
                auto end = std::chrono::high_resolution_clock::now();
                currentTimings.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
            }

            std::sort(currentTimings.begin(), currentTimings.end());
            long long sum = 0;
            int count = 0;
            for (size_t i = 1; i < currentTimings.size() - 1; ++i)
            {
                sum += currentTimings[i];
                count++;
            }
            float avgMicroseconds = count > 0 ? static_cast<float>(sum) / count : static_cast<float>(currentTimings[0]);
            m_TimingsGO3D.push_back(avgMicroseconds / 1000.f);
        }

        delete[] arr;
    }

    void TrashCacheComponent::RunExercise2Alt() const
    {
        const size_t bufferSize = 1 << 24;
        GameObject3DAlt* arr = new GameObject3DAlt[bufferSize]{};
        m_TimingsGO3DAlt.clear();

        for (int step = 1; step <= 1024; step *= 2)
        {
            std::vector<long long> currentTimings;
            for (int s = 0; s < m_Samples; ++s)
            {
                auto start = std::chrono::high_resolution_clock::now();
                for (size_t i = 0; i < bufferSize; i += step)
                {
                    arr[i].ID *= 2;
                }
                auto end = std::chrono::high_resolution_clock::now();
                currentTimings.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
            }

            std::sort(currentTimings.begin(), currentTimings.end());
            long long sum = 0;
            int count = 0;
            for (size_t i = 1; i < currentTimings.size() - 1; ++i)
            {
                sum += currentTimings[i];
                count++;
            }
            float avgMicroseconds = count > 0 ? static_cast<float>(sum) / count : static_cast<float>(currentTimings[0]);
            m_TimingsGO3DAlt.push_back(avgMicroseconds / 1000.f);
        }

        delete[] arr;
    }
}