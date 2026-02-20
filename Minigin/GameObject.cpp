#include <string>
#include "GameObject.h"

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update(float deltaTime)
{
	for (auto& component : m_Components)
	{
		component->Update(deltaTime);
	}

	std::erase_if(m_Components, [](const std::unique_ptr<Component>& component){ return component->IsMarkedForDestroy(); });
}

void dae::GameObject::Render() const
{
	for (const auto& component : m_Components)
	{
		component->Render();
	}
}

// W02
void dae::GameObject::SetLocalPosition(float x, float y)
{
	m_localTransform.SetPosition(x, y, 0.0f);
	SetPositionDirty();
}

const dae::Transform& dae::GameObject::GetTransform()
{
	if (m_positionIsDirty)
	{
		UpdateWorldTransform();
	}
	return m_worldTransform;
}

void dae::GameObject::SetPositionDirty()
{
	m_positionIsDirty = true;
	// If position changes, all children's positions change
	for (auto child : m_pChildren)
	{
		child->SetPositionDirty();
	}
}

void dae::GameObject::UpdateWorldTransform()
{
	if (m_positionIsDirty)
	{
		if (m_pParent == nullptr)
		{
			m_worldTransform = m_localTransform;
		}
		else
		{
			const auto& parentPos = m_pParent->GetTransform().GetPosition();
			const auto& localPos = m_localTransform.GetPosition();

			m_worldTransform.SetPosition(parentPos.x + localPos.x, parentPos.y + localPos.y, 0.0f);
		}
	}
	m_positionIsDirty = false;
}

void dae::GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
{
	if (IsChild(parent) || parent == this || m_pParent == parent)
	{
		return;
	}

	if (parent == nullptr)
	{
		SetLocalPosition(GetTransform().GetPosition().x, GetTransform().GetPosition().y);
	}
	else
	{
		if (keepWorldPosition)
		{
			SetLocalPosition(GetTransform().GetPosition().x - parent->GetTransform().GetPosition().x,
				GetTransform().GetPosition().y - parent->GetTransform().GetPosition().y);
		}
		SetPositionDirty();
	}

	if (m_pParent)
	{
		m_pParent->RemoveChild(this);
	}
	m_pParent = parent;
	if (m_pParent)
	{
		m_pParent->AddChild(this);
	}
}

void dae::GameObject::AddChild(GameObject* child)
{
	if (std::ranges::find(m_pChildren, child) == m_pChildren.end())
	{
		m_pChildren.push_back(child);
	}
}

void dae::GameObject::RemoveChild(GameObject* child)
{
	std::erase(m_pChildren, child);
}

bool dae::GameObject::IsChild(GameObject* child) const
{
	for (const auto& c : m_pChildren)
	{
		if (c == child || c->IsChild(child))
		{
			return true;
		}
	}
	return false;
}