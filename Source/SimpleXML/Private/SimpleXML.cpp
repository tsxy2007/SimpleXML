// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

#include "SimpleXML.h"
#include "SimpleXMLBPLibrary.h"
#include "XmlObjectConverter.h"

#define LOCTEXT_NAMESPACE "FSimpleXMLModule"
DEFINE_LOG_CATEGORY(LogSimpleXML);

void FSimpleXMLModule::StartupModule()
{
}

void FSimpleXMLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimpleXMLModule, SimpleXML)