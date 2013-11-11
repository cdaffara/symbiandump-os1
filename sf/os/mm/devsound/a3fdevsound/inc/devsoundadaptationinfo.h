// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DEVSOUNDADAPTATIONINFO_H
#define DEVSOUNDADAPTATIONINFO_H

/**
@publishedPartner 
@released
@file
*/

//  INCLUDES

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3ffourcclookup.h>

class MA3FDevSoundAdaptationInfoObserver;

const TUid KTruePauseCustomInterfaceTypeUid = {0x1028643F};

/**
Custom interface class for query a3f adaptation about if resume is supported
*/
class MTruePauseCustomInterface
	{
public:
	/**
	Returns ETrue when True Pause is supported.
	*/ 
	virtual TBool IsResumeSupported(TUid aCodecType, TUid aFormat) = 0;
	};


// CLASS DECLARATION

/**
An interface to a A3FDevsoundAdaptationInfo.
A3FDevSoundAdaptationInfo is a replacable component that should correspond to the underlying
A3F adaptation, and provides information to the DevSound Adaptor information it cannot otherwise
obtain. If the underlying A3F adaptation changes, then potentially so should the implementation of
A3FDevSoundAdaptationInfo. Additionally the MaxGain and MaxVolume values should be set to reflect
those required for the specific product.
@lib devsoundadaptationinfo.lib
*/
class CA3FDevSoundAdaptationInfo : public CBase
	{
public:
	/**
	Constructs and returns a pointer to a new CA3FDevSoundAdaptationInfo object.
	Leaves on failure.
	@return CA3FDevSoundAdaptationInfo* - on success, pointer to new class instance.
	This call returns ownership, as in the standard NewL() pattern
	*/
	IMPORT_C static CA3FDevSoundAdaptationInfo* NewL(MA3FDevSoundAdaptationInfoObserver& aAdaptationInfoObserver, CFourCCConvertor& aFourCcConvertor);

	/**
	Destructor.
	*/
	~CA3FDevSoundAdaptationInfo();

	/**
	Requests Maximun Valid Gain/Volume. This is an asynchronous call. If error return is KErrNone,
	the result will be returned in subsequent RequestMaxGainComplete() call.
	@param aCodecType KUidAudioEncoder for MaxGain, KUidAudioDecoder for MaxVolume
	@return KErrNone if successfull, else corresponding error code
	*/
	virtual TInt RequestMaxGain(TUid aCodecType)=0;


	/**
	Requests a list of supported FourCC codes. This is an asynchronous call. If error return is KErrNone,
	the completion will be indicated by RequestSupportedFormatsComplete().
	@param aCodecType KUidAudioEncoder for supported encoder formats, KUidAudioDecoder for supported decoder formats
	@param aSupportedFormats The results are appended to this array.
	@return KErrNone if successfull, else corresponding error code
	*/
	virtual TInt RequestSupportedFormats(TUid aCodecType, RArray<TUid>& aSupportedFormats)=0;

	/**
	Queries A3F adaptation about if True Pause is supported 
	the completion will be indicated by RequestSupportedFormatsComplete().
	@param aCodecType KUidAudioEncoder for encoder format, KUidAudioDecoder for decoder format
	@param aFormat The format to be queried
	@return ETrue if True Pause is supported, EFalse otherwise
	*/
	inline TBool IsResumeSupported(TUid aCodecType, TUid aFormat);


protected:
	/**
	Constructor
	*/
	CA3FDevSoundAdaptationInfo();
	};

inline TBool CA3FDevSoundAdaptationInfo::IsResumeSupported(TUid aCodecType, TUid aFormat)
	{
	TBool supported = EFalse;
	
	// Extension pattern
	TAny* interface = NULL;
	TInt err = Extension_(KTruePauseCustomInterfaceTypeUid.iUid, interface, NULL); 
	if(err == KErrNone)
		{
		MTruePauseCustomInterface* truePause = static_cast<MTruePauseCustomInterface*>(interface);
		supported = truePause->IsResumeSupported(aCodecType, aFormat);
		}	
	return supported;
	}

#endif // DEVSOUNDADAPTATIONINFO_H

//end of file

