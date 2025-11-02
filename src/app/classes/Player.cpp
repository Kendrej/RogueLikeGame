#include "Player.h"
#include <imgui.h>
#include <cmath>
#include "../../engine/gfx/Assets.h"

Player::Player(int spriteId, uint32_t width, uint32_t height, float pos_x, float pos_y, float speed)
	: Entity(spriteId, width, height, pos_x, pos_y), speed_(speed) {}

Player* Player::spawn(Assets* assets, const char* path, uint32_t width, uint32_t height, float posX, float posY)
{
	int spriteId = assets->getOrLoad(path);
	return new Player(spriteId, width, height, posX, posY);
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

		ImVec2 pos = getPosition();
		float newX = pos.x + dx;
		float newY = pos.y + dy;
		const float maxX = io.DisplaySize.x - static_cast<float>(getWidth());
		const float maxY = io.DisplaySize.y - static_cast<float>(getHeight());
		if (newX < 0.0f) dx = -pos.x; else if (newX > maxX) dx = maxX - pos.x;
		if (newY < 0.0f) dy = -pos.y; else if (newY > maxY) dy = maxY - pos.y;

		moveBy(dx, dy);
	}
}
