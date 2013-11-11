// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// File:		DevSoundUtility.h
// Author:		Vasudevan Ramachandraiah
// Date:		July 16, 2002
// Class that provides API to list ECOM plugin implementation IDs
// (c) Nokia Inc.
// Revisions:
// Date:			Author			Description
// 
//

#ifndef __MMFDEVSOUNDUTILITY_H
#define __MMFDEVSOUNDUTILITY_H

#include <ecom/ecom.h>
#include <badesca.h>

#include "mmfutilities.h" // For TFourCC

#define KFourCCLength 4

class TMMFRawPackage
/**
*@internalTechnology
*/
	{
public:
	inline TPtr8& Package();
	inline const TPtr8& Package() const;
protected:
	TMMFRawPackage(TInt aDerivedSize);
	inline void SetSize(TInt aDerivedSize);
protected:
	TPtr8 iThis;
	};


class TMMFToneFixedSequenceNames : public TMMFRawPackage
/**
*@internalTechnology
*/
	{
public:
	inline TMMFToneFixedSequenceNames();
public:
	TInt iCount;
	};


class HMMFToneFixedSequenceNames : public TMMFToneFixedSequenceNames
/**
*@internalTechnology
*/
	{
public:
	HMMFToneFixedSequenceNames();
	HMMFToneFixedSequenceNames* AddNameL(const TDesC& aName);
	};


class CMMFDevSoundUtility : public CBase
/**
*internalTechnology
*/
	{
public:
	// Destructor
	~CMMFDevSoundUtility();
	static CMMFDevSoundUtility* NewL();
	static CMMFDevSoundUtility* NewLC();

	void SeekUsingFourCCL(TUid aInterfaceUid, RImplInfoPtrArray& aPlugInArray, const TFourCC& aSrcDataType, const TFourCC& aDstDataType, const TDesC& aPreferredSupplier);
	void SelectByPreference(RImplInfoPtrArray& aPlugInArray, const TDesC& aPreferredSupplier);
	void SeekHwDevicePluginsL(RArray<TFourCC>& aSupportedDataTypes, TMMFState aState);

#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	void FindHwDeviceAdapterL(TUid aInterfaceUid, RImplInfoPtrArray& aPlugInArray);
	void SeekCodecPluginsL(RArray<TFourCC>& aSupportedDataTypes, TMMFState aState, TBool aAppend);
#endif // SYMBIAN_MULTIMEDIA_CODEC_API

	void InitializeFixedSequenceL(CPtrC8Array** aFixedSequences);
	TBool RecognizeSequence(const TDesC8& aData);
	const TDesC& FixedSequenceName(TInt aSequenceNumber);

private:
	CMMFDevSoundUtility();
	void ConstructL();
	TBool IsDataTypeMatch(CImplementationInformation* hwDeviceResourceEntry,const TDesC8& fourCCPtrPCM16, TMMFState aState);

private:
	HMMFToneFixedSequenceNames* iFixedSequenceNames;
	HBufC8* iInfo;
	} ;

#endif // __MMFDEVSOUNDUTILITY_H
