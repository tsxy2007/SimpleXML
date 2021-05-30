// Copyright 2020 - 2021, butterfly, SimpleXML Plugin, All Rights Reserved.

#include "XmlObjectConverter.h"
#include "SimpleXML.h"
#include "UObject/TextProperty.h"

bool FXmlObjectConverter::SaveStructToXMLFile(const UStruct* StructDefinition, const void* Struct, const FString& XmlFile, int64 CheckFlags, int64 SkipFlags)
{
	const char* declaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";
	tinyxml2::XMLDocument doc;
	//doc.GetDocument()->Parse(declaration);
	tinyxml2::XMLDeclaration* Dec = doc.NewDeclaration(declaration);
	doc.InsertFirstChild(Dec);
	tinyxml2::XMLNode* BaseNode = doc.NewElement("Data");
	doc.InsertEndChild(BaseNode);

	if (!FXmlObjectConverter::UStructToXML(StructDefinition, Struct, BaseNode, CheckFlags, SkipFlags))
	{
		return false;
	}
	if (doc.SaveFile(TCHAR_TO_ANSI(*XmlFile), true) != tinyxml2::XML_SUCCESS)
	{
		return false;
	}
	return true;
}

bool FXmlObjectConverter::UStructToXMLString(const UStruct* StructDefinition, const void* Struct, FString& OutJsonString, int64 CheckFlags, int64 SkipFlags)
{
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLNode* BaseNode = doc.NewElement("Data");
	doc.InsertEndChild(BaseNode);

	if (!FXmlObjectConverter::UStructToXML(StructDefinition, Struct, BaseNode, CheckFlags, SkipFlags))
	{
		return false;
	}
	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);//将Print打印到Xmlprint类中 即保存在内存中
	OutJsonString = FString(printer.CStr());
	return true;
}

bool FXmlObjectConverter::UStructToXML(const UStruct* StructDefinition, const void* Struct, tinyxml2::XMLNode* RootNode, int64 CheckFlags, int64 SkipFlags)
{
	if (SkipFlags == 0)
	{
		// If we have no specified skip flags, skip deprecated, transient and skip serialization by default when writing
		SkipFlags |= CPF_Deprecated | CPF_Transient;
	}
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
		const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);
		FXmlObjectConverter::UPropertyToXMLNode(Property, Value, RootNode, CheckFlags, SkipFlags);
	}
	return true;
}

