#pragma once
#include "Voortman3DCore.hpp"
#include "resource.h"

namespace Voortman3D {
	class Voortman3D final : public Voortman3DCore {
	public:
		Voortman3D(HINSTANCE hInstance);
	};
}

// Easy defined main for reuse in other projects
VOORTMAN_3D_MAIN()