#pragma once

#include <Defs.h>
#include <Core/Extra.h>
#include "ObjectSettings.h"

NAMESPACE_START_SCOPE_RND

class OScript;
class ORenderer;

class Entity {
public:
	RENDER_API Entity();
	RENDER_API ~Entity();

	RENDER_API void SetModel(std::string path, glm::vec4 color);
	
	RENDER_API Transform GetTransfrom();

	RENDER_API void SetTransform(const rnd::Transform& transform);
	RENDER_API void SetRotation(glm::vec3 rotate);
	RENDER_API void SetTranslation(glm::vec3 translate);
	RENDER_API void SetScale(glm::vec3 scale);

private:
	ObjectSettings objSettings;
	
	ORenderer* m_ORenderer;
};

NAMESPACE_END_SCOPE_RND
