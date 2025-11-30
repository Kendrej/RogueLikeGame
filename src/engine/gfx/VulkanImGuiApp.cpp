#include "VulkanImGuiApp.h"

#include "game/entities/Entity.h"
#include "game/entities/LivingEntity.h"
#include "game/entities/animation/AnimationController.h"
#include "app/GameSetup.h"
#include "game/world/Map.h"
#include "game/entities/Player.h"
#include "game/world/World.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vk_utils.h>

int VulkanImGuiApp::run()
{
    try
    {
        initWindow();
        initVulkan();
        initImGui();
        world_ = std::make_unique<World>(assets_.get());
        setupGame(*world_);
        mainLoop();
        vkDeviceWaitIdle(device_);
        cleanup();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal: " << e.what() << std::endl;
        cleanup();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int VulkanImGuiApp::runSmokeTest()
{
    try
    {
        std::cout << "[Smoke] Init window..." << std::endl;
        initWindow();
        std::cout << "[Smoke] Init Vulkan..." << std::endl;
        initVulkan();
        std::cout << "[Smoke] Init ImGui..." << std::endl;
        initImGui();
        vkDeviceWaitIdle(device_);
        std::cout << "[Smoke] OK" << std::endl;
        cleanup();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Smoke] Failed: " << e.what() << std::endl;
        cleanup();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void VulkanImGuiApp::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface");
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createRenderPass();
    createFramebuffers();
    createCommandPoolAndBuffers();
    createSyncObjects();
    createDescriptorPoolForImGui();
    // tymczasowo tu zeby bylo widac ale kiedys do refaktoryzaji
    Assets::Ctx actx{physicalDevice_, device_, graphicsQueue_, commandPool_};
    assets_ = std::make_unique<Assets>(actx);
}

static void drawDebugOverlay(GLFWwindow* window, const std::vector<std::unique_ptr<Entity>>& entityList,
                             const Player* mainPlayer)
{
    // Zbierz dane
    int winW = 0, winH = 0, fbW = 0, fbH = 0;
    glfwGetWindowSize(window, &winW, &winH);
    glfwGetFramebufferSize(window, &fbW, &fbH);

    auto& io = ImGui::GetIO();

    // Ustawienia okienka (przyklejone w lewym górnym rogu, półprzezroczyste)
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                             ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Debug overlay", nullptr, flags))
    {
        ImGui::Text("FPS: %.1f  (dt=%.4f s)", io.Framerate, io.DeltaTime);
        ImGui::Separator();

        ImGui::Text("GLFW Window:      %d x %d", winW, winH);
        ImGui::Text("GLFW Framebuffer: %d x %d", fbW, fbH);
        ImGui::Text("ImGui DisplaySize: %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
        ImGui::Text("ImGui FB Scale:   %.2f x %.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

        ImGui::Separator();
        ImGui::Text("Entities: %d", (int)entityList.size());
        if (ImGui::TreeNode("List"))
        {
            for (size_t i = 0; i < entityList.size(); ++i)
            {
                if (!entityList[i])
                    continue;
                ImVec2 p      = entityList[i]->getPosition();
                auto*  living = dynamic_cast<LivingEntity*>(entityList[i].get());
                if (living)
                {
                    ImGui::BulletText("#%zu pos=(%.0f, %.0f) size=%ux%u id=%d hp=%d/%d", i, p.x, p.y,
                                      entityList[i]->getWidth(), entityList[i]->getHeight(),
                                      entityList[i]->getEntityId(), living->getHp(), living->getMaxHp());
                }
                else
                {
                    ImGui::BulletText("#%zu pos=(%.0f, %.0f) size=%ux%u id=%d", i, p.x, p.y, entityList[i]->getWidth(),
                                      entityList[i]->getHeight(), entityList[i]->getEntityId());
                }
            }
            ImGui::TreePop();
        }

        if (mainPlayer)
        {
            ImGui::Separator();
            ImVec2 pp = mainPlayer->getPosition();
            ImGui::Text("Player: pos=(%.0f, %.0f) size=%ux%u entityId=%d", pp.x, pp.y, mainPlayer->getWidth(),
                        mainPlayer->getHeight(), mainPlayer->getEntityId());
            auto* lp = static_cast<const LivingEntity*>(mainPlayer);
            ImGui::Text("Player HP: %d / %d", lp->getHp(), lp->getMaxHp());
        }
    }
    ImGui::End();
}

void VulkanImGuiApp::mainLoop()
{
    while (!glfwWindowShouldClose(window_))
    {
        glfwPollEvents();

        // Sprawdz ESC najpierw, zanim ruszymy sync (unikanie potencjalnego czekania na fence po wyjściu)
        if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
            break; // wyjdz od razu
        }

        VulkanImGuiApp::FrameSync& fs = frames_[currentFrame_];
        vkWaitForFences(device_, 1, &fs.inFlight, VK_TRUE, UINT64_MAX);
        vkResetFences(device_, 1, &fs.inFlight);

        uint32_t imageIndex = 0;
        VkResult acq =
            vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, fs.imageAvailable, VK_NULL_HANDLE, &imageIndex);
        if (acq == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapchain();
            continue;
        }
        else if (acq != VK_SUCCESS && acq != VK_SUBOPTIMAL_KHR)
        {
            std::cerr << "Failed to acquire swapchain image: " << acq << std::endl;
            break;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float dt = ImGui::GetIO().DeltaTime;

        // Update screen bounds for world
        if (world_)
        {
            auto& io = ImGui::GetIO();
            world_->setScreenBounds(io.DisplaySize.x, io.DisplaySize.y);
        }

        // --- Proste sterowanie WASD oparte o GLFW (nie zależne od stanu przechwycenia klawiatury przez ImGui) ---
        if (auto* player = world_ ? world_->getPlayer() : nullptr)
        {
            float dx = 0.0f, dy = 0.0f;
            if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
                dy -= 1.0f;
            if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
                dy += 1.0f;
            if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
                dx -= 1.0f;
            if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
                dx += 1.0f;
            player->applyInput(ImVec2(dx, dy));

            // change attack mode
            static bool lastQ    = false;
            bool        qPressed = glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS;
            if (qPressed && !lastQ)
            {
                player->toggleAttackMode();
            }
            lastQ = qPressed;

            // player attack
            if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                if (player->isMeleeMode())
                {
                    if (player->canMelee())
                    {
                        if (!player->getAnimationController())
                        {
                            world_->performMeleeAttack(*player);
                        }
                        else
                        {
                            player->setIsPerformingMeleeAttack(true);
                        }
                        player->startMeleeCooldown();
                    }
                }
                else if (player->isRangedMode())
                {
                    if (player->canShoot())
                    {
                        if (!player->getAnimationController())
                        {
                            world_->performRangedAttack(*player);
                        }
                        else
                        {
                            player->setIsPerformingRangedAttack(true);
                        }
                        player->startRangedCooldown();
                    }
                }
            }
        }

        if (world_)
            world_->update(dt);

        // --- Rysowanie swiata/tla (poza oknami) ---
        drawWorld();

        // debug window
        if (world_)
            drawDebugOverlay(window_, world_->entities(), world_->getPlayer());

        ImGui::Render();

        VkCommandBuffer cmd = commandBuffers_[imageIndex];
        vkResetCommandBuffer(cmd, 0);
        recordCommandBuffer(cmd, imageIndex);

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo         si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        si.waitSemaphoreCount   = 1;
        si.pWaitSemaphores      = &fs.imageAvailable;
        si.pWaitDstStageMask    = waitStages;
        si.commandBufferCount   = 1;
        si.pCommandBuffers      = &cmd;
        si.signalSemaphoreCount = 1;
        si.pSignalSemaphores    = &fs.renderFinished;
        vkutils::checkVk(vkQueueSubmit(graphicsQueue_, 1, &si, fs.inFlight), "vkQueueSubmit failed");

        VkPresentInfoKHR pi{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        pi.waitSemaphoreCount = 1;
        pi.pWaitSemaphores    = &fs.renderFinished;
        pi.swapchainCount     = 1;
        pi.pSwapchains        = &swapchain_;
        pi.pImageIndices      = &imageIndex;
        VkResult pres         = vkQueuePresentKHR(presentQueue_, &pi);
        if (pres == VK_ERROR_OUT_OF_DATE_KHR || pres == VK_SUBOPTIMAL_KHR)
        {
            recreateSwapchain();
        }
        else if (pres != VK_SUCCESS)
        {
            std::cerr << "vkQueuePresentKHR failed: " << pres << std::endl;
            break;
        }

        currentFrame_ = (currentFrame_ + 1) % static_cast<uint32_t>(frames_.size());
    }
}

void VulkanImGuiApp::cleanup()
{
    if (device_)
        vkDeviceWaitIdle(device_);

    world_.reset();
    // ImGui
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    assets_.reset();
    // Vulkan sync
    for (auto& f : frames_)
    {
        if (f.imageAvailable)
            vkDestroySemaphore(device_, f.imageAvailable, nullptr);
        if (f.renderFinished)
            vkDestroySemaphore(device_, f.renderFinished, nullptr);
        if (f.inFlight)
            vkDestroyFence(device_, f.inFlight, nullptr);
    }

    if (imguiDescriptorPool_)
        vkDestroyDescriptorPool(device_, imguiDescriptorPool_, nullptr);

    if (commandPool_)
        vkDestroyCommandPool(device_, commandPool_, nullptr);

    cleanupSwapchain();

    if (renderPass_)
        vkDestroyRenderPass(device_, renderPass_, nullptr);

    if (device_)
        vkDestroyDevice(device_, nullptr);
    if (surface_)
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
    destroyDebugMessenger();
    if (instance_)
        vkDestroyInstance(instance_, nullptr);

    if (window_)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void VulkanImGuiApp::drawWorld()
{
    if (!assets_ || !world_)
        return;

    ImDrawList* bg = ImGui::GetBackgroundDrawList();

    // Draw black bar at top for UI
    bg->AddRectFilled(ImVec2(0.0f, 0.0f), ImVec2(ImGui::GetIO().DisplaySize.x, World::UI_TOP_BAR_HEIGHT),
                      IM_COL32(0, 0, 0, 255));

    auto drawHpBar = [bg](const LivingEntity& le, float x, float y, float w)
    {
        int hp    = le.getHp();
        int maxHp = le.getMaxHp();
        if (maxHp <= 0)
            return;
        float ratio           = static_cast<float>(hp) / static_cast<float>(maxHp);
        ratio                 = std::clamp(ratio, 0.0f, 1.0f);
        const float barHeight = 6.0f;
        ImVec2      barMin(x, y - barHeight - 4.0f);
        ImVec2      barMax(x + w, y - 4.0f);
        bg->AddRectFilled(barMin, barMax, IM_COL32(30, 30, 30, 200), 2.0f);
        bg->AddRect(barMin, barMax, IM_COL32(200, 200, 200, 255), 2.0f);
        ImVec2 hpFill(barMin.x + (barMax.x - barMin.x) * ratio, barMax.y);
        ImU32  col = (ratio > 0.5f) ? IM_COL32(0, 200, 0, 220) : IM_COL32(200, 50, 0, 220);
        bg->AddRectFilled(ImVec2(barMin.x + 1, barMin.y + 1), ImVec2(hpFill.x - 1, barMax.y - 1), col, 2.0f);
        // Tekst HP (środek paska)
        char buf[32];
        snprintf(buf, sizeof(buf), "%d/%d", hp, maxHp);
        ImVec2 textSize = ImGui::CalcTextSize(buf);
        ImVec2 textPos(barMin.x + (barMax.x - barMin.x - textSize.x) * 0.5f, barMin.y - textSize.y - 1.0f);
        bg->AddText(textPos, IM_COL32(255, 255, 255, 220), buf);
    };

    // Helper function to draw scaled sprite centered on position
    auto drawScaledSprite = [bg, this](int iconId, ImVec2 pos, float width, float height, float scale)
    {
        const auto& entity = assets_->icon(iconId);

        const float renderW = width * scale;
        const float renderH = height * scale;

        // Center the larger sprite on the hitbox
        const float offsetX = (renderW - width) * 0.5f;
        const float offsetY = (renderH - height) * 0.5f;

        ImVec2 renderPos(pos.x - offsetX, pos.y - offsetY);

        bg->AddImage(entity.imTex, renderPos, ImVec2(renderPos.x + renderW, renderPos.y + renderH), ImVec2(0, 0),
                     ImVec2(1, 1), IM_COL32_WHITE);
    };

    for (const auto& up : world_->entities())
    {
        if (!up)
            continue;
        if (up.get() == world_->getPlayer())
            continue;
        const Entity& e = *up;
        if (!e.isVisible())
            continue;

        const ImVec2   pos = e.getPosition();
        const uint32_t w   = e.getWidth();
        const uint32_t h   = e.getHeight();
        int            iconId;
        float          ENTITY_SPRITE_SCALE = 1.0f;
        if (auto* living = dynamic_cast<LivingEntity*>(up.get()))
        {
            if (auto* animationController = living->getAnimationController())
            {
                ENTITY_SPRITE_SCALE = 5.0f;
                iconId              = animationController->getCurrentFrameIconId();
            }
            else
            {
                iconId = living->getEntityId();
            }
            drawHpBar(*living, pos.x, pos.y, static_cast<float>(w));
        }
        else
        {
            iconId = e.getEntityId();
        }
        drawScaledSprite(iconId, pos, static_cast<float>(w), static_cast<float>(h), ENTITY_SPRITE_SCALE);
    }

    // Rysuj gracza
    if (auto* player = world_->getPlayer())
    {
        const ImVec2   pos = player->getPosition();
        const uint32_t w   = player->getWidth();
        const uint32_t h   = player->getHeight();
        int            iconId;
        if (auto* animationController = player->getAnimationController())
        {
            iconId = animationController->getCurrentFrameIconId();
        }
        else
        {
            iconId = player->getEntityId();
        }

        // Draw player sprite with 5x scale
        const float PLAYER_SPRITE_SCALE = 5.0f;
        drawScaledSprite(iconId, pos, static_cast<float>(w), static_cast<float>(h), PLAYER_SPRITE_SCALE);
        drawHpBar(*player, pos.x, pos.y, static_cast<float>(w));
    }
}
