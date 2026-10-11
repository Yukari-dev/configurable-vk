#include "../configurable-vk.h"

int main(){
  SDL_Init(SDL_INIT_VIDEO);
  InstanceConfig config;
  config.application_name = "configurable-vk";
  config.flags = CON_INSTANCE_ENGINE_PREFIX_FLAG | CON_INSTANCE_VALIDATION_LAYERS_FLAG;
  config.all_api_version = CON_API_VERSION_1_3;

  VkInstance instance;
  CONF_RESULT res = conf_CreateInstance(&config, &instance);

  if(!res){
    printf("Failed to instance\n");
    return -1;
  }

  vkDestroyInstance(instance, NULL);
  return 0;
}
