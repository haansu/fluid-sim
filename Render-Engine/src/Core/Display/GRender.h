#pragma once

// Core
#include "GModel.h"
#include <Defs.h>
//

// STL
#include <vector>
//

namespace Render {

	class GRender {
	public:
		NO_COPY(GRender);

		GRender() {};

		void AddModel(const GModel& model);
		void DeleteModel(const GModel& model);

	private:
		std::vector<GModel> m_Models;
	};

}