bool FXmlObjectConverter::UPropertyToXMLNode(FProperty* Property, const void* Value, tinyxml2::XMLNode* RootNode, int64 CheckFlags, int64 SkipFlags)
{
	FString VariName = Property->GetName();

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
		tinyxml2::XMLNode* ArrayXMLNode = RootNode->GetDocument()->NewElement(TCHAR_TO_UTF8(*Tag));
		RootNode->InsertEndChild(ArrayXMLNode);
		ArrayXMLNode->ToElement()->SetAttribute("Size", Helper.Num());
		for (int32 i = 0, n = Helper.Num(); i < n; ++i)
		{
			FXmlObjectConverter::UPropertyToXMLNode(ArrayProperty->Inner, Helper.GetRawPtr(i), ArrayXMLNode, CheckFlags & (~CPF_ParmFlags), SkipFlags);
		}
		bIsContainer = true;
	}
	else if (FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		FString Tag = VariName + TEXT("_set");
		tinyxml2::XMLNode* ArrayXMLNode = RootNode->GetDocument()->NewElement(TCHAR_TO_UTF8(*Tag));
		RootNode->InsertEndChild(ArrayXMLNode);
		FScriptSetHelper Helper(SetProperty, Value);
		ArrayXMLNode->ToElement()->SetAttribute("Size", Helper.Num());
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
		tinyxml2::XMLNode* MapXMLNode = RootNode->GetDocument()->NewElement(TCHAR_TO_UTF8(*Tag));
		RootNode->InsertEndChild(MapXMLNode);

		FScriptMapHelper Helper(MapProperty, Value);

		MapXMLNode->ToElement()->SetAttribute("Size", Helper.Num());

		for (int32 i = 0, n = Helper.Num(); n; ++i)
		{
			if (Helper.IsValidIndex(i))
			{
				FString DataTag = VariName + TEXT("_mapData");
				tinyxml2::XMLElement* MapXMLItemNode = MapXMLNode->GetDocument()->NewElement(TCHAR_TO_UTF8(*VariName));
				MapXMLNode->InsertEndChild(MapXMLItemNode);
				FString KeyString;
				MapProperty->KeyProp->ExportTextItem(KeyString, Helper.GetKeyPtr(i), nullptr, nullptr, 0);
				MapXMLItemNode->SetAttribute("Key",TCHAR_TO_UTF8(*KeyString));
				FXmlObjectConverter::UPropertyToXMLNode(MapProperty->ValueProp, Helper.GetValuePtr(i), MapXMLItemNode, CheckFlags & (~CPF_ParmFlags), SkipFlags);
				--n;
			}
		}
		bIsContainer = true;
	}
	else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
		if (TheCppStructOps && TheCppStructOps->HasExportTextItem())
		{
			Property->ExportTextItem(StringValue, Value, nullptr, nullptr, 0);
		}
		else
		{
			tinyxml2::XMLNode* StructElement = RootNode->GetDocument()->NewElement(TCHAR_TO_UTF8(*VariName));
			RootNode->InsertEndChild(StructElement);
			FXmlObjectConverter::UStructToXML(StructProperty->Struct, Value, StructElement, CheckFlags& (~CPF_ParmFlags), SkipFlags);
			bIsContainer = true;
		}
	}
	else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		// Instanced properties should be copied by value, while normal UObject* properties should output as asset references
		UObject* Object = ObjectProperty->GetObjectPropertyValue(Value);
		if (Object && (ObjectProperty->HasAnyPropertyFlags(CPF_PersistentInstance) /*|| (OuterProperty && OuterProperty->HasAnyPropertyFlags(CPF_PersistentInstance))*/))
		{
			tinyxml2::XMLNode* ClassElement = RootNode->GetDocument()->NewElement(TCHAR_TO_UTF8(*VariName));
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
		tinyxml2::XMLElement* PropertyNode = RootNode->GetDocument()->NewElement(TCHAR_TO_UTF8(*VariName));
		RootNode->InsertEndChild(PropertyNode);
		char* C_Str = TCHAR_TO_UTF8(*StringValue);
		PropertyNode->SetAttribute("Value", C_Str);
		
	}
	return true;
}

bool FXmlObjectConverter::XmlObjectToUStruct(const tinyxml2::XMLNode* XMLRoot, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags /*= 0*/, int64 SkipFlags /*= 0*/)
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
		FString VarName = Property->GetName();
		if (Property->IsA<FArrayProperty>())
		{
			VarName += TEXT("_es");
		}
		else if (Property->IsA<FSetProperty>())
		{
			VarName += TEXT("_set");
		}
		else if (Property->IsA<FMapProperty>())
		{
			VarName += TEXT("_map");
		}
		const tinyxml2::XMLElement* ChildXmlNode = XMLRoot->FirstChildElement(TCHAR_TO_UTF8(*VarName));
		void* Value = Property->ContainerPtrToValuePtr<uint8>(OutStruct);
		if (!FXmlObjectConverter::XmlNodeToUProperty(ChildXmlNode, Property, Value,StructDefinition,OutStruct, CheckFlags, SkipFlags))
		{
			return false;
		}
	}
	return true;
}

bool FXmlObjectConverter::XmlObjectStringToUStruct(const FString& XmlString, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags /*= 0*/, int64 SkipFlags /*= 0*/)
{
	tinyxml2::XMLDocument Doc;
	Doc.Parse(TCHAR_TO_UTF8(*XmlString),XmlString.Len());
	if (Doc.Error())
	{
		return false;
	}
	if (!FXmlObjectConverter::XmlObjectToUStruct(Doc.RootElement(),StructDefinition,OutStruct,CheckFlags,SkipFlags))
	{
		return false;
	}
	return true;
}

