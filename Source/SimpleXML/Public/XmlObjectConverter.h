// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FXmlNode;
/**
 * 
 */
class SIMPLEXML_API XmlObjectConverter
{
public:
	XmlObjectConverter();
	~XmlObjectConverter();


public:
	static bool UPropertyToXMLString(const UStruct* StructDefinition, const void* Struct, FString& JSONString,int64 CheckFlags, int64 SkipFlags);
};
