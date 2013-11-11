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
// Class to read and retrieve static module information
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include "lbsroot.h"
#include "lbsmoduleinfoparser.h"
#include "LbsExtendModuleInfo.h"

CLbsIniFileReader::CLbsIniFileReader
	(
	const TDesC& aIniLoadFileName
	)
	:
	iPtr(NULL, 0),
	iPoint(0),
	iIndex(1),
	iSection(NULL, 0),
	iSectionEnd(0)
	{
	iIniLoadFileName.Zero();
	iIniLoadFileName.Append(aIniLoadFileName);
	}

CLbsIniFileReader::~CLbsIniFileReader()
	{
	delete (TText*)iPtr.Ptr();
	}

CLbsIniFileReader* CLbsIniFileReader::NewL(RFs& aFs, const TDesC& aIniLoadFileName)
	{
	CLbsIniFileReader* self=new(ELeave) CLbsIniFileReader(aIniLoadFileName);
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop();
	return self;
	}

void CLbsIniFileReader::ConstructL(RFs& aFs)
	{
	TInt data_size =0;

	TAutoClose<RFile> file;
	User::LeaveIfError(file.iObj.Open(aFs, iIniLoadFileName, EFileStreamText|EFileRead));
	file.PushL();
	User::LeaveIfError(file.iObj.Size(data_size));

	TUint8* rawBuff = (TUint8*) User::AllocLC(data_size * sizeof(TText));
	TPtr8 rawTPtr(rawBuff, data_size * sizeof(TText));
	User::LeaveIfError(file.iObj.Read(rawTPtr));

	TText* dstPtr = (TText*) rawBuff;
	for(TInt i = data_size - 1; i >= 0; --i)
		{
		dstPtr[i] = rawBuff[i];
		/*
	 	 * 
		 * It's possible that values will be entered in any of the following
		 * formats :
		 * HorizontalAccuracy=20
		 * HorizontalAccuracy 20
		 * HorizontalAccuracy= 20
		 * HorizontalAccuracy = 20
		 * HorizontalAccuracy =20
		 *
		 * which is a bit of a headache for TLex parsing, as it deals with 
		 * character sequences. Fortunately, '=' isn't valid as part of any 
		 * of the settings, so the easiest thing to do is replace any 
		 * instances with a space, thus converting all the above formats into
		 * the first one.
		 */
		const TText KEqualsCharacter = '=';	
		const TText KSpaceCharacter = ' ';	
	 
		if(dstPtr[i] == KEqualsCharacter)
			{
			dstPtr[i] = KSpaceCharacter;	
			}
		}

	iPtr.Set((TText*) rawBuff, data_size, data_size);

	CleanupStack::Pop(rawBuff); 

	CleanupStack::PopAndDestroy();//file
	}

TBool CLbsIniFileReader::FindVar(const TDesC &aVarName, TPtrC &aResult) const
	{
	TInt pos = iSection.Find(TPtrC(aVarName));
	if (pos==KErrNotFound)
		{
		return(EFalse);
		}

	TLex lex(iSection.Mid(pos));
	lex.SkipCharacters();
	lex.SkipSpaceAndMark();
	lex.SkipCharacters();
	
	aResult.Set(lex.MarkedToken().Ptr(),lex.MarkedToken().Length());
	return(ETrue);
	}

TBool CLbsIniFileReader::FindVar(const TDesC &aVarName, TInt &aResult) const
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aVarName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}

TBool CLbsIniFileReader::FindVar(const TDesC &aVarName, TInt64 &aResult) const
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aVarName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}

TBool CLbsIniFileReader::FindVar(const TDesC &aVarName, TReal32 &aResult) const
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aVarName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}
	
TBool CLbsIniFileReader::FindVar(const TDesC &aVarName, TVersion &aVersion) const
	{
	TPtrC ptr(NULL,0);
	TUint number;
	if (FindVar(aVarName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(number)==KErrNone)
		    {
			aVersion.iMajor = number;
			aVersion.iMinor = GetNextVersionPart(lex);
			aVersion.iBuild = GetNextVersionPart(lex);
		    // Return OK even if only one number was found.    
			return(ETrue);
		    }
		}
	return(EFalse);
	}

TBool CLbsIniFileReader::SetNextPoint()
	{
	iIndex++;

	TBuf16<128> findText;

	findText.Zero();
	findText.Append(_L("["));
	findText.AppendNum(iIndex);
	findText.Append(_L("]"));

	iPoint = iPtr.Find(findText);

	if (iPoint == KErrNotFound)
		{
		return(EFalse);
		}

	return(ETrue);
	}

