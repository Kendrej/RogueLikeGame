#include "Player.h"
#include <imgui.h>
#include <cmath>
#include "../../engine/gfx/Assets.h"

Player::Player(int playerId, uint32_t width, uint32_t height, float pos_x, float pos_y)
	: LivingEntity(playerId, width, height, pos_x, pos_y){}

void Player::update(float dt)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;
	ImVec2 dir{0.0f, 0.0f};
	if (ImGui::IsKeyDown(ImGuiKey_W)) dir.y -= 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_S)) dir.y += 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_A)) dir.x -= 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_D)) dir.x += 1.0f;

	applyInput(dir);
	LivingEntity::update(dt);

}
