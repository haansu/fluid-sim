#pragma once

// Core
#include "GModel.h"
#include "GColor.h"
#include <Defs.h>
#include <Core/Extra.h>
//

namespace Render {

	class GObject {
	public:
		GObject(GDevice& device, const std::string& modelPath, glm::vec4 color) {
			m_Model = new GModel(device, modelPath, color);
		}

		~GObject() {
			delete m_Model;
		}
	public_var:
		Transform transform{};
		GModel* m_Model;
		GColor m_Color;
	private:

	};

}

