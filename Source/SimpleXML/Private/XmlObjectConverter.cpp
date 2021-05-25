// Fill out your copyright notice in the Description page of Project Settings.


#include "XmlObjectConverter.h"
#include "tinyxml2.h"
#include "SimpleXML.h"

FXmlObjectConverter::FXmlObjectConverter()
{
}

FXmlObjectConverter::~FXmlObjectConverter()
{
}

bool FXmlObjectConverter::UStructToXMLString(const UStruct* StructDefinition, const void* Struct, FString& OutJsonString, int64 CheckFlags, int64 SkipFlags)
{
	bool bResult = false;
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLNode* BaseNode = doc.NewElement("Data");
	doc.InsertEndChild(BaseNode);

	FXmlObjectConverter::UStructToXML(StructDefinition, Struct, BaseNode,CheckFlags, SkipFlags);
	FString XmlPath = FPaths::GameDevelopersDir() + TEXT("1.xml");
	doc.SaveFile(TCHAR_TO_ANSI(*XmlPath));
	return bResult;
}

bool FXmlObjectConverter::UStructToXML(const UStruct* StructDefinition, const void* Struct, tinyxml2::XMLNode* RootNode, int64 CheckFlags, int64 SkipFlags)
{
	for (TFieldIterator<FProperty> PropIt(StructDefinition); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		if (CheckFlags != 0 && !Property->HasAnyPropertyFlags(CheckFlags))
		{
			continue;
		}
		if (Property->HasAnyPropertyFlags(SkipFlags))
		{
			continue;
		}

		FXmlObjectConverter::UPropertyToXMLNode(Property, Struct, RootNode, CheckFlags, SkipFlags);
	}
	return true;
}

