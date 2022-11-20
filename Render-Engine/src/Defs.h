#pragma once

#define RENDER_API __declspec(dllexport)

#define public_var		public
#define public_dirty	public

#define private_var		private

// Object will no be able to be copied
#define NO_COPY(CLASS_NAME) CLASS_NAME(const CLASS_NAME&) = delete;\
							CLASS_NAME& operator=(const CLASS_NAME&) = delete;

// Object will not be able to be moved
#define NO_MOVE(CLASS_NAME) CLASS_NAME(CLASS_NAME&&) = delete;\
							CLASS_NAME& operator=(CLASS_NAME&&) = delete;
							