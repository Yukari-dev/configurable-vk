#ifndef _CONFIGURABLE_VK_H_
#define _CONFIGURABLE_VK_H_
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct{
  const char *title;
  int16_t width;
  int16_t height;
} WindowConfig;

static inline bool conf_CreateWindow(const WindowConfig* window_config, SDL_Window** out_window){
  if (!window_config || !out_window) {
    return false;
  }

  Uint64 flags = SDL_WINDOW_RESIZABLE;

  *out_window = SDL_CreateWindow( window_config->title, window_config->width, window_config->height, flags);

  return (*out_window != NULL);
}


#endif
