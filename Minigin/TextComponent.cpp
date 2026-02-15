#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextComponent.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"
#include "GameObject.h" 

namespace dae
{
    TextComponent::TextComponent(GameObject* pOwner, const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color)
        : Component(pOwner)
        , m_needsUpdate(true)
        , m_text(text)
        , m_font(std::move(font))
        , m_textTexture(nullptr)
        , m_color(color) // Save it here!
    {
    }

    void TextComponent::Update(float /*deltaTime*/)
    {
        if (m_needsUpdate)
        {
            const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), m_text.length(), m_color);
            if (surf == nullptr)
            {
                throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
            }

            auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
            if (texture == nullptr)
            {
                throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
            }

            SDL_DestroySurface(surf);
            m_textTexture = std::make_shared<Texture2D>(texture);

            m_needsUpdate = false;
        }
    }

    void TextComponent::Render() const
    {
        if (m_textTexture != nullptr)
        {
            const auto& pos = GetOwner()->GetTransform().GetPosition();
            Renderer::GetInstance().RenderTexture(*m_textTexture, pos.x, pos.y);
        }
    }

    void TextComponent::SetText(const std::string& text)
    {
        if (m_text != text)
        {
            m_text = text;
            m_needsUpdate = true;
        }
    }

    void TextComponent::SetColor(const SDL_Color& color)
    {
        m_color = color;
        m_needsUpdate = true;
    }
}