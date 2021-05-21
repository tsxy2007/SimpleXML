// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSimpleXMLModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
