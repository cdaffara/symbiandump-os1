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

#include "t_testmwsinifile.h"
#include "t_logfile.h"

_LIT(KTestPluginName, "testmwsinifile");

_LIT(KWsIniFile, "z:\\system\\data\\wsini.ini");
_LIT(KString, "string");
_LIT(KInteger, "integer");

/***************************************************
CTestMWsIniFile
****************************************************/
/**
Creates a new CTestMWsIniFile object
*/ 
CTestMWsIniFile* CTestMWsIniFile::CreateL()
	{
	return new(ELeave) CTestMWsIniFile;
	}

/**
Initialisation phase of two phase construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
void CTestMWsIniFile::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);

	iWsIniFile = aEnv.ObjectInterface<MWsIniFile>();
	iIniData = CIniData::NewL(KWsIniFile, ' ');	//delimiter between key and value in wsini.ini
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("TestMWsIniFile is created."));		
	CleanupStack::PopAndDestroy(log);
	//runs tests which test the MWsIniFile interface
	RunMWsIniFileTestsL();
	}

/**
Constructor for CTestMWsIniFile
*/
CTestMWsIniFile::CTestMWsIniFile()
	{
	}

/**
Destructor for CTestMWsIniFile
*/
CTestMWsIniFile::~CTestMWsIniFile()
	{
	iWsIniFile = NULL;
    delete iIniData;
	}

/**
Tests which test the MWsIniFile interface 
*/
void CTestMWsIniFile::RunMWsIniFileTestsL()
	{
	_LIT(KDefaultInt, "defaultInt"); //default section data
	_LIT(KDefaultStr, "defaultStr");
	_LIT(KScreen0Int, "screen0Int");
	_LIT(KScreen0Str, "screen0Str");
	_LIT(KCustomSection, "TESTMWSINIFILE"); //custom section data
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
void CTestMWsIniFile::FindScreenVariableL(TInt aScreen, const TDesC& aIntSection, const TDesC& aStringSection)
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
void CTestMWsIniFile::FindCustomSectionVariableL(const TDesC& aSection, const TDesC& aIntSection, const TDesC& aStringSection)
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
void CTestMWsIniFile::FindDefaultVariableL(const TDesC& aIntSection, const TDesC& aStringSection)
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
void CTestMWsIniFile::FindDefaultMissingVariableL(const TDesC& aMissingVariable)
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
void CTestMWsIniFile::FindScreenMissingVariableL(TInt aScreen, const TDesC& aMissingVariable)
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
void CTestMWsIniFile::FindCustomSectionMissingVariableL(const TDesC& aSection, const TDesC& aMissingVariable)
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
void CTestMWsIniFile::FindDefaultVarOppositeMethodL(const TDesC& aAttribute, const TDesC& aType)
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
void CTestMWsIniFile::FindScreenVarOppositeMethodL(TInt aScreen, const TDesC& aAttribute, const TDesC& aType)
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
void CTestMWsIniFile::FindCustomSectionVarOppositeMethodL(const TDesC& aSection, const TDesC& aAttribute, const TDesC& aType)
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
void CTestMWsIniFile::FindNEScreenVariableL(TInt aScreen, const TDesC& aIntSection, const TDesC& aStringSection)
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
void CTestMWsIniFile::FindNESectionVariableL(const TDesC& aSection, const TDesC& aIntSection, const TDesC& aStringSection)
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
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestMWsIniFile::PluginName() const
	{
	return KTestPluginName;
	}
