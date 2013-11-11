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
// devsoundadaptationinfoimpl.cpp
//

// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh> // for KMmfUidPluginInterfaceHwDevice
#include <mm/mmcleanup.h> 						//needed for CleanupResetAndDestroyPushL()
#include "devsoundadaptationinfoimplbody.h"
#include "devsoundadaptationinfoconsts.h"
#include <a3f/mdevsoundadaptationinfoobserver.h>
#include <a3f/audioformatuids.h>

const TInt KHwDeviceDefaultDataLength= 9;


//-------------------------------------------------------------------
// 	CA3FDevSoundAdaptationInfoImpl::CA3FDevSoundAdaptationInfoImpl
//	C++ default constructor.
//-------------------------------------------------------------------
//
CA3FDevSoundAdaptationInfoImpl::CBody* CA3FDevSoundAdaptationInfoImpl::CBody::NewL(MA3FDevSoundAdaptationInfoObserver& aAdaptationInfoObserver, CFourCCConvertor& aFourCcConvertor)
	{
	CBody* self = new (ELeave) CBody;
	CleanupStack::PushL(self);
	self->ConstructL(aAdaptationInfoObserver, aFourCcConvertor);
	CleanupStack::Pop(self);
	return self;
	}

//-------------------------------------------------------------------
// 	CA3FDevSoundAdaptationInfoImpl::CA3FDevSoundAdaptationInfoImpl
//	C++ default constructor.
//-------------------------------------------------------------------
//
CA3FDevSoundAdaptationInfoImpl::CBody::CBody(): CActive(EPriorityStandard)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody()::CBody() *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CActiveScheduler::Add(this);
	DP_OUT();
	}

//----------------------------------------------------
// 	CA3FDevSoundAdaptationInfoImpl::ConstructL
// 	Symbian 2nd phase constructor can leave.
//----------------------------------------------------
//
void CA3FDevSoundAdaptationInfoImpl::CBody::ConstructL(MA3FDevSoundAdaptationInfoObserver& aAdaptationInfoObserver, CFourCCConvertor& aFourCcConvertor)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iAdaptationInfoObserver = &aAdaptationInfoObserver;
	iFourCcConvertor = &aFourCcConvertor;
	DP_OUT();
	}

//--------------------------------------------------------------
//	CA3FDevSoundAdaptationInfoImpl::~CA3FDevSoundAdaptationInfoImpl
//	Destructor
//--------------------------------------------------------------
//
CA3FDevSoundAdaptationInfoImpl::CBody::~CBody()
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::~CBody *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

//---------------------------------------------------------
// 	CA3FDevSoundAdaptationInfoImpl::CBody::RequestMaxGain
// 	Retrieves Maximum Valid Gain/Volume
//---------------------------------------------------------
//
TInt CA3FDevSoundAdaptationInfoImpl::CBody::RequestMaxGain(TUid aCodecType)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::RequestMaxGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iCodecType = aCodecType;
	iMaxGainRequested = ETrue;
	
	// Simulate an asyncronous response
	if (!IsActive())
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}

	DP0_RET(KErrNone, "%d");
	}

//-----------------------------------------------------
// 	CA3FDevSoundAdaptationInfoImpl::CBody::RequestSupportedFormatsL
//  Retrieves supported formats (in this version hwDevices).
//-----------------------------------------------------
//
TInt CA3FDevSoundAdaptationInfoImpl::CBody::RequestSupportedFormats(TUid aCodecType, RArray<TUid>& aSupportedFormats)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::RequestSupportedFormats *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err=KErrNone;
	
	iCodecType = aCodecType;
	
	TRAP(err, ReadSupportedFormatsL(aCodecType, aSupportedFormats));
	
	if(err==KErrNone)
		{
		iFormatsRequested = ETrue;
	
		// Simulate an asyncronous response
		if (!IsActive())
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
			}
		}
				
	DP0_RET(err, "%d");
	}


TInt CA3FDevSoundAdaptationInfoImpl::CBody::Extension_(TUint aExtensionId, TAny*& aExtPtr, TAny*)
	{
	if (aExtensionId == KTruePauseCustomInterfaceTypeUid.iUid)
		{
		aExtPtr = static_cast<MTruePauseCustomInterface*>(this);
		return KErrNone;
		}
	else
		{
		return CActive::Extension_(aExtensionId, aExtPtr, NULL);
		}
	}

