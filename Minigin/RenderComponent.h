#ifndef RENDERCOMPONENT_H // No #pragma once, use include guards (for tom's sake)
#define RENDERCOMPONENT_H

#include <string>
#include <memory>
#include "Component.h"

namespace dae
{
    class Texture2D;

    class RenderComponent final : public Component
    {
    public:
        RenderComponent(GameObject* pOwner);
        virtual ~RenderComponent() = default;

        void Update(float deltaTime) override;
        void Render() const override;

        void SetTexture(const std::string& filename);

    private:
        std::shared_ptr<Texture2D> m_texture;
    };
}
#endif // RENDERCOMPONENT_H