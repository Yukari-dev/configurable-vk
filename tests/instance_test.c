#include "../configurable-vk.h"

int main(){
  SDL_Init(SDL_INIT_VIDEO);
  InstanceConfig config;
  config.application_name = "configurable-vk";
  config.flags = INSTANCE_ENGINE_PREFIX_FLAG | INSTANCE_VALIDATION_LAYERS_FLAG;
  config.all_api_version = CON_API_VERSION_1_3;

  VkInstance instance;
  VkDebugUtilsMessengerEXT messenger;
  CON_RESULT ins = conf_CreateInstance(&config, &instance);
  CON_RESULT msg = conf_CreateDebugMessenger(&config, instance, &messenger);

  if(ins != CON_SUCCESS){
    printf("Failed to create instance\n");
    SDL_Quit();
    return -1;
  }

  if(msg != CON_SUCCESS){
    printf("Failed to create debug messenger\n");
    SDL_Quit();
    return -1;
  }

  DestroyDebugUtilsMessengerEXT(instance, messenger, NULL);
  vkDestroyInstance(instance, NULL);
  SDL_Quit();
  return 0;
}
