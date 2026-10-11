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

typedef enum{
  INSTANCE_ENGINE_PREFIX_FLAG = 0b01,
  INSTANCE_VALIDATION_LAYERS_FLAG = 0b10
} InstanceFlags;

typedef bool CON_RESULT;

#define CON_SUCCESS true
#define CON_FAILED false

typedef struct{
  const char *title;
  int16_t width;
  int16_t height;
} WindowConfig;

typedef struct{
  SDL_Window* window;
  VkInstance* instance;
} SurfaceConfig;

typedef struct{
  const char *application_name;
  const char *engine_name;
  uint32_t all_api_version;
  uint32_t flags;
} InstanceConfig;

typedef struct{
  uint32_t graphics_family;
  uint32_t present_family;
} QueueFamilyIndex;

typedef struct{
  VkInstance* instance;
  VkSurfaceKHR* surface;
} PhysicalDeviceConfig;

static inline CON_RESULT conf_CreateWindow(const WindowConfig* window_config, SDL_Window** out_window){
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

static inline bool check_validation_layers(const char **validation_layers, uint32_t size){
  uint32_t layer_count = 0;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);
  VkLayerProperties *layers = (VkLayerProperties*)malloc(layer_count * sizeof(VkLayerProperties));
  vkEnumerateInstanceLayerProperties(&layer_count, layers);

  bool found = false;
  for(uint32_t i = 0; i < layer_count; i++){
    for(uint32_t j = 0; j < size; j++){
      if(strcmp(validation_layers[j], layers[i].layerName) == 0){
        found = true;
        break;
      }
    }
  }

  free(layers);
  return found;
}

static inline VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT *data,
  void *pUserData
){
  const char* sev = "?";
  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) sev = "ERROR";
  else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) sev = "WARN";
  else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) sev = "INFO";

  const char* type = "?";
  if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) type = "VALIDATION";
  else if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) type = "PERFORMANCE";
  else if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) type = "GENERAL";

  printf("[severity: %s, type: %s]: %s", sev, type, data->pMessage);
  return VK_FALSE;
}

static inline VkResult CreateDebugUtilsMessengerEXT(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
  const VkAllocationCallbacks *pAllocator,
  VkDebugUtilsMessengerEXT *pDebugMessenger
){
  PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if(func != NULL)
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  else
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static inline void DestroyDebugUtilsMessengerEXT(
  VkInstance instance, 
  VkDebugUtilsMessengerEXT debugMessnger,
  const VkAllocationCallbacks *pAllocator
){
  PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL)
    func(instance, debugMessnger, pAllocator);
}

static inline void populate_debug_messenger(VkDebugUtilsMessengerCreateInfoEXT* create_info){
  create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info->messageSeverity = 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info->messageType = 
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  create_info->pfnUserCallback = DebugCallback;
}

static inline CON_RESULT conf_CreateInstance(const InstanceConfig* config, VkInstance* out_instance){
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = config->application_name;
  app_info.applicationVersion = config->all_api_version;
  app_info.engineVersion = config->all_api_version;
  app_info.apiVersion = config->all_api_version;

  char engine_name_buffer[64];
  if(config->flags & INSTANCE_ENGINE_PREFIX_FLAG){
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

  VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
  if(config->flags & INSTANCE_VALIDATION_LAYERS_FLAG){
    info.enabledLayerCount = layer_count;
    info.ppEnabledLayerNames = validation_layers;
    populate_debug_messenger(&debug_info);
    info.pNext = &debug_info;
  }

  uint32_t sdl_count = 0;
  const char* const *sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_count);
  if(!sdl_extensions)
    return CON_FAILED;

  const char *extra_extensions[] = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
  };
  Uint32 extra_count = sizeof(extra_extensions) / sizeof(extra_extensions[0]);
  Uint32 total_count = sdl_count + extra_count;
  const char **all_extensions = (const char**)malloc(total_count * sizeof(const char*));
  if(!all_extensions)
    return CON_FAILED;
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

