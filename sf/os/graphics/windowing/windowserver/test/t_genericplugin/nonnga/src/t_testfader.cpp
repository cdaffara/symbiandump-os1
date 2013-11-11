// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file
 @test
 @internalComponent
*/

#include "t_testfader.h"
#include "t_logfile.h"

_LIT(KTestFaderName, "testfader");
_LIT(KTestFaderDataName, "testfader_data");
_LIT(KTestFaderInvalidConstructName, "testfader_invalidconstruct");

_LIT(KWsIniFile, "z:\\system\\data\\wsini.ini");
_LIT(KString, "string");
_LIT(KInteger, "integer");

/***************************************************
CTestFader
****************************************************/
/**
Creates a new CTestFader object
*/ 
CTestFader* CTestFader::CreateL()
	{
	return new(ELeave) CTestFader;
	}

/**
Initialisation phase of two phase construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
void CTestFader::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	iBlackMap = 128;
	iWhiteMap = 255;
	iWsIniFile = aEnv.ObjectInterface<MWsIniFile>();
	iIniData = CIniData::NewL(KWsIniFile, ' ');	//delimiter between key and value in wsini.ini
	iEnv = &aEnv;
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("Testfader is created."));		
	CleanupStack::PopAndDestroy(log);
	//runs tests which test the MWsIniFile interface
	RunMWsIniFileTestsL();
	}

/**
Constructor for CTestFader
*/
CTestFader::CTestFader()
	{
	}

/**
Destructor for CTestFader
*/
CTestFader::~CTestFader()
	{
	iEnv = NULL;
	iWsIniFile = NULL;
    delete iIniData;
	}

/**
Sets the fade area. Test fading plugin gets the black and white map from the 
service plugin and simply uses bitgdi to perform fading
@param	aBitGc The bitmapped graphics context
@param	aRegion The region for fading
*/
void CTestFader::FadeArea(CFbsBitGc* aBitGc,const TRegion* aRegion)
	{
	//get fading parameters from service plugin
	__ASSERT_ALWAYS(iEnv->ObjectInterface<MWsFader>()==NULL, User::Invariant());
	MWsPluginManager* wsPluginManager = iEnv->ObjectInterface<MWsPluginManager>();
	__ASSERT_ALWAYS(static_cast<CTestServicePlugin*>(wsPluginManager->ResolvePluginInterface(KBadPluginUid))==NULL, User::Invariant());
	CTestServicePlugin* servicePlugin = static_cast<CTestServicePlugin*>(wsPluginManager->ResolvePluginInterface(KServicePluginUid));
	TInt err;
	TRAP(err,iWhiteMap = servicePlugin->GetWhiteMapL());
	if(err != KErrNone)
		{
		User::Panic(KLogFileName, err);
		}
	TRAP(err,iBlackMap = servicePlugin->GetBlackMapL());
	if(err != KErrNone)
		{
		User::Panic(KLogFileName, err);
		}
	aBitGc->Reset();
   	aBitGc->SetFadingParameters(iBlackMap,iWhiteMap);
	aBitGc->FadeArea(aRegion);	
	}

