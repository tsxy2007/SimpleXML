// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

#include "SimpleXML.h"

#define LOCTEXT_NAMESPACE "FSimpleXMLModule"

void FSimpleXMLModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	
}

void FSimpleXMLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleXMLModule, SimpleXML)