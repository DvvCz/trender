#include "game.hpp"

#include <print>

#include "plugins/debug-cam-controller/debug-cam-controller.hpp"
#include "plugins/physics/physics.hpp"

#include "util/error.hpp"

#include "scenes/test.hpp"
#include "scenes/nfs.hpp"

int main() {
  auto game = std::make_unique<Game>();
  game->addPlugin(DefaultPlugins());
  game->addPlugin(plugins::Physics());
  game->addPlugin(plugins::DebugCamController());
  game->addSystem(Schedule::Startup, scenes::test::startup);
  // game->addPlugin(scenes::nfs::NFS());

  auto result = game->start();
  if (!result.has_value()) {
    std::println(stderr, "Failed to start game:\n\t{}", util::error::indent(result.error()));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