TInt  CLbsIniFileReader::GetDataPoint()
	{
	return	iPoint;
	}

TInt  CLbsIniFileReader::GetDataIndex()
	{
	return iIndex;
	}

void  CLbsIniFileReader::ResetPoint()
	{
	iPoint = 0;
	iIndex = 1;
	}

TBool CLbsIniFileReader::FindNextSection()
	{
	_LIT(KSectionHeaderStart, "[");
	_LIT(KSectionHeaderEnd, "]");
	
	// Clear the previous section information.
	iSection.Set(KNullDesC);

	// Early check to see if we have already searched 
	// to the end of the file.
	if (iSectionEnd == iPtr.Length())
		{
		return (EFalse);
		}

	// Begin searching from the end of the last section.
	TPtrC ptrSection = iPtr.Mid(iSectionEnd);
	
	// Search for the section header '[<text>]'.
	TInt sectionHeaderEnd(KErrNotFound);
	TInt sectionHeaderStart = ptrSection.Find(KSectionHeaderStart);
	if (sectionHeaderStart == KErrNotFound)
		{
		// No match, so no more sections in the file.
		iSectionEnd = iPtr.Length();
		return (EFalse);
		}
	else
		{
		sectionHeaderEnd = ptrSection.Find(KSectionHeaderEnd);
		if ((sectionHeaderEnd == KErrNotFound)
			|| (sectionHeaderEnd < sectionHeaderStart))
			{
			// Parsing error - the section header was not closed properly
			iSectionEnd = iPtr.Length();
			return (EFalse);
			}
		else if (sectionHeaderEnd == ptrSection.Length())
			{
			// Parsing error - the section header ends at the end
			// of the file, so the section is empty!
			iSectionEnd = iPtr.Length();
			return (EFalse);
			}
		}
	iSection.Set(ptrSection.Mid(sectionHeaderEnd + 1));
	
	// Search for the start of the next section 
	// - take this to be the end of the current section.
	TInt sectionEnd = iSection.Find(KSectionHeaderStart);
	if (sectionEnd != KErrNotFound)
		{
		iSection.Set(iSection.Left(sectionEnd));
		iSectionEnd += (sectionHeaderStart + sectionEnd + 1);
		}
	else
		{
		// No more sections after this one, so set the index to the end.
		iSectionEnd = iPtr.Length();
		}

	return (ETrue);
	}

//
// ModuleInfo specific functions

//
// READ QUALITY CRITERIA
//
    
_LIT(KTimeToFirstFixStr,     "TimeToFirstFix");	
_LIT(KTimeToNextFixStr,      "TimeToNextFix");	
_LIT(KHorizontalAccuracyStr, "HorizontalAccuracy");	
_LIT(KVerticalAccuracyStr,   "VerticalAccuracy");	
_LIT(KCostIndicatorStr,      "CostIndicator");	
_LIT(KPowerConsumptionStr,   "PowerConsumption");	
const TInt KMilliSecToMicroSecConverter = 1000;

