// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsteststepparsingbase.cpp
// 
//

#include <e32math.h>
#include "ctlbsteststepparsingbase.h"

CT_LbsTestStepParsingBase::~CT_LbsTestStepParsingBase()
	{
	}

CT_LbsTestStepParsingBase::CT_LbsTestStepParsingBase()
	{
	}

TBool CT_LbsTestStepParsingBase::GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint32& aResult)
	{
	// ok we have to have this because CTestStep is brain dead. It does have a
	// GetHexFromConfig method which you should be able to use, but some
	// "child" doesn't understand Hex. Why they think the method should only 
	// return the anser in an _signed_ int is beyond me. Esp. when they use
	// TLex.Val(TUint32, EHex) internaly. Which you would think _might_ be
	// be a hint to a mistake. Anyhow the broken method is both exported, and 
	// there insufficent access avaiable to the class members to be able
	// to overload this method :(. So we have to roll our own methods (again)
	const TInt KMaxHexStringSize = 10;// enough for 0x12345678
	TBuf<KMaxHexStringSize> hexBuffer;
	TPtrC ptr;
	ptr.Set(hexBuffer);
	if(GetStringFromConfig(aSectName, aKeyName, ptr))
		{
		// strip any leading "0x" or "0X"
		if(ptr[0] == TChar('0') && (ptr[1] == TChar('X') || ptr[1] == TChar('x')))
			{
			// leading "0x" encoded. So strip
			hexBuffer = ptr.Right(ptr.Length()-2);
			}
		else
			{
			hexBuffer = ptr;
			}
		TLex lex(hexBuffer);
		TInt res = lex.Val(aResult, EHex);
		if(res != KErrNone)
			{
			return EFalse;
			}
		return ETrue;	
		}
	else
		{
		// can't find the hex string keyword
		return EFalse;
		}

	}
	

TBool CT_LbsTestStepParsingBase::GetAGPSModeFromConfig(const TDesC& aSectName, const TDesC& aKeyName, CLbsAdmin::TGpsMode& aAGPSMode)
	{
	TInt agpsModeValue;
	if(GetIntFromConfig(aSectName, aKeyName, agpsModeValue))
		{
		switch(agpsModeValue)
			{
			case 1:
				aAGPSMode = CLbsAdmin::EGpsPreferTerminalBased;
				break;
			case 2:
				aAGPSMode = CLbsAdmin::EGpsAlwaysTerminalBased;
				break;
			case 3:
				aAGPSMode = CLbsAdmin::EGpsAutonomous;
				break;
			default:
			// otherwise the mode is unkown (which is illegal) or not currently suported
				return EFalse;
			}
		// if we make it here the value is ok
		return ETrue;
		}
	else
		{
		// can't find the key string for some reason
		return EFalse;
		}
	}

// parse a single position from the ini file, missing components are optional..
TBool CT_LbsTestStepParsingBase::GetReferencePositionFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TPosition& aPos)
	{
//	TPosition pos;
	TPtrC ptr;
	if(GetStringFromConfig(aSectName, aKeyName, ptr))
		{
		// ok we have the string, now work out the position
		//$position,<number of copies>,
		CDesCArrayFlat* fields = new(ELeave) CDesCArrayFlat(4);
		CleanupStack::PushL(fields);
		TChar comma(',');
		SplitL(ptr, fields, comma);
		DefaultData(aPos); // initially fill with default data - to that missing fields have a value
		ExtractDataL(aPos, fields);
		CleanupStack::PopAndDestroy(fields);
		return ETrue;
		}
	else
		{
		// there is no position string in this config section
		// this might have been intentional
		return EFalse;
		}

// to be reworked in a new base class!
// for the moment hard code this....

//	aPos.SetCoordinate(52.2454,0.1668);
//	return ETrue;
// this has to be rewritten	
//	IMPORT_C void GetConfigured_PosInfosL(const TDesC& aConfigFileName, const TDesC& aConfigSection, RPointerArray<TAny>& aPosInfoArr);

	}


