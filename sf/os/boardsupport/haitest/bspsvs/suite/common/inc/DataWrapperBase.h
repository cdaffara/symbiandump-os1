/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#if (!defined __DATA_WRAPPER_BASE__)
#define __DATA_WRAPPER_BASE__

//	EPOC includes
#include <datawrapper.h>
#define SECS_TO_MS(x)  (x*1000000)

typedef TBuf<KTEFMaxNameLength>	TIniDataName;

_LIT(KConsname,				"Test Console");

#define	GETFROMCONFIGOPTIONAL(aType, aSectName, aKeyName, aResult, aLogMessage)	\
		if ( !Get##aType##FromConfig(aSectName, aKeyName, aResult) )	\
			{															\
			WARN_PRINTF3(aLogMessage, &aKeyName, aResult);				\
			}

#define	GETFROMCONFIGMANDATORY(aType, aSectName, aKeyName, aResult, aLogMessage, aDataOk)	\
		if ( !Get##aType##FromConfig(aSectName, aKeyName, aResult) )	\
			{															\
			ERR_PRINTF2(aLogMessage, &aKeyName);						\
			SetBlockResult(EFail);										\
			aDataOk=EFalse;												\
			}

#define	GETSTRINGFROMCONFIGOPTIONAL(aSectName, aKeyName, aResult, aLogMessage)	\
		if ( !GetStringFromConfig(aSectName, aKeyName, aResult) )		\
			{															\
			WARN_PRINTF3(aLogMessage, &aKeyName, &aResult);				\
			}

#define	GETSTRINGFROMCONFIGMANDATORY(aSectName, aKeyName, aResult, aLogMessage, aDataOk)	\
		if ( !GetStringFromConfig(aSectName, aKeyName, aResult) )		\
			{															\
			ERR_PRINTF2(aLogMessage, &aKeyName);						\
			SetBlockResult(EFail);										\
			aDataOk=EFalse;												\
			}

class CDataWrapperBase : public CDataWrapper
	{
public:
	class TEnumEntryTable
		{
	public:
		const TDesC&	iString;
		TInt			iValue;
		};

public:
	TBool					GetBoolFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TBool& aResult);
	TBool					GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	TBool					GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult);
	TBool					GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	TBool					GetUintFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint& aResult);
	TBool					GetPointFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPoint& aResult);
	TBool					GetRectFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TRect& aResult);
	TBool					GetSizeFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TSize& aResult);
	TBool					GetEnumFromConfig(const TDesC& aSectName, const TDesC& aKeyName, const TEnumEntryTable* aTable, TInt& aResult);
	TBool					GetOrFromConfig(const TDesC& aSectName, const TDesC& aKeyName, const TEnumEntryTable* aTable, TUint& aResult);
	TBool 					GetUint8FromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint8& aResult);
	void 					Timedelay(TInt aTimeoutInSecs);
	virtual void			InitialiseL();
	inline RFs&				FileServer();
	inline CConsoleBase*	GetConsoleL();
	TBool					TranslateEnumToString(CDataWrapper& aDataWrapper, const TDesC& aEnum, const TInt aValue, TDesC& aTrasnaltion);
	TBool					TranslateNumberToEnumStringL(CDataWrapper& aDataWrapper, const TDesC& aSection, const TInt aValue, TDes& aTranslation);
	void 					PromptMessageL(const TDesC& aMsg,const TInt aDelayMiliSec,TKeyCode& aKeyCode);
	virtual TBool			DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CDataWrapperBase();
	virtual ~CDataWrapperBase();

	TBool	GetCommandStringParameter(const TDesC& aParameterName, const TDesC& aSection, TPtrC& aResult, TText8 *aFileName, TInt aLine, TBool aMandatory);
	TBool	GetCommandIntParameter(const TDesC& aParameterName, const TDesC& aSection, TInt& aResult, TText8 *aFileName, TInt aLine, TBool aMandatory);
	TBool	GetCommandBoolParameter(const TDesC& aParameterName, const TDesC& aSection, TBool& aResult, TText8 *aFileName, TInt aLine, TBool aMandatory);

private:
	TBool	ProcessString(const TDesC& aStr, TUint& aResult, const TEnumEntryTable* aTable);
	TBool	ProcessEntry(const TDesC& aStr, TUint& aResult, const TEnumEntryTable* aTable);
	TBool 	ProcessEnumList(CDataWrapper& aDataWrapper,	const TDesC& aStr, const TDesC& aEnum, const TUint& aValue, TDes& aTranslation);
	TBool	GetCommandStringParameterL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult);
	TBool 	KeyCheck();
	void 	DoCmdUtilityPromptMessageL(const TDesC& aSection);

private:
	// Included ini files
	RPointerArray<CIniData>	iInclude;
	RPointerArray<HBufC>	iBuffer;
	RFs                     iFs;
	RTimer					iTimer;
	};

#include "DataWrapperBase.inl"

#endif /* __DATA_WRAPPER_BASE__ */
