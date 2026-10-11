#include "../configurable-vk.h"

int main(){
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return -1;
  }

  WindowConfig window_config = {.title = "configurable-vk", .width = 800, .height = 600};
  SDL_Window* window = NULL;
  CON_RESULT window_result = conf_CreateWindow(&window_config, &window);
  if (window_result != CON_SUCCESS) {
    printf("Failed to create window: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  InstanceConfig config;
  config.application_name = "configurable-vk";
  config.flags = INSTANCE_ENGINE_PREFIX_FLAG | INSTANCE_VALIDATION_LAYERS_FLAG;
  config.all_api_version = CON_API_VERSION_1_3;
  VkInstance instance;
  CON_RESULT ins = conf_CreateInstance(&config, &instance);
  if(ins != CON_SUCCESS){
    printf("Failed to create instance\n");
    SDL_Quit();
    return -1;
  }

  VkDebugUtilsMessengerEXT messenger;
  CON_RESULT msg = conf_CreateDebugMessenger(&config, instance, &messenger);

  if(msg != CON_SUCCESS){
    printf("Failed to create debug messenger\n");
    SDL_Quit();
    return -1;
  }

  VkSurfaceKHR surface;
  SurfaceConfig surface_config = {
    .window = window,
    .instance = &instance
  };
  CON_RESULT surf = conf_CreateSurface(&surface_config, &surface);

  PhysicalDeviceConfig physical_device_config = {};
  physical_device_config.instance = &instance;
  physical_device_config.surface = &surface;
  VkPhysicalDevice physical_device;
  CON_RESULT p_dev = conf_PickPhysicalDevice(&physical_device_config, &physical_device);

  if(p_dev != CON_SUCCESS){
    printf("Failed to pick physical device\n");
    SDL_Quit();
    return -1;
  }

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physical_device, &properties);
  printf("%s\n", properties.deviceName);

  DestroyDebugUtilsMessengerEXT(instance, messenger, NULL);
  vkDestroySurfaceKHR(instance, surface, NULL);
  vkDestroyInstance(instance, NULL);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
