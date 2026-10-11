#ifndef _CONFIGURABLE_VK_H_
#define _CONFIGURABLE_VK_H_
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stdbool.h>

#define CON_API_VERSION_1_0 VK_API_VERSION_1_0
#define CON_API_VERSION_1_1 VK_API_VERSION_1_1
#define CON_API_VERSION_1_2 VK_API_VERSION_1_2
#define CON_API_VERSION_1_3 VK_API_VERSION_1_3
#define CON_API_VERSION_1_4 VK_API_VERSION_1_4

#define CON_INSTANCE_ENGINE_PREFIX_FLAG 0b01
#define CON_INSTANCE_VALIDATION_LAYERS_FLAG 0b10

typedef bool CONF_RESULT;

typedef struct{
  const char *title;
  int16_t width;
  int16_t height;
} WindowConfig;

typedef struct{
  const char *application_name;
  const char *engine_name;
  uint32_t all_api_version;
  uint32_t flags;
} InstanceConfig;

static inline CONF_RESULT conf_CreateWindow(const WindowConfig* window_config, SDL_Window** out_window){
  if (!window_config || !out_window) {
    return false;
  }

  Uint64 flags = SDL_WINDOW_RESIZABLE;

  *out_window = SDL_CreateWindow( window_config->title, window_config->width, window_config->height, flags);

  return (*out_window != NULL);
}

static inline char **get_required_extensions(){
  uint32_t count = 0;
  const char* const *extensions = SDL_Vulkan_GetInstanceExtensions(&count);

  char **dest = (char**)malloc(count * sizeof(char *));

  for(uint32_t i = 0; i < count; i++){
    dest[i] = (char *)malloc(strlen(extensions[i]) + 1);
    if(!dest[i]){
      for(size_t j = 0; j < i; j++){
        free(dest[i]);
      }
      free(dest);
      return NULL;
    }
    strcpy(dest[i], extensions[i]);
  }
  return dest;
}

static inline CONF_RESULT conf_CreateInstance(const InstanceConfig* config, VkInstance* out_instance){
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = config->application_name;
  app_info.applicationVersion = config->all_api_version;
  app_info.engineVersion = config->all_api_version;
  app_info.apiVersion = config->all_api_version;

  char engine_name_buffer[64];
  if(config->flags & CON_INSTANCE_ENGINE_PREFIX_FLAG){
    snprintf(engine_name_buffer, sizeof(engine_name_buffer), "%s engine", config->application_name);
    app_info.pEngineName = engine_name_buffer;
  } else{
    app_info.pEngineName = config->engine_name;
  }

  VkInstanceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  info.pApplicationInfo = &app_info;

  const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
  };
  uint32_t layer_count = sizeof(validation_layers) / sizeof(validation_layers[0]);

  info.enabledLayerCount = layer_count;
  info.ppEnabledLayerNames = validation_layers;

  uint32_t sdl_count = 0;
  const char* const *sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_count);
  if(!sdl_extensions)
    return false;

  const char *extra_extensions[] = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
  };
  Uint32 extra_count = sizeof(extra_extensions) / sizeof(extra_extensions[0]);
  Uint32 total_count = sdl_count + extra_count;
  const char **all_extensions = (const char**)malloc(total_count * sizeof(const char*));
  if(!all_extensions)
    return false;
  memcpy(all_extensions, sdl_extensions, sdl_count * sizeof(const char *));

  for(Uint32 i = 0; i < extra_count; ++i){
    all_extensions[sdl_count + i] = extra_extensions[i];
  }

  info.enabledExtensionCount = total_count;
  info.ppEnabledExtensionNames = all_extensions;
  
  VkResult ok = vkCreateInstance(&info, NULL, out_instance);

  free(all_extensions);

  return ok == VK_SUCCESS;
}

#endif