// from MTruePauseCustomInterface
// ---------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::CBody::IsResumeSupported
// ---------------------------------------------------------------------------
TBool CA3FDevSoundAdaptationInfoImpl::CBody::IsResumeSupported(TUid aCodecType, TUid aFormat)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::IsTruePauseSupported *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	
	const TUid supportedDecoders [] = 
		{
		KUidFormatPCM16, 
		KUidFormatPCMU16B, 
		KUidFormatPCMU16,
		KUidFormatPCM16B, 
		KUidFormatPCM8, 
		KUidFormatPCMU8, 
		KUidFormatAlaw,
		KUidFormatMulaw,
		KUidFormatGSM610,
		KUidFormatIMAADPCM,
		KUidFormatTone,
		KNullUid,  // Indicates end of array
		};
	
	TBool supported = EFalse;

	// PCM16 Encoder
	if(aFormat == KUidFormatPCM16 && aCodecType == KUidAudioEncoder)
		{
		supported = ETrue;
		}
	// Only certain decoders
	else if(aCodecType == KUidAudioDecoder)
		{
		TInt i = 0;
		TUid supportedDecoderUid = supportedDecoders[i];
		do
			{
			if (supportedDecoderUid == aFormat)
				{
				supported = ETrue;
				break;
				}
			supportedDecoderUid = supportedDecoders[i++];
			}
		while(supportedDecoderUid != KNullUid);
		}
	DP0_RET(supported, "%d");
	}

//From CActive
// ---------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::CBody::RunL
// ---------------------------------------------------------------------------
void CA3FDevSoundAdaptationInfoImpl::CBody::RunL()
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::RunL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;
	
	if(iMaxGainRequested)
		{
		
		TInt maxgain = 0;
	
		if (iCodecType == KUidAudioDecoder)
			{
			maxgain = DevSoundAdaptationInfoConsts::MaxVolume();
			}
		else if (iCodecType == KUidAudioEncoder)
			{
			maxgain = DevSoundAdaptationInfoConsts::MaxGain();
			}
		else
			{
			err = KErrArgument;
			}

		iMaxGainRequested = EFalse;
		
		// Send the callback
		iAdaptationInfoObserver->RequestMaxGainComplete(iCodecType, err, maxgain);
		
		}

	if(iFormatsRequested)
		{
		err = KErrNone;
	
		if(iCodecType != KUidAudioDecoder && iCodecType != KUidAudioEncoder)
			{
			err = KErrArgument;
			}

		iFormatsRequested = EFalse;
		
		// Send the callback
		iAdaptationInfoObserver->RequestSupportedFormatsComplete(iCodecType, err);
		}

	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::CBody::RunError
// ---------------------------------------------------------------------------
TInt CA3FDevSoundAdaptationInfoImpl::CBody::RunError(TInt aError)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::RunError *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(aError, "%d");
	}

// ---------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::CBody::DoCancel
// ---------------------------------------------------------------------------
void CA3FDevSoundAdaptationInfoImpl::CBody::DoCancel()
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::DoCancel *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}
	
// ---------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::CBody::ReadSupportedFormats
// ---------------------------------------------------------------------------
void CA3FDevSoundAdaptationInfoImpl::CBody::ReadSupportedFormatsL(TUid aCodecType, RArray<TUid>& aSupportedFormats)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::ReadSupportedFormatsL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	RArray<TFourCC> supportedDataTypes;
	CleanupClosePushL(supportedDataTypes);
	
	SeekHwDevicePluginsL(supportedDataTypes, aCodecType);

	TInt maxFourCCElements = supportedDataTypes.Count();
		
	//clear any existing data in iSupportedFormats array
	aSupportedFormats.Reset();

	for(TInt typeIndex=0; typeIndex<maxFourCCElements; typeIndex++)
		{
		TUid format;
		
		TInt error = iFourCcConvertor->FourCCToFormat(supportedDataTypes[typeIndex], format);
		if(error == KErrNone)
			{
			aSupportedFormats.AppendL(format);
			}
		}
				
	CleanupStack::PopAndDestroy(&supportedDataTypes);
	
	DP_OUT();
	}


