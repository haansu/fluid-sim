#pragma once

// Core
#include "GModel.h"
#include "GColor.h"
#include <Defs.h>
#include <Core/Extra.h>
//

// STL
#include <unordered_map>
#include <memory>
//

namespace Render {

	class GObject {
	public:
		GObject(GDevice& device, const std::string& modelPath, glm::vec4 color) {
			this->color = color;

			if (m_ModelBuffer.find(modelPath) == m_ModelBuffer.end()) {
				m_Model = std::make_shared<GModel>(device, modelPath);
 				m_ModelBuffer[modelPath] = m_Model;

				return;
			}
			
			m_Model = m_ModelBuffer[modelPath];
		}

		~GObject() {}

		std::shared_ptr<GModel> GetModelPtr() { return m_Model; }
	public_var:
		Trf transform{};
		glm::vec4 color{1.0f};

	private:
		std::shared_ptr<GModel> m_Model;
		static std::unordered_map<std::string, std::shared_ptr<GModel>> m_ModelBuffer;
	};

}

