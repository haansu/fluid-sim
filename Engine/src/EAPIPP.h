#pragma once

#ifdef ENGINE_API_EXPORT 
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
