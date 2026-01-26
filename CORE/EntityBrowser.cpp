#include "EntityBrowser.hpp"



namespace EntityBrowser {

	std::vector<ComponentTypeInfo> registeredComponents{};

	const std::vector<ComponentTypeInfo>& getRegisteredComponents() { return registeredComponents; }

}
