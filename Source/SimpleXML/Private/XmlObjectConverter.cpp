// Fill out your copyright notice in the Description page of Project Settings.


#include "XmlObjectConverter.h"

XmlObjectConverter::XmlObjectConverter()
{
}

XmlObjectConverter::~XmlObjectConverter()
{
}

bool XmlObjectConverter::UPropertyToXMLString(const UStruct* StructDefinition, const void* Struct, FString& OutJsonString, int64 CheckFlags, int64 SkipFlags)
{
	bool bResult = false;
	const FString _XmlContent = "<DocumentElement>\n</DocumentElement>";

	FXmlFile XmlFile = { _XmlContent,EConstructMethod::ConstructFromBuffer };

	FXmlNode* RootNode = XmlFile.GetRootNode();

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
		FString VariName = Property->GetName();
		const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);
		FString StringValue = "";
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
				StringValue = FString::FormatAsNumber(NumericProperty->GetFloatingPointPropertyValue(Value));
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
			//TArray< TSharedPtr<FJsonValue> > Out;
			//FScriptArrayHelper Helper(ArrayProperty, Value);
			//for (int32 i = 0, n = Helper.Num(); i < n; ++i)
			//{
			//	TSharedPtr<FJsonValue> Elem = FJsonObjectConverter::UPropertyToJsonValue(ArrayProperty->Inner, Helper.GetRawPtr(i), CheckFlags & (~CPF_ParmFlags), SkipFlags, ExportCb, ArrayProperty);
			//	if (Elem.IsValid())
			//	{
			//		// add to the array
			//		Out.Push(Elem);
			//	}
			//}
		}
		else if (FSetProperty* SetProperty = CastField<FSetProperty>(Property))
		{
			//TArray< TSharedPtr<FJsonValue> > Out;
			//FScriptSetHelper Helper(SetProperty, Value);
			//for (int32 i = 0, n = Helper.Num(); n; ++i)
			//{
			//	if (Helper.IsValidIndex(i))
			//	{
			//		TSharedPtr<FJsonValue> Elem = FJsonObjectConverter::UPropertyToJsonValue(SetProperty->ElementProp, Helper.GetElementPtr(i), CheckFlags & (~CPF_ParmFlags), SkipFlags, ExportCb, SetProperty);
			//		if (Elem.IsValid())
			//		{
			//			// add to the array
			//			Out.Push(Elem);
			//		}

			//		--n;
			//	}
			//}
			//return MakeShared<FJsonValueArray>(Out);
		}
		else if (FMapProperty* MapProperty = CastField<FMapProperty>(Property))
		{
			/*TSharedRef<FJsonObject> Out = MakeShared<FJsonObject>();

			FScriptMapHelper Helper(MapProperty, Value);
			for (int32 i = 0, n = Helper.Num(); n; ++i)
			{
				if (Helper.IsValidIndex(i))
				{
					TSharedPtr<FJsonValue> KeyElement = FJsonObjectConverter::UPropertyToJsonValue(MapProperty->KeyProp, Helper.GetKeyPtr(i), CheckFlags & (~CPF_ParmFlags), SkipFlags, ExportCb, MapProperty);
					TSharedPtr<FJsonValue> ValueElement = FJsonObjectConverter::UPropertyToJsonValue(MapProperty->ValueProp, Helper.GetValuePtr(i), CheckFlags & (~CPF_ParmFlags), SkipFlags, ExportCb, MapProperty);
					if (KeyElement.IsValid() && ValueElement.IsValid())
					{
						FString KeyString;
						if (!KeyElement->TryGetString(KeyString))
						{
							MapProperty->KeyProp->ExportTextItem(KeyString, Helper.GetKeyPtr(i), nullptr, nullptr, 0);
							if (KeyString.IsEmpty())
							{
								UE_LOG(LogJson, Error, TEXT("Unable to convert key to string for property %s."), *MapProperty->GetName())
									KeyString = FString::Printf(TEXT("Unparsed Key %d"), i);
							}
						}

						Out->SetField(KeyString, ValueElement);
					}

					--n;
				}
			}

			return MakeShared<FJsonValueObject>(Out);*/
		}
		else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			//UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
			//// Intentionally exclude the JSON Object wrapper, which specifically needs to export JSON in an object representation instead of a string
			//if (StructProperty->Struct != FJsonObjectWrapper::StaticStruct() && TheCppStructOps && TheCppStructOps->HasExportTextItem())
			//{
			//	FString OutValueStr;
			//	TheCppStructOps->ExportTextItem(OutValueStr, Value, nullptr, nullptr, PPF_None, nullptr);
			//	OutValueStr);
			//}

			//TSharedRef<FJsonObject> Out = MakeShared<FJsonObject>();
			//if (FJsonObjectConverter::UStructToJsonObject(StructProperty->Struct, Value, Out, CheckFlags & (~CPF_ParmFlags), SkipFlags, ExportCb))
			//{
			//	return MakeShared<FJsonValueObject>(Out);
			//}
		}
		else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			// Instanced properties should be copied by value, while normal UObject* properties should output as asset references
			/*UObject* Object = ObjectProperty->GetObjectPropertyValue(Value);
			if (Object && (ObjectProperty->HasAnyPropertyFlags(CPF_PersistentInstance) || (OuterProperty && OuterProperty->HasAnyPropertyFlags(CPF_PersistentInstance))))
			{
				TSharedRef<FJsonObject> Out = MakeShared<FJsonObject>();

				Out->SetStringField(ObjectClassNameKey, Object->GetClass()->GetFName().ToString());
				if (FJsonObjectConverter::UStructToJsonObject(ObjectProperty->GetObjectPropertyValue(Value)->GetClass(), Object, Out, CheckFlags, SkipFlags, ExportCb))
				{
					TSharedRef<FJsonValueObject> JsonObject = MakeShared<FJsonValueObject>(Out);
					JsonObject->Type = EJson::Object;
					return JsonObject;
				}
			}
			else
			{
				FString StringValue;
				Property->ExportTextItem(StringValue, Value, nullptr, nullptr, PPF_None);
				StringValue;
			}*/
		}
		else
		{
			// Default to export as string for everything else
			Property->ExportTextItem(StringValue, Value, NULL, NULL, PPF_None);
		}

		RootNode->AppendChildNode(VariName, StringValue);

	}
	FString XmlPath = FPaths::GameSourceDir() + TEXT("1.xml");
	XmlFile.Save(XmlPath);
	return bResult;
}