/**
Tests which test the MWsIniFile interface 
*/
void CTestFader::RunMWsIniFileTestsL()
	{
	_LIT(KDefaultInt, "defaultInt"); //default section data
	_LIT(KDefaultStr, "defaultStr");
	_LIT(KScreen0Int, "screen0Int");
	_LIT(KScreen0Str, "screen0Str");
	_LIT(KCustomSection, "TESTFADER"); //custom section data
	_LIT(KStdFaderInt, "stdfaderInt"); 
	_LIT(KStdFaderStr, "stdfaderStr");
	_LIT(KNeScreenInt, "NEScreenInt"); //nonexistent screen data (Screen5)
	_LIT(KNeScreenStr, "NEScreenStr");
	_LIT(KNeSection, "NESection"); //nonexistent section data
	_LIT(KNeSectionInt, "NESectionInt");
	_LIT(KNeSectionStr, "NESectionStr");
	_LIT(KMissingVar, "DoesNotExist");

	const TInt KScreen0 = 0;
	const TInt KNeScreen = 5;
	
	//
	//     Positive Tests     //
	//
	
	FindDefaultVariableL(KDefaultInt, KDefaultStr);  //Default Section
	FindScreenVariableL(KScreen0, KScreen0Int, KScreen0Str);  //Screen0 Section
	FindCustomSectionVariableL(KCustomSection, KStdFaderInt, KStdFaderStr);  //Custom Section

	//
	//     Negative Tests     //
	//
			
	//ACCESS MISSING VARIABLE
	FindDefaultMissingVariableL(KMissingVar);	
	FindScreenMissingVariableL(KScreen0,KMissingVar);
	FindCustomSectionMissingVariableL(KCustomSection,KMissingVar);
	
	//ACCESS INTEGER ATTRIBUTE WITH FindVar MEANT FOR STRING ATTRIBUTE
	FindDefaultVarOppositeMethodL(KDefaultInt, KInteger);
	FindScreenVarOppositeMethodL(KScreen0,KScreen0Int, KInteger);
	FindCustomSectionVarOppositeMethodL(KCustomSection,KStdFaderInt, KInteger);
	
	//ACCESS STRING ATTRIBUTE WITH FindVar MEANT FOR INTEGER ATTRIBUTE
	FindDefaultVarOppositeMethodL(KDefaultStr, KString);
	FindScreenVarOppositeMethodL(KScreen0,KScreen0Str, KString);
	FindCustomSectionVarOppositeMethodL(KCustomSection,KStdFaderStr, KString);
		
	//ACCESS ATTRIBUTE IN A SCREEN THAT DOES NOT EXIST
	FindNEScreenVariableL(KNeScreen, KNeScreenInt, KNeScreenStr);
	
	//ACCESS ATTRIBUTE IN A SECTION THAT DOES NOT EXIST
	FindNESectionVariableL(KNeSection, KNeSectionInt, KNeSectionStr);
	}

/**
Finds the variable from screen section
@param	aScreen The screen number
@param	aIntSection The key name of an integer value
@param	aStringSection The key name of an string value
*/
void CTestFader::FindScreenVariableL(TInt aScreen, const TDesC& aIntSection, const TDesC& aStringSection)
  	{
   	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
  	TInt intResult;
	TPtrC stringResult;
	TInt trueIntData;
	TPtrC trueStringData;
  	TBuf<255> screenSection;
  	screenSection.Format(_L("SCREEN%d"),aScreen);		
  	if(iWsIniFile->FindVar(aScreen,aIntSection))
		{
		if(iWsIniFile->FindVar(aScreen,aIntSection,intResult) && iIniData->FindVar(screenSection,aIntSection,trueIntData) && intResult == trueIntData)
			{
			iPrint.Format(_L("Screen Section has correct integer data: %d"), intResult);
			log->WriteToLogL(iPrint);				
			}
		else
			{
			log->WriteToLogL(_L("Screen integer data has incorrect value"));
			}		
		}
	else
		{
		iPrint.Format(_L("Screen%d Section does not have %S variable"), aScreen, &aIntSection);
		log->WriteToLogL(iPrint);
		}
	
	if(iWsIniFile->FindVar(aScreen,aStringSection))
		{
		if(iWsIniFile->FindVar(aScreen,aStringSection,stringResult) && iIniData->FindVar(screenSection,aStringSection,trueStringData) && !stringResult.Compare(trueStringData))
			{
			iPrint.Format(_L("Screen Section has correct string data: %S"), &stringResult);
			log->WriteToLogL(iPrint);
			}
		else
			{
			log->WriteToLogL(_L("Screen string data has incorrect value"));
			}
		}
	else
		{
		iPrint.Format(_L("Screen%d Section does not have %S variable"), aScreen, &aStringSection);
		log->WriteToLogL(iPrint);
		}
	CleanupStack::PopAndDestroy(log);
  	}

