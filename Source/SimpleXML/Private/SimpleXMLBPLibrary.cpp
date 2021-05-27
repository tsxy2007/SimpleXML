// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

#include "SimpleXMLBPLibrary.h"
#include "SimpleXML.h"
#include "JsonObjectConverter.h"
#include "XmlFile.h"
#include "XmlObjectConverter.h"


USimpleXMLBPLibrary::USimpleXMLBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool USimpleXMLBPLibrary::JsonObjectStringToUStruct(const FString& JsonString, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags /*= 0*/, int64 SkipFlags /*= 0*/)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to parse json=[%s]"), *JsonString);
		return false;
	}
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructDefinition, OutStruct, CheckFlags, SkipFlags))
	{
		UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to deserialize. json=[%s]"), *JsonString);
		return false;
	}
	return true;
}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execUStructToJsonObjectString)
{
	Stack.Step(Stack.Object, NULL);
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;

	//Get JsonString reference
	P_GET_PROPERTY_REF(FStrProperty, JSONString);
	P_FINISH;

	P_NATIVE_BEGIN;
	FJsonObjectConverter::UStructToJsonObjectString(StructProperty->Struct, StructPtr, JSONString, 0, 0);
	P_NATIVE_END;
}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execUJsonStringToStruct)
{
	P_GET_PROPERTY(FStrProperty, JSONString);


	Stack.Step(Stack.Object, NULL);
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	P_NATIVE_BEGIN; 
	bool success = USimpleXMLBPLibrary::JsonObjectStringToUStruct(JSONString, StructProperty->Struct, StructPtr, 0, 0);
	*(bool*)RESULT_PARAM = success;
	P_NATIVE_END;
}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execUStructToXMLObjectString)
{
	Stack.Step(Stack.Object, NULL);
	FStructProperty* LocalProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;

	//Get JsonString reference
	P_GET_PROPERTY_REF(FStrProperty, JSONString);
	P_FINISH;

	P_NATIVE_BEGIN;
	FXmlObjectConverter::UStructToXMLString(LocalProperty->Struct, StructPtr, JSONString, 0, 0);
	P_NATIVE_END;
	
}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execSaveStructToXml)
{
	Stack.Step(Stack.Object, NULL);
	FStructProperty* LocalProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;

	//Get JsonString reference
	P_GET_PROPERTY_REF(FStrProperty, XmlFilePath);
	P_FINISH;

	P_NATIVE_BEGIN;
	bool success = FXmlObjectConverter::SaveStructToXMLFile(LocalProperty->Struct, StructPtr, XmlFilePath, 0, 0);
	*(bool*)RESULT_PARAM = success;
	P_NATIVE_END;
}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execUXmlStringToStruct)
{
	P_GET_PROPERTY(FStrProperty, JSONString);
	Stack.Step(Stack.Object, NULL);
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	P_NATIVE_BEGIN;
	bool success = FXmlObjectConverter::XmlObjectStringToUStruct(JSONString, StructProperty->Struct, StructPtr, 0, 0);
	*(bool*)RESULT_PARAM = success;
	P_NATIVE_END;
}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execUXmlFileToStruct)
{
	P_GET_PROPERTY(FStrProperty, XmlFilePath);
	Stack.Step(Stack.Object, NULL);
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	P_NATIVE_BEGIN;
	bool success = FXmlObjectConverter::XmlFileToUStruct(XmlFilePath, StructProperty->Struct, StructPtr, 0, 0);
	*(bool*)RESULT_PARAM = success;
	P_NATIVE_END;
}