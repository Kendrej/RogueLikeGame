#include "Player.h"
#include <imgui.h>
#include <cmath>
#include "../../engine/gfx/Assets.h"

Player::Player(int spriteId, uint32_t width, uint32_t height, float pos_x, float pos_y, float speed)
	: Entity(spriteId, width, height, pos_x, pos_y), speed_(speed) {}

void Player::spawnPlayer(std::unique_ptr<Player>& outPlayer, Assets* assets, const char* path, uint32_t width, uint32_t height, float posX, float posY)
{
	int spriteId = assets->getOrLoad(path);
	outPlayer = std::make_unique<Player>(spriteId, width, height, posX, posY);
}

void Player::update(float dt)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;

	float dx = 0.0f;
	float dy = 0.0f;

	if (ImGui::IsKeyDown(ImGuiKey_W)) dy -= 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_S)) dy += 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_A)) dx -= 1.0f;
	if (ImGui::IsKeyDown(ImGuiKey_D)) dx += 1.0f;

	if (dx != 0.0f || dy != 0.0f) {
		float len = std::sqrt(dx * dx + dy * dy);
		dx = (dx / len) * speed_ * dt;
		dy = (dy / len) * speed_ * dt;

		ImVec2 curPos = getPosition();
		float newX = curPos.x + dx;
		float newY = curPos.y + dy;
		const float maxX = io.DisplaySize.x - static_cast<float>(getWidth());
		const float maxY = io.DisplaySize.y - static_cast<float>(getHeight());
		if (newX < 0.0f) dx = -curPos.x; else if (newX > maxX) dx = maxX - curPos.x;
		if (newY < 0.0f) dy = -curPos.y; else if (newY > maxY) dy = maxY - curPos.y;

		moveBy(dx, dy);
	}
}