/**
Finds the variable from custom section
@param	aSection The name of the custom section
@param	aIntSection The key name of an integer value
@param	aStringSection The key name of an string value
*/ 
void CTestFader::FindCustomSectionVariableL(const TDesC& aSection, const TDesC& aIntSection, const TDesC& aStringSection)
  	{
  	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
  	TInt intResult;
	TPtrC stringResult;
	TInt trueIntData;
	TPtrC trueStringData;
  	if(iWsIniFile->FindVar(aSection,aIntSection))
		{
		if(iWsIniFile->FindVar(aSection,aIntSection,intResult) && iIniData->FindVar(aSection,aIntSection,trueIntData) && intResult == trueIntData)
			{
			iPrint.Format(_L("Custom Section has correct integer data: %d"), intResult);
			log->WriteToLogL(iPrint);
			}
		else
			{
			log->WriteToLogL(_L("Custom Section integer data has incorrect value"));
			}		
		}
	else
		{
		iPrint.Format(_L("Custom Section %S does not have %S variable"), &aSection, &aIntSection);
		log->WriteToLogL(iPrint);
		}
	
	if(iWsIniFile->FindVar(aSection,aStringSection))
		{
		if(iWsIniFile->FindVar(aSection,aStringSection,stringResult) && iIniData->FindVar(aSection,aStringSection,trueStringData) && !stringResult.Compare(trueStringData))
			{
			iPrint.Format(_L("Custom Section has correct string data: %S"), &stringResult);
			log->WriteToLogL(iPrint);
			}
		else
			{
			log->WriteToLogL(_L("Custom Section string data has incorrect value"));
			}
		}
	else
		{
		iPrint.Format(_L("Custom Section %S does not have %S variable"), &aSection, &aStringSection);
		log->WriteToLogL(iPrint);
		}
	CleanupStack::PopAndDestroy(log);
  	}

/**
Finds the variable from default section
@param	aIntSection The key name of an integer value
@param	aStringSection The key name of an string value
*/
void CTestFader::FindDefaultVariableL(const TDesC& aIntSection, const TDesC& aStringSection)
  	{
  	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
  	TInt intResult;
	TPtrC stringResult;
	TInt trueIntData;
	TPtrC trueStringData;
  	if(iWsIniFile->FindVar(aIntSection))
		{
		if(iWsIniFile->FindVar(aIntSection,intResult) && iIniData->FindVar(aIntSection, trueIntData) && intResult == trueIntData)
			{
			iPrint.Format(_L("Default Section has correct integer data: %d"), intResult);
			log->WriteToLogL(iPrint);				
			}
		else
			{
			log->WriteToLogL(_L("Default Section integer data has incorrect value"));
			}
		}
	else
		{
		iPrint.Format(_L("Default Section does not have %S variable"), &aIntSection);
		log->WriteToLogL(iPrint);
		}
		
	if(iWsIniFile->FindVar(aStringSection))
		{
		if(iWsIniFile->FindVar(aStringSection,stringResult) && iIniData->FindVar(aStringSection, trueStringData) && !stringResult.Compare(trueStringData))
			{
			iPrint.Format(_L("Default Section has correct string data: %S"), &stringResult);
			log->WriteToLogL(iPrint);
			}
		else
			{
			log->WriteToLogL(_L("Default Section string data has incorrect value"));
			}
		}
	else
		{
		iPrint.Format(_L("Default Section does not have %S variable"), &aStringSection);
		log->WriteToLogL(iPrint);
		}
	CleanupStack::PopAndDestroy(log);
  	}

/**
Finds a missing variable from default section
@param	aMissingVariable The key name of the missing variable.
*/
void CTestFader::FindDefaultMissingVariableL(const TDesC& aMissingVariable)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aMissingVariable))
		{		
		iPrint.Format(_L("Default Section has %S variable"), &aMissingVariable);
		log->WriteToLogL(iPrint);
		}
	if(iWsIniFile->FindVar(aMissingVariable,intResult))
		{
		iPrint.Format(_L("Default Section MissingVar Data: %d"), intResult);
		log->WriteToLogL(iPrint);			
		}
	else if(iWsIniFile->FindVar(aMissingVariable,stringResult))
		{
		iPrint.Format(_L("Default Section MissingVar Data: %S"), &stringResult);
		log->WriteToLogL(iPrint);
		}
	else
		{
		log->WriteToLogL(_L("Default Section does not have missing attribute"));
		}
	CleanupStack::PopAndDestroy(log);
	}

