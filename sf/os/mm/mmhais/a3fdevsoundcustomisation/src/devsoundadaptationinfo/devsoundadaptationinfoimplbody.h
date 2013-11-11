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
//

#ifndef DEVSOUNDADAPTATIONINFOIMPLBODY_H
#define DEVSOUNDADAPTATIONINFOIMPLBODY_H

#include <e32std.h>
#include <ecom/ecom.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3ffourcclookup.h>
#include <mmf/common/mmfutilities.h> // For TFourCC
#include "devsoundadaptationinfoimpl.h"

#define KFOURCCLENGTH 4

class MA3FDevSoundAdaptationInfoObserver;

/*
The real implementation of CA3FDevsoundAdaptationInfo.
*/
NONSHARABLE_CLASS(CA3FDevSoundAdaptationInfoImpl::CBody) : public CActive, public MTruePauseCustomInterface
	{
public:

	/*
	Factory
	*/
	static CBody* NewL(MA3FDevSoundAdaptationInfoObserver& aObserver, CFourCCConvertor& aFourCcConvertor);

	/*
	Destructor.
	*/
	~CBody();

	TInt RequestMaxGain(TUid aCodecType);
	TInt RequestSupportedFormats(TUid aCodecType, RArray<TUid>& aSupportedFormats);
	
	// From CActive
	TInt Extension_(TUint aExtensionId, TAny*& aExtPtr, TAny*);
	
	// from MTruePauseCustomInterface
	TBool IsResumeSupported(TUid aCodecType, TUid aFormat);

private:

	static TBool IsDataTypeMatch(TFourCC aFourCCPCM16,TFourCC aFourCC);

	void SeekHwDevicePluginsL(RArray<TFourCC>& aSupportedDataTypes, TUid aCodecType);

	void ReadSupportedFormatsL(TUid aCodecType, RArray<TUid>& aSupportedFormats);

	TBool FourCCExists(RArray<TFourCC>& aSupportedDataTypes,TFourCC aFourCCEntry);

	void ReadFourCC(CImplementationInformation* aHwDeviceResourceEntry, TFourCC& aFourCCLeft, TFourCC& aFourCCRight);

protected:
	void ConstructL(MA3FDevSoundAdaptationInfoObserver& aAdaptationInfoObserver, CFourCCConvertor& aFourCcConvertor);
	CBody();

	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:

	MA3FDevSoundAdaptationInfoObserver* iAdaptationInfoObserver;

	// Pointer to fourcc convertor
	// Not owned.
	CFourCCConvertor* iFourCcConvertor;

	TBool iMaxGainRequested;
	TBool iFormatsRequested;

	TUid iCodecType;
	};


#endif // DEVSOUNDADAPTATIONINFOIMPLBODY_H