bool FXmlObjectConverter::XmlNodeToUProperty(const tinyxml2::XMLNode* XmlNode, FProperty* Property, void* OutValue, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags, int64 SkipFlags)
{
	if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		const UEnum* Enum = EnumProperty->GetEnum();
		check(Enum);
		FString StrValue(XmlNode->ToElement()->Attribute("Value"));
		int64 IntValue = Enum->GetValueByName(FName(*StrValue));
		if (IntValue == INDEX_NONE)
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable import enum %s from string value %s for property %s"), *Enum->CppType, *StrValue, *Property->GetNameCPP());
			return false;
		}
		EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutValue, IntValue);
	}
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		bool Result = (bool)XmlNode->ToElement()->IntAttribute("Value");
		BoolProperty->SetPropertyValue(OutValue, Result);
	}
	else if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
	{
		if (NumericProperty->IsEnum())
		{
			// see if we were passed a string for the enum
			const UEnum* Enum = NumericProperty->GetIntPropertyEnum();
			check(Enum); // should be assured by IsEnum()
			FString StrValue(XmlNode->ToElement()->Attribute("Value"));
			int64 IntValue = Enum->GetValueByName(FName(*StrValue));
			if (IntValue == INDEX_NONE)
			{
				UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable import enum %s from string value %s for property %s"), *Enum->CppType, *StrValue, *Property->GetNameCPP());
				return false;
			}
			NumericProperty->SetIntPropertyValue(OutValue, IntValue);
		}
		else if (NumericProperty->IsFloatingPoint())
		{
			// AsNumber will log an error for completely inappropriate types (then give us a default)
			NumericProperty->SetFloatingPointPropertyValue(OutValue, XmlNode->ToElement()->FloatAttribute("Value"));
		}
		else if (NumericProperty->IsInteger())
		{
			NumericProperty->SetIntPropertyValue(OutValue, (int64)XmlNode->ToElement()->FloatAttribute("Value"));
		}
		else
		{
			UE_LOG(LogSimpleXML, Error, TEXT("JsonValueToUProperty - Unable to set numeric property type %s for property %s"), *Property->GetClass()->GetName(), *Property->GetNameCPP());
			return false;
		}
	}
	else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		// AsString will log an error for completely inappropriate types (then give us a default)
		StringProperty->SetPropertyValue(OutValue, XmlNode->ToElement()->Attribute("Value"));
	}
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		FString VarName = Property->GetName();
		int32 ArrLen = XmlNode->ToElement()->IntAttribute("Size");
		// make the output array size match
		FScriptArrayHelper Helper(ArrayProperty, OutValue);
		Helper.Resize(ArrLen);

		// set the property values
		const tinyxml2::XMLNode* TmpNode = XmlNode->FirstChildElement(TCHAR_TO_UTF8(*VarName));
		for (int32 i = 0; TmpNode; ++i, TmpNode = TmpNode->NextSibling())
		{
			if (!FXmlObjectConverter::XmlNodeToUProperty(TmpNode, ArrayProperty->Inner, Helper.GetRawPtr(i),StructDefinition,OutStruct, CheckFlags & (~CPF_ParmFlags), SkipFlags))
			{
				UE_LOG(LogSimpleXML, Error, TEXT("XMLNodeToUProperty - Unable to deserialize array element [%d] for property %s"), i, *Property->GetNameCPP());
				return false;
			}
		}
	}
	else if (FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		FString VarName = Property->GetName();
		FString ValueVarName = Property->GetName() + "_Value";
		int32 MapSize = XmlNode->ToElement()->IntAttribute("Size");
		// make the output array size match
		FScriptMapHelper Helper(MapProperty, OutValue);
		Helper.EmptyValues(MapSize);

		// set the property values
		const tinyxml2::XMLNode* TmpNode = XmlNode->FirstChildElement(TCHAR_TO_UTF8(*VarName));
		for (int32 i = 0; TmpNode; ++i, TmpNode = TmpNode->NextSibling())
		{
			const tinyxml2::XMLNode* ValeNode = TmpNode->FirstChildElement(TCHAR_TO_UTF8(*ValueVarName));
			int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();
			FString ImportText(TmpNode->ToElement()->Attribute("Key"));
			const TCHAR* ImportTextPtr = *ImportText;
			MapProperty->KeyProp->ImportText(ImportTextPtr, Helper.GetKeyPtr(NewIndex), PPF_None, nullptr);
			if (!FXmlObjectConverter::XmlNodeToUProperty(ValeNode, MapProperty->ValueProp, Helper.GetValuePtr(NewIndex), StructDefinition, OutStruct, CheckFlags & (~CPF_ParmFlags), SkipFlags))
			{
				UE_LOG(LogSimpleXML, Error, TEXT("XMLNodeToUProperty - Unable to deserialize array element [%d] for property %s"), i, *Property->GetNameCPP());
				return false;
			}
		}
		Helper.Rehash();
		
	}
	else if (FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		FString VarName = Property->GetName();
		int32 ArrLen = XmlNode->ToElement()->IntAttribute("Size");
		// make the output array size match
		FScriptSetHelper Helper(SetProperty, OutValue);

		// set the property values
		const tinyxml2::XMLNode* TmpNode = XmlNode->FirstChildElement(TCHAR_TO_UTF8(*VarName));
		for (int32 i = 0; TmpNode; ++i, TmpNode = TmpNode->NextSibling())
		{
			int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();
			if (!FXmlObjectConverter::XmlNodeToUProperty(TmpNode, SetProperty->ElementProp, Helper.GetElementPtr(NewIndex), StructDefinition, OutStruct, CheckFlags & (~CPF_ParmFlags), SkipFlags))
			{
				UE_LOG(LogJson, Error, TEXT("XmlNodeToUProperty - Unable to deserialize set element [%d] for property %s"), i, *Property->GetNameCPP());
				return false;
			}
		}
		Helper.Rehash();
	}
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		TextProperty->SetPropertyValue(OutValue, FText::FromString(FString(XmlNode->ToElement()->Attribute("Value"))));
	}
	else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		static const FName NAME_DateTime(TEXT("DateTime"));
		static const FName NAME_Color(TEXT("Color"));
		static const FName NAME_LinearColor(TEXT("LinearColor"));
	
		if (StructProperty->Struct->GetFName() == NAME_DateTime)
		{
			FString DateString(XmlNode->ToElement()->Attribute("Value"));
			FDateTime& DateTimeOut = *(FDateTime*)OutValue;
			if (DateString == TEXT("min"))
			{
				// min representable value for our date struct. Actual date may vary by platform (this is used for sorting)
				DateTimeOut = FDateTime::MinValue();
			}
			else if (DateString == TEXT("max"))
			{
				// max representable value for our date struct. Actual date may vary by platform (this is used for sorting)
				DateTimeOut = FDateTime::MaxValue();
			}
			else if (DateString == TEXT("now"))
			{
				// this value's not really meaningful from json serialization (since we don't know timezone) but handle it anyway since we're handling the other keywords
				DateTimeOut = FDateTime::UtcNow();
			}
			else if (FDateTime::ParseIso8601(*DateString, DateTimeOut))
			{
				// ok
			}
			else if (FDateTime::Parse(DateString, DateTimeOut))
			{
				// ok
			}
			else
			{
				UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable to import FDateTime for property %s"), *Property->GetNameCPP());
				return false;
			}
		}
		/*else if (StructProperty->Struct->GetFName() == NAME_LinearColor)
		{
			FLinearColor& ColorOut = *(FLinearColor*)OutValue;
			FString ColorString(XmlNode->ToElement()->Attribute("Value"));

			FColor IntermediateColor;
			IntermediateColor = FColor::FromHex(ColorString);

			ColorOut = IntermediateColor;
		}
		else if (StructProperty->Struct->GetFName() == NAME_Color)
		{
			FColor& ColorOut = *(FColor*)OutValue;
			FString ColorString(XmlNode->ToElement()->Attribute("Value"));

			ColorOut = FColor::FromHex(ColorString);
		}*/
		else if (StructProperty->Struct->GetCppStructOps() && StructProperty->Struct->GetCppStructOps()->HasImportTextItem())
		{
			UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();

			FString ImportTextString(XmlNode->ToElement()->Attribute("Value"));
			const TCHAR* ImportTextPtr = *ImportTextString;
			if (!TheCppStructOps->ImportTextItem(ImportTextPtr, OutValue, PPF_None, nullptr, (FOutputDevice*)GWarn))
			{
				// Fall back to trying the tagged property approach if custom ImportTextItem couldn't get it done
				Property->ImportText(ImportTextPtr, OutValue, PPF_None, nullptr);
			}
		}
		else
		{
			if (!XmlObjectToUStruct(XmlNode, StructProperty->Struct, OutValue, CheckFlags & (~CPF_ParmFlags), SkipFlags))
			{
				UE_LOG(LogSimpleXML, Error, TEXT("JsonValueToUProperty - FJsonObjectConverter::JsonObjectToUStruct failed for property %s"), *Property->GetNameCPP());
				return false;
			}
		}
	} 
	else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		FString ImportTextString(XmlNode->ToElement()->Attribute("Value"));
		const TCHAR* ImportTextPtr = *ImportTextString;
		if (Property->ImportText(ImportTextPtr, OutValue, 0, NULL) == NULL)
		{
			UE_LOG(LogSimpleXML, Error, TEXT("XmlNodeToUProperty - Unable import property type %s from string value for property %s"), *Property->GetClass()->GetName(), *Property->GetNameCPP());
			return false;
		}
		/*UObject* Outer = GetTransientPackage();
		if (StructDefinition->IsChildOf(UObject::StaticClass()))
		{
			Outer = (UObject*)OutStruct;
		}*/

			////TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
			//UClass* PropertyClass = ObjectProperty->PropertyClass;

			//// If a specific subclass was stored in the Json, use that instead of the PropertyClass
			//FString ClassString = Obj->GetStringField(ObjectClassNameKey);
			//Obj->RemoveField(ObjectClassNameKey);
			//if (!ClassString.IsEmpty())
			//{
			//	UClass* FoundClass = FindObject<UClass>(ANY_PACKAGE, *ClassString);
			//	if (FoundClass)
			//	{
			//		PropertyClass = FoundClass;
			//	}
			//}

			//UObject* createdObj = StaticAllocateObject(PropertyClass, Outer, NAME_None, EObjectFlags::RF_NoFlags, EInternalObjectFlags::None, false);
			//(*PropertyClass->ClassConstructor)(FObjectInitializer(createdObj, PropertyClass->ClassDefaultObject, false, false));

			//ObjectProperty->SetObjectPropertyValue(OutValue, createdObj);

			//check(Obj.IsValid()); // should not fail if Type == EJson::Object
			//if (!XmlNodeToUProperty(Obj->Values, PropertyClass, createdObj, PropertyClass, createdObj, StructDefinition, OutStruct, CheckFlags & (~CPF_ParmFlags), SkipFlags))
			//{
			//	UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - FJsonObjectConverter::JsonObjectToUStruct failed for property %s"), *Property->GetNameCPP());
			//	return false;
			//}
		}
	//	else if (JsonValue->Type == EJson::String)
	//	{
	//		// Default to expect a string for everything else
	//		if (Property->ImportText(*JsonValue->AsString(), OutValue, 0, NULL) == NULL)
	//		{
	//			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable import property type %s from string value for property %s"), *Property->GetClass()->GetName(), *Property->GetNameCPP());
	//			return false;
	//		}
	//}
	else
	{
		FString ImportTextString(XmlNode->ToElement()->Attribute("Value"));
		const TCHAR* ImportTextPtr = *ImportTextString;
		// Default to expect a string for everything else
		if (Property->ImportText(ImportTextPtr, OutValue, 0, NULL) == NULL)
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable import property type %s from string value for property %s"), *Property->GetClass()->GetName(), *Property->GetNameCPP());
			return false;
		}
	}

	return true;
}

bool FXmlObjectConverter::XmlFileToUStruct(const FString& XMLFilePath, const UStruct* StructDefinition, void* OutStruct, int64 CheckFlags /*= 0*/, int64 SkipFlags /*= 0*/)
{
	tinyxml2::XMLDocument Doc;
	Doc.LoadFile(TCHAR_TO_ANSI(*XMLFilePath));
	if (Doc.Error())
	{
		return false;
	}
	if (!FXmlObjectConverter::XmlObjectToUStruct(Doc.RootElement(), StructDefinition, OutStruct, CheckFlags, SkipFlags))
	{
		return false;
	}
	return true;
}
