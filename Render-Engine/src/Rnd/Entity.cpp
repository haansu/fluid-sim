#include "pch.h"
#include "Entity.h"

// Core
#include "ORenderer.h"
//

// STL
#include <memory.h>
//


NAMESPACE_START_SCOPE_RND

RENDER_API Entity::Entity() {
	m_ORenderer = ORenderer::GetInstance();
}

RENDER_API Entity::~Entity() {
	m_ORenderer->m_DrawListSettings.erase(objSettings.id);
}

RENDER_API void Entity::SetModel(std::string path, glm::vec4 color) {
	// '_' is reserved for generated models
	if (path.length() == 0 || path[0] == '_')
		return;

	objSettings.path = path;
	objSettings.color = color;
	m_ORenderer->m_DrawListSettings[objSettings.id] = &objSettings;
}

RENDER_API void Entity::SetColor(glm::vec4 color) {
	objSettings.color = color;
}

RENDER_API rnd::Transform Entity::GetTransfrom() {
	return objSettings.transform;
}

RENDER_API void Entity::SetTransform(const rnd::Transform& transform) {
	objSettings.transform = transform;
}

RENDER_API void Entity::SetRotation(glm::vec3 rotate) {
	objSettings.transform.rotate = rotate;
}

RENDER_API void Entity::SetTranslation(glm::vec3 translate) {
	objSettings.transform.translate = translate;
}

RENDER_API void Entity::SetScale(glm::vec3 scale) {
	objSettings.transform.scale = scale;
}

NAMESPACE_END_SCOPE_RND