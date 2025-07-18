#pragma once

/*
	cancelled for now, cause i found no reason for this abstraction
*/












// Core UI base classes
#include "core/UIElement.hpp"
#include "../widgets/container/UIRoot.hpp"
#include "../widgets/UIButton.hpp"
#include "../widgets/UIButton.hpp"
#include "core/ElementManager.hpp"

// Layout system
//#include "layout/Anchor.hpp"
//#include "layout/Constraints.hpp"
//#include "layout/UILayout.hpp"
//
//// Renderer
//#include "renderer/Renderer.hpp"
//#include "renderer/FontRenderer.hpp"
//
//// Widget manager and resource manager
//#include "manager/WidgetManager.hpp"
//#include "manager/ResourceManager.hpp"

// Utilities
#include "utils/assetManager.hpp"
//#include "utils/Color.hpp"
//
//// Widgets
//#include "../../widgets/Label.hpp"
//#include "../../widgets/Button.hpp"
//#include "../../widgets/Panel.hpp"


class UILib{
private:
	GUI manager;

public:
	UILib(){}

	GUI* getManager(){return &manager;}
};

/*----------------------------NOT YET IMPLEMENTED IGNORE----------------------------*/