bool FXmlObjectConverter::UPropertyToXMLNode(FProperty* Property, const void* Struct, tinyxml2::XMLNode* RootNode, int64 CheckFlags, int64 SkipFlags)
{
	FString VariName = Property->GetName();
	const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);
	FString StringValue = "";
	bool bIsContainer = false;
	if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		// export enums as strings
		UEnum* EnumDef = EnumProperty->GetEnum();
		StringValue = EnumDef->GetNameStringByValue(EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(Value));
	}
	else if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
	{
		// see if it's an enum
		UEnum* EnumDef = NumericProperty->GetIntPropertyEnum();
		if (EnumDef != NULL)
		{
			// export enums as strings
			StringValue = EnumDef->GetNameStringByValue(NumericProperty->GetSignedIntPropertyValue(Value));
		}

		// We want to export numbers as numbers
		if (NumericProperty->IsFloatingPoint())
		{
			StringValue = FString::SanitizeFloat(NumericProperty->GetFloatingPointPropertyValue(Value));
		}
		else if (NumericProperty->IsInteger())
		{
			StringValue = FString::FormatAsNumber(NumericProperty->GetSignedIntPropertyValue(Value));
		}
		// fall through to default
	}
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		// Export bools as bools
		StringValue = FString::FormatAsNumber(BoolProperty->GetPropertyValue(Value));
	}
	else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		StringValue = StringProperty->GetPropertyValue(Value);
	}
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		StringValue = TextProperty->GetPropertyValue(Value).ToString();
	}
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		FScriptArrayHelper Helper(ArrayProperty, Value);
		
		FString Tag = VariName + TEXT("_es");
		tinyxml2::XMLNode* ArrayXMLNode = RootNode->GetDocument()->NewElement(TCHAR_TO_ANSI(*Tag));
		RootNode->InsertEndChild(ArrayXMLNode);
		for (int32 i = 0, n = Helper.Num(); i < n; ++i)
		{
			FXmlObjectConverter::UPropertyToXMLNode(ArrayProperty->Inner, Helper.GetRawPtr(i), ArrayXMLNode, CheckFlags & (~CPF_ParmFlags), SkipFlags);
		}
		bIsContainer = true;
	}
	else if (FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		FString Tag = VariName + TEXT("_set");
		tinyxml2::XMLNode* ArrayXMLNode = RootNode->GetDocument()->NewElement(TCHAR_TO_ANSI(*Tag));
		RootNode->InsertEndChild(ArrayXMLNode);
		FScriptSetHelper Helper(SetProperty, Value);
		for (int32 i = 0, n = Helper.Num(); n; ++i)
		{
			if (Helper.IsValidIndex(i))
			{
				FXmlObjectConverter::UPropertyToXMLNode(SetProperty->ElementProp, Helper.GetElementPtr(i), ArrayXMLNode, CheckFlags & (~CPF_ParmFlags), SkipFlags);
				--n;
			}
		}
		bIsContainer = true;
	}
	else if (FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		
		FString Tag = VariName + TEXT("_map");
		tinyxml2::XMLNode* MapXMLNode = RootNode->GetDocument()->NewElement(TCHAR_TO_ANSI(*Tag));
		RootNode->InsertEndChild(MapXMLNode);

		FScriptMapHelper Helper(MapProperty, Value);
		for (int32 i = 0, n = Helper.Num(); n; ++i)
		{
			if (Helper.IsValidIndex(i))
			{
				FString DataTag = VariName + TEXT("_mapData");
				tinyxml2::XMLElement* MapXMLItemNode = MapXMLNode->GetDocument()->NewElement(TCHAR_TO_ANSI(*VariName));
				MapXMLNode->InsertEndChild(MapXMLItemNode);
				FString KeyString;
				MapProperty->KeyProp->ExportTextItem(KeyString, Helper.GetKeyPtr(i), nullptr, nullptr, 0);
				MapXMLItemNode->SetAttribute("Key",TCHAR_TO_ANSI(*KeyString));
				FXmlObjectConverter::UPropertyToXMLNode(MapProperty->ValueProp, Helper.GetValuePtr(i), MapXMLItemNode, CheckFlags & (~CPF_ParmFlags), SkipFlags);
				--n;
			}
		}
		bIsContainer = true;
	}
	else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
		FString Tag = VariName;
		tinyxml2::XMLNode* StructElement = RootNode->GetDocument()->NewElement(TCHAR_TO_ANSI(*VariName));
		RootNode->InsertEndChild(StructElement);
		FXmlObjectConverter::UStructToXML(StructProperty->Struct, Value, StructElement, CheckFlags& (~CPF_ParmFlags), SkipFlags);
		bIsContainer = true;
	}
	else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		// Instanced properties should be copied by value, while normal UObject* properties should output as asset references
		UObject* Object = ObjectProperty->GetObjectPropertyValue(Value);
		if (Object && (ObjectProperty->HasAnyPropertyFlags(CPF_PersistentInstance) /*|| (OuterProperty && OuterProperty->HasAnyPropertyFlags(CPF_PersistentInstance))*/))
		{
			tinyxml2::XMLNode* ClassElement = RootNode->GetDocument()->NewElement(TCHAR_TO_ANSI(*VariName));
			RootNode->InsertEndChild(ClassElement);
			FXmlObjectConverter::UStructToXML(ObjectProperty->GetObjectPropertyValue(Value)->GetClass(), Object, ClassElement, CheckFlags, SkipFlags);
			bIsContainer = true;
		}
		else
		{
			Property->ExportTextItem(StringValue, Value, nullptr, nullptr, PPF_None);
		}
	}
	else
	{
		// Default to export as string for everything else
		Property->ExportTextItem(StringValue, Value, NULL, NULL, PPF_None);
	}
	if (bIsContainer == false)
	{
		//RootNode->AppendChildNode(VariName, StringValue);
		tinyxml2::XMLElement* PropertyNode = RootNode->GetDocument()->NewElement(TCHAR_TO_ANSI(*VariName));
		RootNode->InsertEndChild(PropertyNode);
		PropertyNode->SetAttribute("Value", TCHAR_TO_ANSI(*StringValue));
		UE_LOG(LogSimpleXML, Warning, TEXT("XML Key [%s] Value[%s]")
			, *VariName, *StringValue);
	}
	return true;
}