// -----------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::SeekHwDevicePluginsL
// This method looks for hwDevicePlugins that support the codec given in aCodecType
// which must be either EMMFStatePlaying or EMMFStateRecording.
// For each HwDevice plugin found the datatype as indicated by its fourCC code
// from the default_data field in the resource file is added to the array of
// aSupportedDataTypes
// -----------------------------------------------------------------------------
//
void CA3FDevSoundAdaptationInfoImpl::CBody::SeekHwDevicePluginsL(
	RArray<TFourCC>& aSupportedDataTypes,
	TUid aCodecType)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::SeekHwDevicePluginsL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	ASSERT(aCodecType == KUidAudioDecoder || aCodecType == KUidAudioEncoder);
	
	//clear any existing data in aSupportedDataTypes array
	aSupportedDataTypes.Reset();

	// Array to return hw device plugin resource info(place on cleanupstack
	// _after_ ListImplementationsL() )
	RImplInfoPtrArray plugInArray;
	CleanupResetAndDestroyPushL(plugInArray);
	
	TUid KUidMmfHWPluginInterfaceCodec = {KMmfUidPluginInterfaceHwDevice};
	
	REComSession::ListImplementationsL(KUidMmfHWPluginInterfaceCodec, plugInArray);

	TUint numberOfHwDevicePlugins = plugInArray.Count();

	// if there are hwdevice plugin resource entries then scan entries
	// matching on a datatype of pcm16 as the destination datatype for play and
	// the source datatype for record. If a match is found and isn't already in
	// the list of supported data types, then add it to the list
	if (numberOfHwDevicePlugins > 0)
		{
		CImplementationInformation* hwDeviceResourceEntry = NULL;
		_LIT8(KPCM16FourCCString, " P16");

		// check each resource entry for dst 4CC = P16 for play and
		// src 4CC = P16 for record
		for (TUint hwDeviceEntry = 0; hwDeviceEntry<numberOfHwDevicePlugins; hwDeviceEntry++)
			{
			hwDeviceResourceEntry = plugInArray[hwDeviceEntry];
			
			TFourCC fourCCLeft;
			TFourCC fourCCRight;
			
			ReadFourCC(hwDeviceResourceEntry, fourCCLeft, fourCCRight);

			TFourCC fourCCPCM16(KPCM16FourCCString);		
			
			if(aCodecType == KUidAudioDecoder)
				{
				if(IsDataTypeMatch(fourCCPCM16, fourCCRight))
					{
					if(!FourCCExists(aSupportedDataTypes, fourCCLeft))
						{
						aSupportedDataTypes.AppendL(fourCCLeft);
						}
					}
				}
			else if (aCodecType == KUidAudioEncoder)
				{
				if(IsDataTypeMatch(fourCCPCM16, fourCCLeft))
					{
					if(!FourCCExists(aSupportedDataTypes, fourCCRight))
						{
						aSupportedDataTypes.AppendL(fourCCRight);
						}
					}	
				}			
			}
		}

	CleanupStack::PopAndDestroy(&plugInArray);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::CBody::FourCCExists
// ---------------------------------------------------------------------------
TBool CA3FDevSoundAdaptationInfoImpl::CBody::FourCCExists(RArray<TFourCC>& aSupportedDataTypes, TFourCC aFourCCEntry)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::FourCCExists *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TBool alreadyExists = EFalse;
	
	for (TUint fourCCEntryNumber=0; fourCCEntryNumber< aSupportedDataTypes.Count(); fourCCEntryNumber++)
		{
		if (aSupportedDataTypes[fourCCEntryNumber] == aFourCCEntry)
			{
			// we already have this 4CC in the supported data types
			alreadyExists = ETrue;
			break;
			}
		}
		
	DP0_RET(alreadyExists, "0x%x");
	}

// ---------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::CBody::ReadFourCC
// ---------------------------------------------------------------------------
void CA3FDevSoundAdaptationInfoImpl::CBody::ReadFourCC(CImplementationInformation* aHwDeviceResourceEntry, TFourCC& aFourCCLeft, TFourCC& aFourCCRight)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::ReadFourCC *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TPtrC8 fourCCPtr(0,0);
	
	// Check for resource entry lenght since for HwDeviceAdaptor it is just "*"
	if (aHwDeviceResourceEntry->DataType().Length()>=KHwDeviceDefaultDataLength)
		{
		// datatype supported 4CC is left 4 chars
		fourCCPtr.Set(aHwDeviceResourceEntry->DataType().Left(KFOURCCLENGTH));
	
		aFourCCLeft.Set(fourCCPtr);

		// datatype supported 4CC is right 4 chars
		fourCCPtr.Set(aHwDeviceResourceEntry->DataType().Right(KFOURCCLENGTH));
	
		aFourCCRight.Set(fourCCPtr);
		}

	DP_OUT();
	}
	
// -----------------------------------------------------------------------------
// CA3FDevSoundAdaptationInfoImpl::IsDataTypeMatch
// -----------------------------------------------------------------------------
TBool CA3FDevSoundAdaptationInfoImpl::CBody::IsDataTypeMatch(TFourCC aFourCCPCM16,TFourCC aFourCC)
	{
	DP_STATIC_CONTEXT(CA3FDevSoundAdaptationInfoImpl::CBody::IsDataTypeMatch *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool match = EFalse;
	
	if(aFourCCPCM16 == aFourCC)
		{
		match = ETrue;
		}

	DP0_RET(match, "0x%x");
	}

// End of file
