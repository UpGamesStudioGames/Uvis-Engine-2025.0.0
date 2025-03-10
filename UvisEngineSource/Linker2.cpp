#include "Core.h"
#include "Linker.h"
#include "Editor.h"

namespace Uvis {
	class UVIS_API {
	public:
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}

		// General event handler
		virtual void OnEvent(void* eventCallback) {}

		// Event timeout handler
		virtual void OnEventTimeout(int timeoutSeconds) {}
	};
}