/**
Finds a missing variable from screen section
@param	aScreen	The screen number
@param	aMissingVariable The key name of the missing variable.
*/
void CTestFader::FindScreenMissingVariableL(TInt aScreen, const TDesC& aMissingVariable)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aScreen,aMissingVariable))
		{		
		iPrint.Format(_L("Screen%d Section has %S variable"), aScreen, &aMissingVariable);
		log->WriteToLogL(iPrint);
		}
	if(iWsIniFile->FindVar(aScreen,aMissingVariable,intResult))
		{
		iPrint.Format(_L("Screen%d Section MissingVar Data: %d"), aScreen, intResult);
		log->WriteToLogL(iPrint);			
		}
	else if(iWsIniFile->FindVar(aScreen,aMissingVariable,stringResult))
		{
		iPrint.Format(_L("Screen%d Section MissingVar Data: %S"), aScreen, &stringResult);
		log->WriteToLogL(iPrint);	
		}
	else
		{
		log->WriteToLogL(_L("Screen Section does not have missing attribute"));
		}
	CleanupStack::PopAndDestroy(log);
	}
/**
Finds a missing variable from custom section
@param	aSection The name of the custom section.
@param	aMissingVariable The key name of the missing variable.
*/
void CTestFader::FindCustomSectionMissingVariableL(const TDesC& aSection, const TDesC& aMissingVariable)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aSection,aMissingVariable))
		{		
		iPrint.Format(_L("Custom Section %S has %S variable"), &aSection, &aMissingVariable);
		log->WriteToLogL(iPrint);
		}
	if(iWsIniFile->FindVar(aSection,aMissingVariable,intResult))
		{
		iPrint.Format(_L("Custom Section %S MissingVar Data: %d"), &aSection, intResult);
		log->WriteToLogL(iPrint);			
		}
	else if(iWsIniFile->FindVar(aSection,aMissingVariable,stringResult))
		{
		iPrint.Format(_L("Custom Section %S MissingVar Data: %S"), &aSection, &stringResult);
		log->WriteToLogL(iPrint);	
		}
	else
		{
		log->WriteToLogL(_L("Custom Section does not have missing attribute"));
		}
	CleanupStack::PopAndDestroy(log);
	}

/**
Access integer attribute in default section with FindVar meant for string attribute
or access string attribute in default section with FindVar meant for integer attribute.
@param	aAttribute The name of the attribute to access
@param	aType The type of the attribute, integer or string.
*/
void CTestFader::FindDefaultVarOppositeMethodL(const TDesC& aAttribute, const TDesC& aType)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aAttribute))
		{
		if(!aType.Compare(KInteger))
			{			
			if(iWsIniFile->FindVar(aAttribute,stringResult))
				{
				iPrint.Format(_L("Default Section - Integer data retrieved with method for string attribute: %S"), &stringResult);
				log->WriteToLogL(iPrint);
				}
			else
				{
				log->WriteToLogL(_L("Default Section - Could not access integer attribute with method for string attribute"));
				}
			}
		if(!aType.Compare(KString))
			{
			if(iWsIniFile->FindVar(aAttribute,intResult))
				{
				iPrint.Format(_L("Default Section - String data retrieved with method for integer attribute: %d"), intResult);
				log->WriteToLogL(iPrint);
				}
			else
				{
				log->WriteToLogL(_L("Default Section - Could not access string attribute with method for integer attribute"));
				}
			}
		}
	else
		{
		iPrint.Format(_L("Default Section does not have %S variable"), &aAttribute);
		log->WriteToLogL(iPrint);
		}
	CleanupStack::PopAndDestroy(log);
	}

