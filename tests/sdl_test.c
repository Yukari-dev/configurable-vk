#include "../configurable-vk.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return -1;
  }

  WindowConfig window_config = {
    .title = "configurable-vk",
    .width = 800,
    .height = 600
  };

  SDL_Window* window = NULL;
  if (!conf_CreateWindow(&window_config, &window)) {
    printf("Failed to create window: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
