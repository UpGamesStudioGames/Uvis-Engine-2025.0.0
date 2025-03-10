#include "Core.h"

#include <iostream>

#include "Editor.h"

#include "ScriptEngine.h"

#include <iostream>

namespace Uvis {

	



	bool Scripting;
	void RenderCore() {
		
		

		if (Scripting) {
			printf("Scripting Launched");
		}
		else {
			ScriptEngine::ReloadAssembly();
			printf("scripting engine crashed and needs the reload assembly");
		}

		
	}
}