/**
Access integer attribute in screen section with FindVar meant for string attribute
or access string attribute in screen section with FindVar meant for integer attribute.
@param	aScreen The screen number
@param	aAttribute The name of the attribute to access
@param	aType The type of the attribute, integer or string.
*/
void CTestFader::FindScreenVarOppositeMethodL(TInt aScreen, const TDesC& aAttribute, const TDesC& aType)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aScreen,aAttribute))
		{
		if(!aType.Compare(KInteger))
			{			
			if(iWsIniFile->FindVar(aScreen, aAttribute,stringResult))
				{
				iPrint.Format(_L("Screen Section - Integer data retrieved with method for string attribute: %S"), &stringResult);
				log->WriteToLogL(iPrint);
				}
			else
				{
				log->WriteToLogL(_L("Screen Section - Could not access integer attribute with method for string attribute"));
				}
			}
		if(!aType.Compare(KString))
			{
			if(iWsIniFile->FindVar(aScreen, aAttribute,intResult))
				{
				iPrint.Format(_L("Screen Section - String data retrieved with method for integer attribute: %d"), intResult);
				log->WriteToLogL(iPrint);
				}
			else
				{
				log->WriteToLogL(_L("Screen Section - Could not access string attribute with method for integer attribute"));
				}
			}
		}
	else
		{
		iPrint.Format(_L("Screen%d Section does not have %S variable"), aScreen, &aAttribute);
		log->WriteToLogL(iPrint);
		}
	CleanupStack::PopAndDestroy(log);
	}

/**
Access integer attribute in custom section with FindVar meant for string attribute
or access string attribute in custom section with FindVar meant for integer attribute.
@param	aSection The name of the custom section
@param	aAttribute The name of the attribute to access
@param	aType The type of the attribute, integer or string.
*/
void CTestFader::FindCustomSectionVarOppositeMethodL(const TDesC& aSection, const TDesC& aAttribute, const TDesC& aType)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aSection,aAttribute))
		{
		if(!aType.Compare(KInteger))
			{			
			if(iWsIniFile->FindVar(aSection, aAttribute,stringResult))
				{
				iPrint.Format(_L("Custom Section - Integer data retrieved with method for string attribute: %S"), &stringResult);
				log->WriteToLogL(iPrint);
				}
			else
				{
				log->WriteToLogL(_L("Custom Section - Could not access integer attribute with method for string attribute"));
				}
			}
		if(!aType.Compare(KString))
			{
			if(iWsIniFile->FindVar(aSection, aAttribute,intResult))
				{
				iPrint.Format(_L("Custom Section - String data retrieved with method for integer attrigute: %d"), intResult);
				log->WriteToLogL(iPrint);
				}
			else
				{
				log->WriteToLogL(_L("Custom Section - Could not access string attribute with method for integer attribute"));
				}
			}
		}
	else
		{
		iPrint.Format(_L("Custom Section %S does not have %S variable"), &aSection, &aAttribute);
		log->WriteToLogL(iPrint);
		}
	CleanupStack::PopAndDestroy(log);
	}

/**
Finds variable from non existed screen section
@param	aScreen The screen number
@param	aIntSection The key name of an integer value
@param	aStringSection The key name of an string value
*/
void CTestFader::FindNEScreenVariableL(TInt aScreen, const TDesC& aIntSection, const TDesC& aStringSection)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aScreen,aIntSection))
		{		
		iPrint.Format(_L("Screen%d Section has %S variable"), &aScreen, &aIntSection);
		log->WriteToLogL(iPrint);
		}
	if(iWsIniFile->FindVar(aScreen,aIntSection,intResult))
		{
		iPrint.Format(_L("Screen%d Section Integer Data: %d"), &aScreen, intResult);
		log->WriteToLogL(iPrint);
		}		
	if(iWsIniFile->FindVar(aScreen,aStringSection,stringResult))
		{
		iPrint.Format(_L("Screen%d Section String Data: %S"), &aScreen, &stringResult);
		log->WriteToLogL(iPrint);
		}
	else
		{
		log->WriteToLogL(_L("Could not access variables because screen does not exist"));
		}
	CleanupStack::PopAndDestroy(log);
	}

