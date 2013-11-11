/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/**
@InternalComponent
@released
*/
#ifndef __LBSMODULEINFOPARSER_H__
#define __LBSMODULEINFOPARSER_H__

#include <f32file.h>

#include <e32base.h>
#include <lbscommon.h>

const TInt	KIniLoadMaxFileName = 256;
const TInt	KIniLoadMidFileName = 128;

class CLbsIniFileReader : public CBase 
	{
public:
	static CLbsIniFileReader* NewL(RFs& aFs, const TDesC& aIniLoadFileName);
	virtual ~CLbsIniFileReader();

	TBool SetNextPoint();	
	TInt  GetDataPoint();	
	TInt  GetDataIndex();	
	void  ResetPoint();
	TBool FindNextSection();

	static TBool CheckExistFile(TDesC& aDirPath, TDesC& aFileName);

	TBool FindVar(const TDesC& aVarName, TPtrC& aResult) const;
	TBool FindVar(const TDesC& aVarName, TInt& aResult) const;	
	TBool FindVar(const TDesC& aVarName, TVersion& aResult) const;
//	TBool FindVar(const TDesC& aVarName, TUint& aResult, TRadix aRadix) const;
//	TBool FindVar(const TDesC& aVarName, TReal64& aResult) const;
	TBool FindVar(const TDesC& aVarName, TReal32& aResult) const;
	TBool FindVar(const TDesC& aVarName, TInt64& aResult) const;

public:
    void ReadPosQualityCriteria(TPositionQuality& aPosQuality);
    void ReadModuleInfoL(TPositionModuleInfo& aModuleInfo);
    void ReadPlugInUid(TUid& aDataSourcePluginId);
    void ReadExecutableNameL(TDes& aExecutableName);
    void ReadDeviceCapabilities(TPositionModuleInfoExtended::TDeviceGpsModeCapabilities& aDeviceCapabilities);


protected:
	CLbsIniFileReader(const TDesC& aIniLoadFileName);
	void ConstructL(RFs& aFs);
	
private:
	TInt  DecimalToBitmask(TInt aDecimalValue);
    TInt  GetNextVersionPart(TLex& aLex) const;
	
protected:
	TPtr	iPtr;	
	TInt	iPoint;	
	TInt	iIndex;	

	TPtrC	iSection;
	TInt	iSectionEnd;

	TBuf<KIniLoadMaxFileName> iIniLoadFileName;	
	};

#endif // __LBSMODULEINFOPARSER_H__
