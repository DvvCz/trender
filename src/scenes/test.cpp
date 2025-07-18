#include "test.hpp"

#include <expected>
#include <memory>
#include <print>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "components/transform.hpp"
#include "components/model.hpp"
#include "components/light.hpp"
#include "components/material.hpp"

#include "asset/obj/obj.hpp"
#include "asset/gltf/gltf.hpp"
#include "asset/img/img.hpp"

#include "render/model/3d/icosphere.hpp"
#include "render/model/3d/cube.hpp"

#include "util/error.hpp"

std::expected<void, std::string> scenes::test::startup(std::shared_ptr<entt::registry> registry,
                                                       std::shared_ptr<Renderer> renderer) {
  std::println("Loading assets, this might take a while...");

  { // gltf fish
    auto asset = asset::loader::Gltf::tryFromFile("resources/BarramundiFish.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load fish asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, -0.25f, 0.1f));
    registry->emplace<components::Scale>(ent, glm::vec3(5.0f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // main light
    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 5.0f));
    registry->emplace<components::Light>(ent, glm::vec3(1.0f), 2.0f, 1000.0f);
  }

  auto greenMaterial = std::make_shared<asset::Material>();
  greenMaterial->ambient = glm::vec3(0.05f);
  greenMaterial->diffuse = glm::vec3(0.2f, 0.6f, 0.2f);
  greenMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  greenMaterial->shininess = 64.0f;
  greenMaterial->dissolve = 1.0f;

  auto blueMaterial = std::make_shared<asset::Material>();
  blueMaterial->ambient = glm::vec3(0.05f);
  blueMaterial->diffuse = glm::vec3(0.2f, 0.2f, 0.6f);
  blueMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  blueMaterial->shininess = 64.0f;
  blueMaterial->dissolve = 1.0f;

  auto metallicRedMaterial = std::make_shared<asset::Material>();
  metallicRedMaterial->ambient = glm::vec3(0.05f);
  metallicRedMaterial->diffuse = glm::vec3(0.8f, 0.1f, 0.1f);
  metallicRedMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  metallicRedMaterial->shininess = 128.0f; // Higher shininess for metallic look
  metallicRedMaterial->dissolve = 1.0f;

  { // bunny
    auto asset = asset::loader::Obj::tryFromFile("resources/Bunny.obj", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load bunny asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 2.0f, -0.325f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // blue cube
    auto model = std::make_shared<model::Cube>(glm::vec3(1.0f));

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 0.5f));
    registry->emplace<components::Rotation>(ent, glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, blueMaterial);
  }

  { // red sphere
    auto model = std::make_shared<model::Icosphere>(0.5f, 4);

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(3.0f, 0.0f, 0.5f));
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, metallicRedMaterial);
  }

  { // flight helmet
    auto asset = asset::loader::Gltf::tryFromFile("resources/FlightHelmet/FlightHelmet.gltf", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load helmet asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(-1.0f, 0.0f, 0.25f));
    registry->emplace<components::Scale>(ent, glm::vec3(0.5f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // dragon
    auto asset = asset::loader::Gltf::tryFromFile("resources/DragonAttenuation.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load dragon asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(6.3f, 1.75f, 0.35f));
    registry->emplace<components::Scale>(ent, glm::vec3(0.35f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // normal tangent test
    auto asset = asset::loader::Gltf::tryFromFile("resources/NormalTangentTest.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load normal tangent asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(-10.0f, 3.0f, 1.0f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // negative scale test
    auto asset = asset::loader::Gltf::tryFromFile("resources/NegativeScaleTest.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load negative scale asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(3.0f, 0.9f, 0.3f));
    registry->emplace<components::Scale>(ent, glm::vec3(0.3f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // texture transform test
    auto asset = asset::loader::Gltf::tryFromFile("resources/TextureTransformTest/TextureTransformTest.gltf",
                                                  *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load transform asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(-10.0f, -5.0f, 1.0f));
    registry->emplace<components::Model3D>(ent, model);
  }

  // { // skybox
  //   auto asset = asset::loader::Img::tryFromFile("resources/mountain.jpg", *renderer->textureManager3D);
  //   if (!asset.has_value()) {
  //     return std::unexpected{std::format("Failed to load skybox asset: {}", util::error::indent(asset.error()))};
  //   }

  //   auto material = std::make_shared<asset::Material>();
  //   material->ambient = glm::vec3(1.0f);
  //   material->diffuse = glm::vec3(0.0f);
  //   material->specular = glm::vec3(0.0f);
  //   material->shininess = 0.0f;
  //   material->diffuseTexture = asset->texture;

  //   auto model = std::make_shared<model::Sphere>(100.0f, 4);

  //   auto ent = registry->create();
  //   registry->emplace<components::Rotation>(ent, glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
  //   registry->emplace<components::Model3D>(ent, model);
  //   registry->emplace<components::Material3D>(ent, material);
  // }

  { // baseplate
    auto model = std::make_shared<model::Cube>(glm::vec3(1000.0f, 1000.0f, 0.01f));

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, -1.0f));
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, greenMaterial);
  }

  return {};
}