/**
Finds variable from non existed custom section
@param	aSection The name of the custom section
@param	aIntSection The key name of an integer value
@param	aStringSection The key name of an string value
*/
void CTestFader::FindNESectionVariableL(const TDesC& aSection, const TDesC& aIntSection, const TDesC& aStringSection)
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	TInt intResult;
	TPtrC stringResult;
	if(iWsIniFile->FindVar(aSection,aIntSection))
		{		
		iPrint.Format(_L("NonExistent Section has %S variable"), &aIntSection);
		log->WriteToLogL(iPrint);
		}
	if(iWsIniFile->FindVar(aSection,aIntSection,intResult))
		{
		iPrint.Format(_L("NonExistent Section Integer Data: %d"), intResult);
		log->WriteToLogL(iPrint);
		}		
	if(iWsIniFile->FindVar(aSection,aStringSection,stringResult))
		{
		iPrint.Format(_L("NonExistent Section String Data: %S"), &stringResult);
		log->WriteToLogL(iPrint);
		}
	else
		{
		log->WriteToLogL(_L("Could not access variables because section does not exist"));
		}
	CleanupStack::PopAndDestroy(log);
	}
	
/**
Sets the fading parameters. Test fading plugin uses black/white map get from serviceplugin 
as fading parameters. It doesn't care what black/white map values are passed in.  
@param	aData The fading parameters
*/
void CTestFader::SetFadingParameters(const TDesC8& /*aData*/)
  	{
   	}

/**
Overides MWsObjectProvider. Resolve an instance of an interface
@param aTypeId The interface UID.
@return The pointer to the instance of an interface.
*/
TAny* CTestFader::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case MWsFader::EWsObjectInterfaceId:
			return static_cast<MWsFader*>(this);
		}
	return NULL;
	}

/**
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestFader::PluginName() const
	{
	return KTestFaderName;
	}

/***************************************************
CTestFader_Data
****************************************************/
/**
Creates a new CTestFader_Data object
@param	aData The construction parameters.
*/ 
CTestFader_Data* CTestFader_Data::CreateL(const TDesC& aData)
	{
	return new(ELeave) CTestFader_Data(aData);
	}

/**
The constructor of CTestFader_Data
@param	aData The construction parameters.
*/
CTestFader_Data::CTestFader_Data(const TDesC& aData)
	{
	iBlackMap = 128;
	iWhiteMap = 255;
	//testfader_data with fader option is set
	TPtrC faderOption(aData); 
	if (!faderOption.CompareF(_L("SET")))
		{
		iBlackMap = 255;
		iWhiteMap = 128;
		}
	}

/**
Initialisation phase of two phase construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
void CTestFader_Data::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	if (iBlackMap == 255 && iWhiteMap ==  128)
		{
		log->WriteToLogL(_L("Testfader_Data is created with fader option is set."));		
		}
	else
		{
		log->WriteToLogL(_L("Testfader_Data is created without fader option."));		
		}
	CleanupStack::PopAndDestroy(log);
	}

/**
Sets the fade area. Test fading plugin uses the constant black and white map 
set by constructor if fading option is Set, otherwise uses default black and 
whit map. It simply uses bitgdi to perform fading
@param	aBitGc The bitmapped graphics context
@param	aRegion The region for fading
*/
void CTestFader_Data::FadeArea(CFbsBitGc* aBitGc,const TRegion* aRegion)
	{
  	aBitGc->Reset();
   	aBitGc->SetFadingParameters(iBlackMap,iWhiteMap);
	aBitGc->FadeArea(aRegion);
	}

/**
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestFader_Data::PluginName() const
	{
	return KTestFaderDataName;
	}

/***************************************************
CTestFader_InvalidConstruct
****************************************************/
/**
Creates a new CTestFader_InvalidConstruct object
*/ 
CTestFader_InvalidConstruct* CTestFader_InvalidConstruct::CreateL()
	{
	return new(ELeave) CTestFader_InvalidConstruct;
	}

/**
Initialisation phase of two phase construction. It simply leaves during construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
void CTestFader_InvalidConstruct::ConstructL(MWsGraphicDrawerEnvironment& /*aEnv*/,const TDesC8& /*aData*/)
	{
	User::Leave(KErrGeneral);
	}

/**
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestFader_InvalidConstruct::PluginName() const
	{
	return KTestFaderInvalidConstructName;
	}
