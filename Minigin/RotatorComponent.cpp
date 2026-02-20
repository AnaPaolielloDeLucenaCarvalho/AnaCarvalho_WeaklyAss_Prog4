#include "RotatorComponent.h"

dae::RotatorComponent::RotatorComponent(GameObject* pOwner, float radius, float speed)
	: Component(pOwner)
	, m_radius(radius)
	, m_speed(speed)
{
}

void dae::RotatorComponent::Update(float deltaTime)
{
	m_angle += m_speed * deltaTime;

	float x = std::cos(m_angle) * m_radius;
	float y = std::sin(m_angle) * m_radius;

	GetOwner()->SetLocalPosition(x, y);
}