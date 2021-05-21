// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

#include "SimpleXMLBPLibrary.h"
#include "SimpleXML.h"
#include "JsonObjectConverter.h"

USimpleXMLBPLibrary::USimpleXMLBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execUStructToJsonObjectString)
{
	Stack.Step(Stack.Object, NULL);
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;

	//Get JsonString reference
	P_GET_PROPERTY_REF(UStrProperty, JSONString);
	P_FINISH;

	P_NATIVE_BEGIN;
	FJsonObjectConverter::UStructToJsonObjectString(StructProperty->Struct, StructPtr, JSONString, 0, 0);
	P_NATIVE_END;
}

DEFINE_FUNCTION(USimpleXMLBPLibrary::execUStructToXMLObjectString)
{

}

void USimpleXMLBPLibrary::Test(const FTestData& Data, FString& XMLString)
{

}