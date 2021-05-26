// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "tinyxml2.h"

class FXmlNode;
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
	template<typename OutStructType>
	static bool JsonObjectStringToUStruct(const FString& XmlString, OutStructType* OutStruct, int64 CheckFlags, int64 SkipFlags);

	template<typename OutStructType>
	static bool XmlObjectToUStruct(const tinyxml2::XMLNode* JsonObject, OutStructType* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0)
	{
		return XmlObjectToUStruct(JsonObject, OutStructType::StaticStruct(), OutStruct, CheckFlags, SkipFlags);
	}

	static bool XmlObjectToUStruct(const tinyxml2::XMLNode* JsonObject,const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);

	static bool XmlObjectStringToUStruct(const FString& JsonString, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);

	static bool XmlNodeToUProperty(const tinyxml2::XMLNode* XmlNode, FProperty* Property, void* OutValue, int64 CheckFlags, int64 SkipFlags);

};

template<typename OutStructType>
bool FXmlObjectConverter::JsonObjectStringToUStruct(const FString& XmlString, OutStructType* OutStruct, int64 CheckFlags, int64 SkipFlags)
{
	tinyxml2::XMLDocument Doc;
	Doc.Parse(XmlString);
	return true;
}