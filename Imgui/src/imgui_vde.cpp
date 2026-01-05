#include <imgui/imgui_vde.h>
#include "../../Engine/src/core/window_impl_glfw_vulkan.h"
#include "../../Engine/src/core/graphicscontext_impl_vulkan.h"
#include "../../Engine/src/core/gpu/descriptorpool_impl_vulkan.h"
#include "../../Engine/src/core/gpu/commandbuffer_impl_vulkan.h"

#include <imgui/fa6_glyphs.h>
#include <imgui/fa6_ttf.h>

void vde::imgui::Initialize(vde::core::Window& window, vde::core::GraphicsContext& context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.GetImpl().window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.ApiVersion          = VK_API_VERSION_1_3;
    init_info.Instance            = window.GetImpl().instance;
    init_info.PhysicalDevice      = window.GetImpl().physicalDevice;
    init_info.Device              = context.GetImpl().device;
    init_info.QueueFamily         = context.GetImpl().graphicsQueueFamily;
    init_info.Queue               = context.GetImpl().graphicsQueue;
    init_info.DescriptorPool      = context.GetImpl().mainDescriptorPool->GetImpl().pool;
    init_info.MinImageCount       = 2;
    init_info.ImageCount          = 2;
    init_info.MSAASamples         = VK_SAMPLE_COUNT_1_BIT;
    init_info.UseDynamicRendering = true;

    VkFormat backbufferFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

    init_info.PipelineRenderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1Ui32,
        .pColorAttachmentFormats = &backbufferFormat
    };
    ImGui_ImplVulkan_Init(&init_info);

    io.Fonts->AddFontDefault();

    ImFontConfig config;
    config.MergeMode                   = true;
    config.FontDataOwnedByAtlas        = false;
    config.GlyphMinAdvanceX            = 13.0f;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromMemoryTTF(fa6_ttf_bytes, fa6_ttf_size, 13.0f, &config, icon_ranges);
}

void vde::imgui::BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode, nullptr);
}

void vde::imgui::EndFrame(vde::core::GraphicsContext& context)
{
    ImGui::Render();
    ImGui::UpdatePlatformWindows();

    auto& cb = context.CommandPool().Acquire();
    if (auto encoder = cb.Record("[ImGui]", vde::core::gpu::ECommandBufferRecordType::OneTimeSubmit); encoder)
    {
        if (auto rendering = encoder->BeginRendering({ &context.Backbuffer() }, nullptr); rendering)
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb.GetImpl().cmd, nullptr);
        }
    }

    context.Submit(cb, { context.GetImpl().lastSubmittedCommandBuffer });
    context.CommandPool().Release(cb);
}

void vde::imgui::Shutdown()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