void CLbsIniFileReader::ReadPosQualityCriteria(TPositionQuality& aPosQuality)
    {
    TInt    intValue;
    TInt64  int64Value;
    TReal32 real32Value;

	// Get the Time To First Fix
	FindVar(KTimeToFirstFixStr,int64Value);
	__ASSERT_ALWAYS( int64Value <= KMaxTInt, User::Panic(KLbsRootPanic, ELbsPanicBadFirstFixValue));
	if(int64Value>=0)
		{
		int64Value *= KMilliSecToMicroSecConverter;
		aPosQuality.SetTimeToFirstFix(int64Value);
		}
	else
		{
		aPosQuality.SetTimeToFirstFix(0);
		}

	// Get the Time To Next Fix
	FindVar(KTimeToNextFixStr,int64Value);
	__ASSERT_ALWAYS( int64Value <= KMaxTInt, User::Panic(KLbsRootPanic, ELbsPanicBadNextFixValue));
	if(int64Value>=0)
		{
		int64Value *= KMilliSecToMicroSecConverter;
		aPosQuality.SetTimeToNextFix(int64Value);
		}
	else
		{
		aPosQuality.SetTimeToNextFix(0);
		}
		
	// Get the Horizontal Accuracy
	FindVar(KHorizontalAccuracyStr,real32Value);
	if(real32Value>=0)
		{
		aPosQuality.SetHorizontalAccuracy(real32Value);
		}
	else
		{
		aPosQuality.SetHorizontalAccuracy(0);
		}

	// Get the Vertical Accuracy
	FindVar(KVerticalAccuracyStr,real32Value);
	if(real32Value>=0)
		{
		aPosQuality.SetVerticalAccuracy(real32Value);
		}
	else
		{
		aPosQuality.SetVerticalAccuracy(0);
		}
		
    // Get the cost indicator
	FindVar(KCostIndicatorStr,intValue);

	TPositionQuality::TCostIndicator costIndicator;
	switch(intValue)
		{
	    case 1: 
			costIndicator = TPositionQuality::ECostZero;
			break;
		case 2: 
			costIndicator = TPositionQuality::ECostPossible;
			break;
		case 3: 
			costIndicator = TPositionQuality::ECostCharge;
			break;
		default: 
			costIndicator = TPositionQuality::ECostUnknown;
			break;
		}
	aPosQuality.SetCostIndicator(costIndicator);

    // Get the power consumption 
    FindVar(KPowerConsumptionStr,intValue);

	TPositionQuality::TPowerConsumption powerConsumption;
	switch(intValue)
		{
		case 1: 
			powerConsumption = TPositionQuality::EPowerZero;
			break;
		case 2: 
			powerConsumption = TPositionQuality::EPowerLow;
			break;
		case 3: 
			powerConsumption = TPositionQuality::EPowerMedium;
			break;
		case 4: 
			powerConsumption = TPositionQuality::EPowerHigh;
			break;
		default:
			powerConsumption = TPositionQuality::EPowerUnknown;
			break;
		}

	aPosQuality.SetPowerConsumption(powerConsumption);
        
    }
    
    
_LIT(KTechnologyTypeStr,   				"TechnologyType");	
_LIT(KClassesSupportedStr, 				"ClassesSupported");	
_LIT(KCapabilitiesStr,     				"Capabilities");	
_LIT(KModuleNameStr,       				"ModuleName");	
_LIT(KModuleIdStr,         				"ModuleId");	
_LIT(KDeviceLocationStr,   				"DeviceLocation");	
_LIT(KVersionStr,          				"Version");	
_LIT(KDataSrcPluginIdStr,  				"DataSourceId");
_LIT(KExecutableNameStr,   				"ExecutableName");
_LIT(KDeviceGpsModeCapabilitiesStr,   		"DeviceGpsModeCaps");

const TInt KPositionModuleMaxNameLength = 50;


void CLbsIniFileReader::ReadPlugInUid(TUid& aDataSourcePluginId)
	{
    TInt    intValue=0;

	FindVar(KDataSrcPluginIdStr, intValue);
	
	if(intValue>0)
		{
		aDataSourcePluginId = (TUid::Uid(intValue));
		}
	else
		{
		aDataSourcePluginId = TUid::Uid(0);
		}
	
	}
	
	
 
	
void CLbsIniFileReader::ReadExecutableNameL(TDes& aExecutableName)
	{
 	TUint16* buffer = new (ELeave) TUint16[KPositionModuleMaxNameLength];
	CleanupStack::PushL(buffer);

	TPtrC16 namePtr(reinterpret_cast<const TUint16*>(buffer),KPositionModuleMaxNameLength);
    TBool success = FindVar(KExecutableNameStr, namePtr);
	
	TBuf<KPositionModuleMaxNameLength> executableName;

	if(success)
        {
        aExecutableName.Copy(namePtr);        	
        }
    else
        {
        aExecutableName.Copy(KNullDesC);        	
        }
        
      CleanupStack::PopAndDestroy(buffer);

	}
	
void CLbsIniFileReader::ReadDeviceCapabilities(TPositionModuleInfoExtended::TDeviceGpsModeCapabilities& aDeviceCapabilities)
	{
	//An ini file is expected to contain the line DeviceGpsModeCaps if this is missing, populate aDeviceCapabilities
	//We assume the value is TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB when .ini parameter is missing
	TInt intValue = 100; //EDeviceGpsModeSimultaneousTATB
    
	//An ini file is expected to contain the line DeviceGpsModeCaps
	//if this is missing, populate aDeviceCapabilities with default
 	FindVar(KDeviceGpsModeCapabilitiesStr, intValue);
   
	aDeviceCapabilities = DecimalToBitmask(intValue);
	}
	
	
	
