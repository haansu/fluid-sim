#pragma once

#ifdef RENDERING_API_EXPORT 
#define RENDERING_API __declspec(dllexport)
#else
#define RENDERING_API __declspec(dllimport)
#endif
