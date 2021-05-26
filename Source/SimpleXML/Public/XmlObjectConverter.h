// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FXmlNode;
namespace tinyxml2
{
	class XMLNode;
};
/**
 * 
 */
class SIMPLEXML_API FXmlObjectConverter
{
public:
	// Struct -> XML
	static bool UStructToXMLString(const UStruct* StructDefinition, const void* Struct, FString& JSONString,int64 CheckFlags, int64 SkipFlags);
	static bool UStructToXML(const UStruct* StructDefinition, const void* Struct, tinyxml2::XMLNode* RootNode, int64 CheckFlags, int64 SkipFlags);
	static bool UPropertyToXMLNode(FProperty* Property,const void* Struct, tinyxml2::XMLNode* XmlNode, int64 CheckFlags, int64 SkipFlags);

public:
	// XML -> Struct
};
