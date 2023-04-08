#include "pch.h"
#include "GObject.h"

namespace Render {
	std::unordered_map<std::string, std::shared_ptr<GModel>> GObject::m_ModelBuffer{};
}