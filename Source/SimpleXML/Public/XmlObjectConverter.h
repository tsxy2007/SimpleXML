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
	static TSharedPtr<FXmlNode> UPropertyToXMLNode(FProperty* Property, const void* Value);
};