/* Read in ModuleInfo from an .ini file
*/	
void CLbsIniFileReader::ReadModuleInfoL(TPositionModuleInfo& aModuleInfo)
    {
    TInt    intValue;
    TBool    success;

    // Technology-type - this is a binary bitmap.
    FindVar(KTechnologyTypeStr, intValue);
    TPositionModuleInfo::TTechnologyType technologyType = DecimalToBitmask(intValue);
    aModuleInfo.SetTechnologyType(technologyType);
    
    // Classes Supported - this is a binary bitmap, but support for each class 
    // has to be set individually.
    FindVar(KClassesSupportedStr, intValue);
    TInt supportedClasses = DecimalToBitmask(intValue);

    TPositionClassFamily currentClass = EPositionInfoFamily;
    while(currentClass <= EPositionUpdateOptionsFamily)
        {
        aModuleInfo.SetClassesSupported(currentClass, supportedClasses & 1); 
        supportedClasses >>= 1;
        currentClass = static_cast<TPositionClassFamily>(static_cast<TInt>(currentClass) + 1);   		
        }
    
    // Capabilities - this is a binary bitmap.
    FindVar(KCapabilitiesStr, intValue);
    TPositionModuleInfo::TCapabilities capabilities = DecimalToBitmask(intValue);
    aModuleInfo.SetCapabilities(capabilities);
    
    // Module Name
	TUint16* buffer = new (ELeave) TUint16[KPositionModuleMaxNameLength];
	CleanupStack::PushL(buffer);

	TPtrC16 modulePtr(reinterpret_cast<const TUint16*>(buffer),KPositionModuleMaxNameLength);
    success = FindVar(KModuleNameStr, modulePtr);

    TBuf<KPositionModuleMaxNameLength> moduleName;
    if(success)
        {
        moduleName.Copy(modulePtr);        	
        }
    else
        {
        moduleName.Copy(KNullDesC);        	
        }
    // Get rid of any delimiting " characters.
    TInt length = moduleName.Length();
    if(length > 0)
        {
    	TInt lastPos = length - 1;
    	TChar quote('\"');
        if(moduleName[lastPos] == quote)
            {
    	    moduleName.Delete(lastPos, 1);
            }
        if(modulePtr[0] == quote)
            {
    	    moduleName.Delete(0, 1);
            }     
        }
    aModuleInfo.SetModuleName(moduleName);
	CleanupStack::PopAndDestroy(buffer);

    // Module Id
    TUid moduleUid = TUid::Uid(0);
	FindVar(KModuleIdStr,intValue);
	if(intValue>0)
		{
		moduleUid = (TUid::Uid(intValue));
		}
	aModuleInfo.SetModuleId(moduleUid);

    // Module Version
    TVersion version(0,0,0);
	FindVar(KVersionStr,version);
	aModuleInfo.SetVersion(version);
    
    // Device Location
	TPositionModuleInfo::TDeviceLocation deviceLocation;
	FindVar(KDeviceLocationStr,intValue);
	switch(intValue)
		{
		case 1:
			deviceLocation = TPositionModuleInfo::EDeviceInternal;
			break;
		case 2: 
			deviceLocation = TPositionModuleInfo::EDeviceExternal;
			break;
		default: 
			deviceLocation = TPositionModuleInfo::EDeviceUnknown;
			break;
		}
	aModuleInfo.SetDeviceLocation(deviceLocation);
	
	// IsAvailable - to be read from .ini file but for now ...
	aModuleInfo.SetIsAvailable(ETrue);
	
    }
	    
// UTILITIES    
/**
 * Used for extracting the latter components of a TVersion from a string of the
 * form 1.2.3
 * Before reading checks that the end of the string has not yet been reached, 
 * then steps past the first character (assumed to be '.' and attempts to read
 * an integer from the following character(s).
 * Any error in version length or missing components (e.g. if the version is "1")
 * reports 0 for thet missing part(s).
 */
TInt CLbsIniFileReader::GetNextVersionPart(TLex& aLex) const
    {
    TInt number = 0;
    if(!aLex.Eos())
        {
        aLex.Inc(); 
        if(!aLex.Eos())
            {
		    aLex.Val(number);
            }
	    }
	return(number);
    }

TInt CLbsIniFileReader::DecimalToBitmask(TInt aDecimalValue)
    {
    TInt bitMask = 0;
    TInt bitCount = 0;
    /*
     * The bitmap "101" will be read in as the decimal value 101. 
     * Convert this to the binary value 0101 by masking.
     */
    while(aDecimalValue)
        {
        if(aDecimalValue & 1)
            {
    	    bitMask |= 1 << bitCount;
            }
        aDecimalValue /= 10;
        ++bitCount;
        }
	return(bitMask);
    }

