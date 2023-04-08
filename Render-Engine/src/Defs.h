#pragma once

#ifdef RENDER_API_EXPORT
#define RENDER_API __declspec(dllexport)
#else
#define RENDER_API __declspec(dllimport)
#endif

#define public_var		public
#define public_dirty	public

#define private_var		private

// Object will no be able to be copied
#define NO_COPY(CLASS_NAME) CLASS_NAME(const CLASS_NAME&) = delete;\
							CLASS_NAME& operator=(const CLASS_NAME&) = delete;

// Object will not be able to be moved
#define NO_MOVE(CLASS_NAME) CLASS_NAME(CLASS_NAME&&) = delete;\
							CLASS_NAME& operator=(CLASS_NAME&&) = delete;


#define NAMESPACE_START_SCOPE_RND namespace rnd {
#define NAMESPACE_END_SCOPE_RND }

#define NAMESPACE_START_SCOPE_RENDER namespace rnd {
#define NAMESPACE_END_SCOPE_RENDER }
