// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

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
	static bool SaveStructToXMLFile(const UStruct* StructDefinition, const void* Struct, const FString& XmlFile, int64 CheckFlags = 0, int64 SkipFlags = 0);
	static bool UStructToXMLString(const UStruct* StructDefinition, const void* Struct, FString& JSONString,int64 CheckFlags = 0, int64 SkipFlags = 0);
	static bool UStructToXML(const UStruct* StructDefinition, const void* Struct, tinyxml2::XMLNode* RootNode, int64 CheckFlags = 0, int64 SkipFlags = 0);
	static bool UPropertyToXMLNode(FProperty* Property,const void* Struct, tinyxml2::XMLNode* XmlNode, int64 CheckFlags = 0, int64 SkipFlags = 0);

	template<typename InStructType>
	static bool UStructToXMLString(const InStructType& Struct, FString& JSONString, int64 CheckFlags = 0 , int64 SkipFlags = 0)
	{
		return FXmlObjectConverter::UStructToXMLString(InStructType::StaticStruct(), &Struct, JSONString, CheckFlags, SkipFlags);
	}

	template<typename InStructType>
	static bool SaveStructToXMLFile(const InStructType& Struct, const FString& XmlFile, int64 CheckFlags = 0, int64 SkipFlags = 0)
	{
		return FXmlObjectConverter::SaveStructToXMLFile(InStructType::StaticStruct(), &Struct, XmlFile, CheckFlags, SkipFlags);
	}
public:
	// XML -> Struct
	template<typename OutStructType>
	static bool XmlStringToUStruct(const FString& XmlString, OutStructType* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0)
	{
		return FXmlObjectConverter::XmlObjectStringToUStruct(XmlString, OutStructType::StaticStruct(), OutStruct, CheckFlags, SkipFlags);
	}

	template<typename OutStructType>
	static bool XmlObjectToUStruct(const tinyxml2::XMLNode* JsonObject, OutStructType* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0)
	{
		return XmlObjectToUStruct(JsonObject, OutStructType::StaticStruct(), OutStruct, CheckFlags, SkipFlags);
	}

	template<typename OutStructType>
	static bool XmlFileToUStruct(const FString& XMLFilePath, OutStructType* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0)
	{
		return FXmlObjectConverter::XmlFileToUStruct(XMLFilePath, OutStructType::StaticStruct(), OutStruct, CheckFlags, SkipFlags);
	}

	static bool XmlObjectToUStruct(const tinyxml2::XMLNode* JsonObject,const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);

	static bool XmlObjectStringToUStruct(const FString& JsonString, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);

	static bool XmlNodeToUProperty(const tinyxml2::XMLNode* XmlNode, FProperty* Property, void* OutValue, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);

	static bool XmlFileToUStruct(const FString& XMLFilePath, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);

};
