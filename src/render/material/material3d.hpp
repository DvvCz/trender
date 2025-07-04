#pragma once

#include <rapidobj/rapidobj.hpp>

#include "render/uniform/block.hpp"
#include "render/texture.hpp"

#include "asset/asset.hpp"

namespace material {
  // Carefully ensure this is std140
  struct Material3D {
    glm::vec3 ambient;
    float shininess;
    glm::vec3 diffuse;
    float dissolve;
    glm::vec3 specular;
    float emissiveStrength;
    glm::vec3 emissive;
    float _padding;

    texture::Texture diffuseTexture;
    texture::Texture normalTexture;
    texture::Texture emissiveTexture;
  };

  static_assert(sizeof(Material3D) % 16 == 0, "Ensure Material3D is std140 compliant");

  class Manager3D {
  public:
    Manager3D(uniform::Block<material::Material3D> uniformMaterial, std::shared_ptr<texture::Manager> texMan);
    ~Manager3D();

    void setMaterial(const asset::Material& material) noexcept;
    void setMaterial(const material::Material3D& material) noexcept;
    [[nodiscard]] material::Material3D getMaterial() const noexcept;

  private:
    std::shared_ptr<texture::Manager> textureManager;

    uniform::Block<material::Material3D> uniformMaterial;
    material::Material3D currentMaterial;
  };
};