//-------------------------------------------------------------------------
void CT_LbsTestStepParsingBase::SplitL(const TDesC& aDes, CDesCArray* aFields, TChar aDelim)
	{
	// split the string based on a demiliter
	TInt length = aDes.Length();

	TUint16* words = reinterpret_cast<TUint16*>(User::AllocL(length));
	CleanupStack::PushL(words);
	TPtr p(words, length); // create a buffer that is s big a sthe line. No single element can be bigger than this :)
	TChar character;
	for(TInt i=0; i<length; ++i)
		{
		character = aDes[i];
		if(character == aDelim)
			{
			// this is the end of a sequence, so add it
			aFields->AppendL(p); // this takes a copy;
			p.Zero(); // nuke the buffer
			}
		else
			{			
			// if we get here its a valid character so add it	
			p.Append(character); // this can never fail
			}
		}
	// if we have any charcters left in p we need to append them to the array. 
	if(p.Length()>0)
		{
		aFields->AppendL(p); // this takes a copy
		}
	CleanupStack::PopAndDestroy(words);
	}
	
	
//----------------------- all copied from CPosInfoConfigReader/CConfigReader ---------------

void CT_LbsTestStepParsingBase::ExtractDataL(TPosition& aPos, CDesCArray* aFields)
	{
	const TInt EPosLatitude = 0;
	const TInt EPosLongitude = 1;
	const TInt EPosAltitude = 2;
	const TInt EPosHorzAccuracy = 3;
	const TInt EPosVertAccuracy = 4;
		
	TReal64 latitude;
	TReal64 longitude;
	TReal32 altitude;
	// TODO: TUid datum;
	TReal32 horzAccuracy;
	TReal32 vertAccuracy;

	TInt fieldCount = aFields->Count();
	for(TInt i=0; i < fieldCount; ++i)
		{
		switch(i)
			{
		case 0:
			ExtractValueL(aFields->MdcaPoint(EPosLatitude), latitude);
			aPos.SetCoordinate(latitude, aPos.Longitude(), aPos.Altitude());
			break;
		case 1:
			ExtractValueL(aFields->MdcaPoint(EPosLongitude), longitude);
			aPos.SetCoordinate(aPos.Latitude(), longitude, aPos.Altitude());
			break;
		case 2:
			ExtractValueL(aFields->MdcaPoint(EPosAltitude), altitude);
			aPos.SetCoordinate(aPos.Latitude(), aPos.Longitude(), altitude);
			break;
		case 3:
			ExtractValueL(aFields->MdcaPoint(EPosHorzAccuracy), horzAccuracy);
			aPos.SetHorizontalAccuracy(horzAccuracy);
			break;
		case 4:
			ExtractValueL(aFields->MdcaPoint(EPosVertAccuracy), vertAccuracy);
			aPos.SetVerticalAccuracy(vertAccuracy);
			break;
		default:
			// ignore any others...
			break;
			}
		}
	}



void CT_LbsTestStepParsingBase::DefaultData(TPosition& aPos)
	{
	const TReal64 KDefaultLatitude(49.2);
	const TReal64 KDefaultLongitude(3.5);
	const TReal32 KDefautAltitude(50.0);
	const TReal32 KDefaultHorizontalAccuracy(2.0);
	const TReal32 KDefaultVerticalAcciracy(3.0);

	aPos.SetCoordinate(KDefaultLatitude, KDefaultLongitude, KDefautAltitude);
	aPos.SetAccuracy(KDefaultHorizontalAccuracy, KDefaultVerticalAcciracy);
	aPos.SetCurrentTime();
	}
	
void CT_LbsTestStepParsingBase::ExtractValueL(const TDesC& aField, TInt& aValue)
	{
	if (aField.Length() != 0)
		{
		TLex lex(aField);
		TInt err = lex.Val(aValue);
		User::LeaveIfError(err);
		}
	}


void CT_LbsTestStepParsingBase::ExtractValueL(const TDesC& aField, TReal32& aValue)
	{
	TRealX nan;
	nan.SetNaN();
	aValue = nan;
	if (aField.Length() != 0)
		{
		TLex lex(aField);
		TInt err = lex.Val(aValue);
		User::LeaveIfError(err);
		}
	}

void CT_LbsTestStepParsingBase::ExtractValueL(const TDesC& aField, TReal64& aValue)
	{
	TRealX nan;
	nan.SetNaN();
	aValue = nan;
	if (aField.Length() != 0)
		{
		TLex lex(aField);
		TInt err = lex.Val(aValue);
		User::LeaveIfError(err);
		}
	}

