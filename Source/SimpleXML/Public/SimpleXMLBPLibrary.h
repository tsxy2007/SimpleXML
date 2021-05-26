// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleXMLBPLibrary.generated.h"


USTRUCT(BlueprintType)
struct FTestData
{
public:
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, Category = TestData)
		FMatrix Transform;

	UPROPERTY(EditAnywhere, Category = TestData)
		FColor VertexColor;

	UPROPERTY(EditAnywhere, Category = TestData)
		int32 MaterialIndex;


public:
	FTestData()
		: Transform(FMatrix::Identity)
		, VertexColor(FColor::White)
		, MaterialIndex(INDEX_NONE)
	{
	}
};


/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class USimpleXMLBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "StructReference", DisplayName = "Struct To JSON String"), Category = "File|Json")
	static void UStructToJsonObjectString(const int32& StructReference, FString& JSONString);
	DECLARE_FUNCTION(execUStructToJsonObjectString);

	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "JSON String To Struct", CustomStructureParam = "StructReference"), Category = "File|Json")	
	static bool UJsonStringToStruct(const FString& JSONString, int32 StructReference);
	DECLARE_FUNCTION(execUJsonStringToStruct);

	static bool JsonObjectStringToUStruct(const FString& JsonString, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);


	UFUNCTION(BlueprintPure,CustomThunk, meta = (CustomStructureParam = "StructReference", DisplayName = "Struct to XML String"), Category = "File|XML")
	static void UStructToXMLObjectString(const int32& StructReference, FString& XMLString);
	DECLARE_FUNCTION(execUStructToXMLObjectString);

	UFUNCTION(BlueprintPure, CustomThunk, meta = (DisplayName = "XML String To Struct", CustomStructureParam = "StructReference"), Category = "File|XML")
	static bool UXmlStringToStruct(const FString& XmlString, int32 StructReference);
	DECLARE_FUNCTION(execUXmlStringToStruct);
};