static inline CON_RESULT conf_CreateDebugMessenger(const InstanceConfig* config, VkInstance instance, VkDebugUtilsMessengerEXT* out_messenger){
  if(!(config->flags & INSTANCE_VALIDATION_LAYERS_FLAG)) return false;
  VkDebugUtilsMessengerCreateInfoEXT create_info = {};
  populate_debug_messenger(&create_info);
  VkResult result = CreateDebugUtilsMessengerEXT(instance, &create_info, NULL, out_messenger);
  return result == VK_SUCCESS;
}

static inline CON_RESULT conf_CreateSurface(SurfaceConfig* config, VkSurfaceKHR* out_surface){
  return SDL_Vulkan_CreateSurface(config->window, *config->instance, nullptr, out_surface);
}

static inline QueueFamilyIndex find_family_queues(VkPhysicalDevice* device, VkSurfaceKHR surface){
  QueueFamilyIndex indices = {.graphics_family = UINT32_MAX, .present_family = UINT32_MAX};

  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(*device, &count, NULL);
  VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(count * sizeof(VkQueueFamilyProperties));
  vkGetPhysicalDeviceQueueFamilyProperties(*device, &count, queues);

  int i = 0;
  for(uint32_t j = 0; j < count; ++j){
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, surface, &present_support);
    if(queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      indices.graphics_family = i;
    if(present_support)
      indices.present_family = i;
    if(indices.graphics_family != UINT32_MAX && indices.present_family != UINT32_MAX)
      break;
  }

  free(queues);
  return indices;
}

static inline bool check_physical_device_extensions(VkPhysicalDevice* device){
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(*device, NULL, &count, NULL);
  VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(count * sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(*device, NULL, &count, extensions);
  if(!extensions)
    return false;

  const char *required_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  uint32_t required_count = sizeof(required_extensions) / sizeof(required_extensions[0]);
  bool all_found = true;
  for(uint32_t i = 0; i < required_count; ++i){
    bool found = false;
    for(uint32_t j = 0; j < count; ++j){
      if(strcmp(extensions[j].extensionName, required_extensions[i]) == 0){
        found = true;
        break;
      }
    }
    if(!found){
      all_found = false;
      break;
    }
  }

  free(extensions);
  return true;
}

static inline bool is_physical_device_suitable(VkPhysicalDevice* device, VkSurfaceKHR* surface){
  QueueFamilyIndex indices = find_family_queues(device, *surface);
  bool extension_support = check_physical_device_extensions(device);
  return (indices.graphics_family != UINT32_MAX && indices.present_family != UINT32_MAX) && extension_support;
}

static inline int rate_physical_device(VkPhysicalDevice* device, VkSurfaceKHR* surface){
  if(!is_physical_device_suitable(device, surface)) return 0;
  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceProperties(*device, &device_properties);
  vkGetPhysicalDeviceFeatures(*device, &device_features);

  int score = 0;
  if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    score += 100;
  score += device_properties.limits.maxImageDimension2D;

  if(!device_features.geometryShader)
    return 0;

  return score;
}

static inline CON_RESULT conf_PickPhysicalDevice(const PhysicalDeviceConfig* config, VkPhysicalDevice* out_physical_device){
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(*config->instance, &device_count, NULL);
  VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(device_count * sizeof(VkPhysicalDevice));
  if(!devices){
    return CON_FAILED;
  }
  vkEnumeratePhysicalDevices(*config->instance, &device_count, devices);

  VkPhysicalDevice best_choice = VK_NULL_HANDLE;
  int32_t max_score = INT32_MIN;
  for(uint32_t i = 0; i < device_count; ++i){
    int score = rate_physical_device(&devices[i], config->surface);
    if(score >= max_score){
      max_score = score;
      best_choice = devices[i];
    }
  }

  free(devices);

  if(max_score > INT32_MIN && best_choice != VK_NULL_HANDLE){
    *out_physical_device = best_choice;
    return CON_SUCCESS;
  }

  return CON_FAILED;
}

#endif
