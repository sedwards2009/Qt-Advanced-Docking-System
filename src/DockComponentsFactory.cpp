//============================================================================
/// \file   DockComponentsFactory.cpp
/// \author Uwe Kindler
/// \date   10.02.2020
/// \brief  Implementation of DockComponentsFactory
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <AutoHideTab.h>
#include "DockComponentsFactory.h"

#include <memory>

#include "DockWidgetTab.h"
#include "DockAreaTabBar.h"
#include "DockAreaTitleBar.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "FloatingDockContainer.h"

namespace ads
{
static std::unique_ptr<CDockComponentsFactory> DefaultFactory(new CDockComponentsFactory());


//============================================================================
CDockWidgetTab* CDockComponentsFactory::createDockWidgetTab(CDockWidget* DockWidget) const
{
	return new CDockWidgetTab(DockWidget);
}

//============================================================================
CAutoHideTab* CDockComponentsFactory::createDockWidgetSideTab(CDockWidget *DockWidget) const
{
	return new CAutoHideTab(DockWidget);
}


//============================================================================
CDockAreaTabBar* CDockComponentsFactory::createDockAreaTabBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTabBar(DockArea);
}


//============================================================================
CDockAreaTitleBar* CDockComponentsFactory::createDockAreaTitleBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTitleBar(DockArea);
}


//============================================================================
CFloatingDockContainer* CDockComponentsFactory::createCFloatingDockContainer(CDockManager* DockManager) const
{
	CFloatingDockContainer* result = new CFloatingDockContainer(DockManager);
	result->init();
	return result;
}


//============================================================================
CFloatingDockContainer* CDockComponentsFactory::createCFloatingDockContainer(CDockAreaWidget* DockArea) const
{
	CFloatingDockContainer* result = new CFloatingDockContainer(DockArea);
	result->init();
	return result;
}


//============================================================================
CFloatingDockContainer* CDockComponentsFactory::createCFloatingDockContainer(CDockWidget* DockWidget) const
{
	CFloatingDockContainer* result = new CFloatingDockContainer(DockWidget);
	result->init();
	return result;
}


//============================================================================
const CDockComponentsFactory* CDockComponentsFactory::factory()
{
	return DefaultFactory.get();
}


//============================================================================
void CDockComponentsFactory::setFactory(CDockComponentsFactory* Factory)
{
	DefaultFactory.reset(Factory);
}


//============================================================================
void CDockComponentsFactory::resetDefaultFactory()
{
	DefaultFactory.reset(new CDockComponentsFactory());
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockComponentsFactory.cpp
