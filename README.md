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

FTestData Testdata;

FString XMlString;

1.Struct To XMLString

FXmlObjectConverter::UStructToXMLString(Testdata,XMlString);

2.XMLString To struct

FTestData OutData;

FString InXmlString;

FXmlObjectConverter::XmlObjectStringToUStruct(InXmlString, FTestData::StaticStruct(),&OutData);

FXmlObjectConverter::XmlObjectStringToUStruct(InXmlString, FTestData::StaticStruct(), &OutData);

FXmlObjectConverter::XmlStringToUStruct<FTestData>(InXmlString, &OutData);

3.Struct To Xml File

FXmlObjectConverter::SaveStructToXMLFile(Testdata, TEXT("D:/1.xml"));	

4.Xml File To Struct

FXmlObjectConverter::XmlFileToUStruct(TEXT("D:/1.xml"),&OutData);

