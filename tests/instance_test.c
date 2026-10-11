#include "../configurable-vk.h"

int main(){
  SDL_Init(SDL_INIT_VIDEO);
  InstanceConfig config;
  config.application_name = "configurable-vk";
  config.flags = CON_INSTANCE_ENGINE_PREFIX_FLAG | CON_INSTANCE_VALIDATION_LAYERS_FLAG;
  config.all_api_version = CON_API_VERSION_1_3;

  VkInstance instance;
  VkDebugUtilsMessengerEXT messenger;
  CONF_RESULT ins = conf_CreateInstance(&config, &instance);
  CONF_RESULT msg = conf_CreateDebugMessenger(&config, instance, &messenger);

  if(!ins){
    printf("Failed to create instance\n");
    return -1;
  }

  if(!msg){
    printf("Failed to create debug messenger\n");
    return -1;
  }

  DestroyDebugUtilsMessengerEXT(instance, messenger, NULL);
  vkDestroyInstance(instance, NULL);
  return 0;
}
