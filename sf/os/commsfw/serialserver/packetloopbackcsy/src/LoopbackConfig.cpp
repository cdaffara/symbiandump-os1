// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file handles retrieving settings from the config file for the loopback test driver
// 
//

/**
 @file
*/


#include <e32base.h>
#include <e32std.h>
#include <e32property.h>

#include <cs_port.h>
#include "SLOGGER.H"
#include <d32comm.h>
#include <c32comm.h>
#include <e32hal.h>

#include "LoopbackConfig.h"

const TUint KStdDelimiter=',';				// < Standard Delimiter for config file (a comma)
// config element indicies for elements common to both packet and serial mode
const TUint KFirstPortConfigElementIndex = 0;
const TUint KSecondPortConfigElementIndex = 1;
const TUint KDelayConfigElementIndex = 2;
// config element indicies for packet mode
const TUint KQueueLengthConfigElementIndex = 3;
const TUint KPacketLengthConfigElementIndex = 4;
// config element indicies for serial mode
const TUint KTotalBufferSizeConfigElementIndex = 3;

CLoopbackConfig* CLoopbackConfig::NewL()
	{
	CLoopbackConfig* me = new(ELeave) CLoopbackConfig();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CLoopbackConfig::CLoopbackConfig() : iConfigItems(NULL)
	{}
	
CLoopbackConfig::~CLoopbackConfig()
	{
	delete iConfigItems;
	}
	
void CLoopbackConfig::ConstructL()
	{	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	CTestConfig* configFile = CTestConfig::NewLC(fs, KConfigFileDir, KLoopbackConfigFilename);
	
	TBuf8<KMaxName> sectionName;
	TInt testNum = 0;
	CTestConfigSection* configSection = NULL;
	TBool foundSection = EFalse;
	if (KErrNone == GetTestNumber(testNum))
		{
		sectionName.Format(KSectionNameFormat, testNum);
		configSection = configFile->Section(sectionName);
		if(configSection != NULL)
			{
			foundSection = ETrue;
			}
		}
	if (!foundSection)
		{
		// use the [Defaults] section
		configSection = configFile->Section(KScriptDefaults);
		}
	if(configSection == NULL)
		{
		CleanupStack::PopAndDestroy(configFile);
		return;
		}
	
	// if retrieving the port type fails, we leave it at the default
	iPortType = EPacketLoopbackPortType;
	const CTestConfigItem* item = NULL;
	item = configSection->Item(KSerialLoopbackPortType);
	if (item != NULL)
		{
		iPortType = ESerialLoopbackPortType;
		}
	
	TUint portCount = configSection->ItemCount(KPort);
	// CArrayFixFlat doesn't like a count of 0 for some reason
	if (portCount == 0)
		portCount = 1;
	iConfigItems = new(ELeave) CArrayFixFlat<TLoopbackConfigItem>(portCount);
	for (TInt i = 0; i < configSection->ItemCount(KPort); i++)
		{
		TLoopbackConfigItem newItem;
		const CTestConfigItem* item = configSection->Item(KPort, i);
		if(item ==  NULL)
			continue;
		else
			{
			TInt elementValue = 0;
			TInt ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 
							KFirstPortConfigElementIndex, elementValue);
			if(ret == KErrNone)
				{
				newItem.iPortA = elementValue;
				}
			
			ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 
					KSecondPortConfigElementIndex, elementValue);
			if(ret == KErrNone)
				{
				newItem.iPortB = elementValue;
				}
			
			ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, KDelayConfigElementIndex, 
					elementValue);
			if(ret == KErrNone)
				{
				newItem.iDelay = elementValue;
				}
			
			if (ESerialLoopbackPortType == iPortType)
				{
				ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 
						KTotalBufferSizeConfigElementIndex, elementValue);
				if(ret == KErrNone)
					{
					newItem.iBufferSize = elementValue;
					}
				}
			else
				{
				ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 
						KQueueLengthConfigElementIndex, elementValue);
				if(ret == KErrNone)
					{
					newItem.iQueueLength = elementValue;
					}
				
				ret = CTestConfig::GetElement(item->Value(),KStdDelimiter, 
						KPacketLengthConfigElementIndex, elementValue);
				if(ret == KErrNone)
					{
					newItem.iPacketLength = elementValue;
					}
				}
			}
		iConfigItems->AppendL(newItem);
		}
	
	CleanupStack::PopAndDestroy(configFile);
    CleanupStack::PopAndDestroy(); // RFs handle
	}

/**
 @return The number of config items found
 */
TUint CLoopbackConfig::Count() const
	{
	return iConfigItems->Count();
	}
	
/**
 @return The configured port type - either packet or serial
 */
TPortType CLoopbackConfig::PortType() const
	{
	return iPortType;
	}
	
/**
 Retrieves the settings from the config file for the given port
 
 @param aUnit The port number to retrieve the settings for
 @param aPort This method assigns the settings to this object
 
 @return KErrNone if the port corresponding to aUnit is found, KErrNotFound otherwise
 */
TInt CLoopbackConfig::GetPortSettings(TUint aUnit, TLoopbackConfigItem& aPortSettings) const
	{
	for (TInt i = 0; i < iConfigItems->Count(); i++)
		{
		if ((iConfigItems->At(i).iPortA == aUnit) || (iConfigItems->At(i).iPortB == aUnit))
			{
			LOGTEXT2(_L8("Found port settings for unit = %d"), aUnit);
			aPortSettings = iConfigItems->At(i);
			return KErrNone;
			}
		}
	LOGTEXT2(_L8("Could not find port settings for unit = %d"), aUnit);
	return KErrNotFound;
	}

TInt CLoopbackConfig::GetTestNumber(TInt& aTestNumber)
/**
 * Retrieve the test number.
 * Use the test number corresonding the the publish and subsciber property KPSLoopbackCsyTestNumber
 */
	{
	TInt ret0 = RProperty::Get(KUidPSPacketLoopbackCsyCategory, KPSLoopbackCsyTestNumber, 
								aTestNumber);
	if (ret0 == KErrNone && aTestNumber >= 0)
		{
		LOGTEXT2(_L8("Got system property KPSLoopbackCsyTestNumber. testNumber=%d"), aTestNumber);
		return KErrNone;
		}
	return KErrNotFound;
	}

