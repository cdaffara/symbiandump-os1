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
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <remconcoreapitarget.h>
#include <remconcoreapitargetobserver.h>
#include <remconinterfaceselector.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_CORE);
#endif

// Useful symbolic constant for navigating around data items.
const TUint KDataItemLength = 4;

EXPORT_C CRemConCoreApiTarget* CRemConCoreApiTarget::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
											  MRemConCoreApiTargetObserver& aObserver)
	{
	LOG_STATIC_FUNC

	CRemConCoreApiTarget* self = new(ELeave) CRemConCoreApiTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRemConCoreApiTarget* CRemConCoreApiTarget::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
			MRemConCoreApiTargetObserver& aObserver, const RArray<TRemConCoreApiOperationId>& aFeatureSupported)
	{
	LOG_STATIC_FUNC

	CRemConCoreApiTarget* self = new(ELeave) CRemConCoreApiTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aFeatureSupported);
	CleanupStack::Pop(self);
	return self;
	}
CRemConCoreApiTarget::CRemConCoreApiTarget(CRemConInterfaceSelector& aInterfaceSelector, 
							   MRemConCoreApiTargetObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConCoreApiUid), 
						 KRemConCoreApiMaxOperationSpecificDataSize, 
						 aInterfaceSelector,
						 ERemConClientTypeTarget),
	iObserver(aObserver)
	{	
	iOutstandingOperations.Reset();
	}

EXPORT_C CRemConCoreApiTarget::~CRemConCoreApiTarget()
	{
	LOG_FUNC
	}

TAny* CRemConCoreApiTarget::GetInterfaceIf(TUid aUid)
	{
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KRemConInterfaceIf1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConInterfaceIf*>(this)
			);
		}

	return ret;
	}

void CRemConCoreApiTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& aData)
	{
	LOG_FUNC
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	LOG1(_L("\taData.Length = %d"), aData.Length());

	// Get the button action
	if ( aData.Length() < KRemConCoreApiButtonDataLength )
		{
		LOG(_L("\tdropping command because button data not found"));
		return; // ditch malformed messages
		}
	TRemConCoreApiButtonAction button = static_cast<TRemConCoreApiButtonAction>(aData.Ptr()[0]);
	
	if( aOperationId < ELargestOperationId ) 
		{
		/* If this command was received earlier and there was no response sent by 
		 * the client (i.e. iOperations[aOperationId] is ETrue) then respond to the 
		 * command so that remcon removes this command from 'Incoming Delivered'
		 * queue
		 */ 
		if( iOutstandingOperations[aOperationId] )
			{
			/*This will send the response unreliably. iOutstandingOperations will not
			 be altered so we do not need to set it again to ETrue for this incoming
			 command*/
			InterfaceSelector().SendUnreliable(TUid::Uid(KRemConCoreApiUid),
												aOperationId,
												ERemConResponse,
												aData);
			}
		else
			iOutstandingOperations[aOperationId] = ETrue;
		}

	switch ( aOperationId )
		{
	case ERemConCoreApiPlay:
		HandlePlay(aData, button);
		break;

	case ERemConCoreApiSelect:
	case ERemConCoreApiUp:
	case ERemConCoreApiDown:
	case ERemConCoreApiLeft:
	case ERemConCoreApiRight:
	case ERemConCoreApiRightUp:
	case ERemConCoreApiRightDown:
	case ERemConCoreApiLeftUp:
	case ERemConCoreApiLeftDown:
	case ERemConCoreApiRootMenu:
	case ERemConCoreApiSetupMenu:
	case ERemConCoreApiContentsMenu:
	case ERemConCoreApiFavoriteMenu:
	case ERemConCoreApiExit:
	case ERemConCoreApi0:
	case ERemConCoreApi1:
	case ERemConCoreApi2:
	case ERemConCoreApi3:
	case ERemConCoreApi4:
	case ERemConCoreApi5:
	case ERemConCoreApi6:
	case ERemConCoreApi7:
	case ERemConCoreApi8:
	case ERemConCoreApi9:
	case ERemConCoreApiDot:
	case ERemConCoreApiEnter:
	case ERemConCoreApiClear:
	case ERemConCoreApiChannelUp:
	case ERemConCoreApiChannelDown:
	case ERemConCoreApiPreviousChannel:
	case ERemConCoreApiSoundSelect:
	case ERemConCoreApiInputSelect:
	case ERemConCoreApiDisplayInformation:
	case ERemConCoreApiHelp:
	case ERemConCoreApiPageUp:
	case ERemConCoreApiPageDown:
	case ERemConCoreApiPower:
	case ERemConCoreApiVolumeUp:
	case ERemConCoreApiVolumeDown:
	case ERemConCoreApiMute:
	case ERemConCoreApiStop:
	case ERemConCoreApiPause:
	case ERemConCoreApiRecord:
	case ERemConCoreApiRewind:
	case ERemConCoreApiFastForward:
	case ERemConCoreApiEject:
	case ERemConCoreApiForward:
	case ERemConCoreApiBackward:
	case ERemConCoreApiAngle:
	case ERemConCoreApiSubpicture:
	case ERemConCoreApiPausePlayFunction:
	case ERemConCoreApiRestoreVolumeFunction:
	case ERemConCoreApiF1:
	case ERemConCoreApiF2:
	case ERemConCoreApiF3:
	case ERemConCoreApiF4:
	case ERemConCoreApiF5:
		iObserver.MrccatoCommand((TRemConCoreApiOperationId)aOperationId, button);
		break;

	case ERemConCoreApiTuneFunction:
		HandleTuneFunction(aData, button);
		break;

	case ERemConCoreApiSelectDiskFunction:
		HandleSelectDiskFunction(aData, button);
		break;

	case ERemConCoreApiSelectAvInputFunction:
		HandleSelectAvInputFunction(aData, button);
		break;

	case ERemConCoreApiSelectAudioInputFunction:
		HandleSelectAudioInputFunction(aData, button);
		break;

	default:
		break;
		}
	}

void CRemConCoreApiTarget::HandlePlay(const TDesC8& aData, TRemConCoreApiButtonAction aButton)
	{
	// Decompose aData to get the playback speed out of it
	TUint speed;
	if ( aData.Length() < KRemConCoreApiButtonDataLength + KDataItemLength )
		{
		LOG(_L("\tcommand lacks play speed data- using default"));
		// If there's no play speed data, go with the default.
		speed = ERemConCoreApiPlaybackSpeedX1;
		}
	else
		{
		TLex8 lex(aData.Mid(KRemConCoreApiButtonDataLength, KDataItemLength));
		if ( lex.Val(speed, EHex) != KErrNone )
			{
			LOG(_L("\tdropping command because play speed data unreadable"));
			// Play speed data was present, but unintelligible. This is a 
			// malformed message- ditch it.
			return;
			}
		}
	iObserver.MrccatoPlay((TRemConCoreApiPlaybackSpeed)speed, aButton);
	}

void CRemConCoreApiTarget::HandleTuneFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton)
	{
	// Decompose aData to get the data out of it
	// There are 3 items of tuning data.
	if ( aData.Length() < KRemConCoreApiButtonDataLength + 3*KDataItemLength )
		{
		LOG(_L("\tdropping command because tune function data not found"));
		return; // ditch malformed messages
		}
	TLex8 lex(aData.Mid(KRemConCoreApiButtonDataLength, KDataItemLength));
	TUint twoPart;
	if ( lex.Val(twoPart, EHex) == KErrNone )
		{
		TLex8 lex(aData.Mid(KRemConCoreApiButtonDataLength + KDataItemLength, KDataItemLength));
		TUint major;
		if ( lex.Val(major, EHex) == KErrNone )
			{
			TLex8 lex(aData.Mid(KRemConCoreApiButtonDataLength + 2*KDataItemLength, KDataItemLength));
			TUint minor;
			if ( lex.Val(minor, EHex) == KErrNone )
				{							  
				iObserver.MrccatoTuneFunction((TBool)twoPart, major, minor, aButton);
				}
			else
				{
				LOG(_L("\tdropping command because minor tune function data unreadable"));
				}
			}
		else
			{
			LOG(_L("\tdropping command because major tune function data unreadable"));
			}
		}
	else
		{
		LOG(_L("\tdropping command because 'two part' tune function data unreadable"));
		}
	}

void CRemConCoreApiTarget::HandleSelectDiskFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton)
	{
	// Decompose aData
	if ( aData.Length() < KRemConCoreApiButtonDataLength + KDataItemLength )
		{
		LOG(_L("\tdropping command because select disk function data not found"));
		return; // ditch malformed messages
		}
	TLex8 lex(aData.Mid(KRemConCoreApiButtonDataLength, KDataItemLength));
	TUint disk;
	if ( lex.Val(disk, EHex) == KErrNone )
		{
		iObserver.MrccatoSelectDiskFunction(disk, aButton);
		}
	else
		{
		// Just ditch malformed messages.
		LOG(_L("\tdropping command because select disk function data unreadable"));
		}
	}

void CRemConCoreApiTarget::HandleSelectAvInputFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton)
	{
	// Decompose aData
	if ( aData.Length() < KRemConCoreApiButtonDataLength + KDataItemLength )
		{
		LOG(_L("\tdropping command because select AV input function data not found"));
		return; // ditch malformed messages
		}
	TLex8 lex(aData.Mid(KRemConCoreApiButtonDataLength, KDataItemLength));
	TUint av;
	if ( lex.Val(av, EHex) == KErrNone )
		{
		iObserver.MrccatoSelectAvInputFunction(av, aButton);
		}
	else
		{
		// Just ditch malformed messages.
		LOG(_L("\tdropping command because select AV input function data unreadable"));
		}
	}

void CRemConCoreApiTarget::HandleSelectAudioInputFunction(const TDesC8& aData, TRemConCoreApiButtonAction aButton)
	{
	// Decompose aData
	if ( aData.Length() < KRemConCoreApiButtonDataLength + KDataItemLength )
		{
		LOG(_L("\tdropping command because select audio input function data not found"));
		return; // ditch malformed messages
		}
	TLex8 lex(aData.Mid(KRemConCoreApiButtonDataLength, KDataItemLength));
	TUint audio;
	if ( lex.Val(audio, EHex) == KErrNone )
		{
		iObserver.MrccatoSelectAudioInputFunction(audio, aButton);
		}
	else
		{
		// Just ditch malformed messages.
		LOG(_L("\tdropping command because select audio input function data unreadable"));
		}
	}

void CRemConCoreApiTarget::SendGenericResponse(TRequestStatus& aStatus,
											   TRemConCoreApiOperationId aOperationId,
											   TInt aError)
	{
	iRspData.Copy((TUint8*)&aError, KRemConCoreApiResultDataLength);
	
	if( aOperationId >= 0 && aOperationId < ELargestOperationId ) 
		{
		iOutstandingOperations[aOperationId] = EFalse;
		}
	
	// We pass iNumRemotes even though we're not interested in its value as 
	// RemCon will write to this location and we need it to be somewhere safe.
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConCoreApiUid),
		(TUint)aOperationId, 
		iNumRemotes,
		ERemConResponse,
		iRspData);
	}

EXPORT_C void CRemConCoreApiTarget::SelectResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiSelect, aError);
	}

EXPORT_C void CRemConCoreApiTarget::UpResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiUp, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::DownResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiDown, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::LeftResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiLeft, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::RightResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiRight, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::RightUpResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiRightUp, aError);
	}

EXPORT_C void CRemConCoreApiTarget::RightDownResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiRightDown, aError);
	}

EXPORT_C void CRemConCoreApiTarget::LeftUpResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiLeftUp, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::LeftDownResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiLeftDown, aError);
	}

EXPORT_C void CRemConCoreApiTarget::RootMenuResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiRootMenu, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::SetupMenuResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiSetupMenu, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::ContentsMenuResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiContentsMenu, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::FavoriteMenuResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiFavoriteMenu, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::ExitResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiExit, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_0Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi0, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_1Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi1, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_2Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi2, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_3Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi3, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_4Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi4, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_5Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi5, aError);
	}

EXPORT_C void CRemConCoreApiTarget::_6Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi6, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_7Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi7, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::_8Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi8, aError);
	}

EXPORT_C void CRemConCoreApiTarget::_9Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApi9, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::DotResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiDot, aError);
	}

EXPORT_C void CRemConCoreApiTarget::EnterResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiEnter, aError);
	}

EXPORT_C void CRemConCoreApiTarget::ClearResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiClear, aError);
	}

EXPORT_C void CRemConCoreApiTarget::ChannelUpResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiChannelUp, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::ChannelDownResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiChannelDown, aError);
	}

EXPORT_C void CRemConCoreApiTarget::PreviousChannelResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiPreviousChannel, aError);
	}

EXPORT_C void CRemConCoreApiTarget::SoundSelectResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiSoundSelect, aError);
	}

EXPORT_C void CRemConCoreApiTarget::InputSelectResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiInputSelect, aError);
	}

EXPORT_C void CRemConCoreApiTarget::DisplayInformationResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiDisplayInformation, aError);
	}

EXPORT_C void CRemConCoreApiTarget::HelpResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiHelp, aError);
	}

EXPORT_C void CRemConCoreApiTarget::PageUpResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiPageUp, aError);
	}

EXPORT_C void CRemConCoreApiTarget::PageDownResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiPageDown, aError);
	}

EXPORT_C void CRemConCoreApiTarget::PowerResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiPower, aError);
	}

EXPORT_C void CRemConCoreApiTarget::VolumeUpResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiVolumeUp, aError);
	}

EXPORT_C void CRemConCoreApiTarget::VolumeDownResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiVolumeDown, aError);
	}

EXPORT_C void CRemConCoreApiTarget::MuteResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiMute, aError);
	}

EXPORT_C void CRemConCoreApiTarget::PlayResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiPlay, aError);
	}

EXPORT_C void CRemConCoreApiTarget::StopResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiStop, aError);
	}

EXPORT_C void CRemConCoreApiTarget::PauseResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiPause, aError);
	}

EXPORT_C void CRemConCoreApiTarget::RecordResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiRecord, aError);
	}

EXPORT_C void CRemConCoreApiTarget::RewindResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiRewind, aError);
	}

EXPORT_C void CRemConCoreApiTarget::FastForwardResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiFastForward, aError);
	}

EXPORT_C void CRemConCoreApiTarget::EjectResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiEject, aError);
	}

EXPORT_C void CRemConCoreApiTarget::ForwardResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiForward, aError);
	}

EXPORT_C void CRemConCoreApiTarget::BackwardResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiBackward, aError);
	}

EXPORT_C void CRemConCoreApiTarget::AngleResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiAngle, aError);
	}

EXPORT_C void CRemConCoreApiTarget::SubpictureResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiSubpicture, aError);
	}

EXPORT_C void CRemConCoreApiTarget::PausePlayFunctionResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiPausePlayFunction, aError);
	}

EXPORT_C void CRemConCoreApiTarget::RestoreVolumeFunctionResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiRestoreVolumeFunction, aError);
	}

EXPORT_C void CRemConCoreApiTarget::TuneFunctionResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiTuneFunction, aError);
	}

EXPORT_C void CRemConCoreApiTarget::SelectDiskFunctionResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiSelectDiskFunction, aError);
	}

EXPORT_C void CRemConCoreApiTarget::SelectAvInputFunctionResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiSelectAvInputFunction, aError);
	}

EXPORT_C void CRemConCoreApiTarget::SelectAudioInputFunctionResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiSelectAudioInputFunction, aError);
	}

EXPORT_C void CRemConCoreApiTarget::F1Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiF1, aError);
	}

EXPORT_C void CRemConCoreApiTarget::F2Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiF2, aError);
	}

EXPORT_C void CRemConCoreApiTarget::F3Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiF3, aError);
	}

EXPORT_C void CRemConCoreApiTarget::F4Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiF4, aError);
	}

EXPORT_C void CRemConCoreApiTarget::F5Response(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, ERemConCoreApiF5, aError);
	}
	
EXPORT_C void CRemConCoreApiTarget::SendResponse(TRequestStatus& aStatus, TRemConCoreApiOperationId aOperationId, TInt aError)
	{
	LOG_FUNC

	SendGenericResponse(aStatus, aOperationId, aError);
	}
void CRemConCoreApiTarget::ConstructL(const RArray<TRemConCoreApiOperationId>& aFeaturesSupported)
	{
	LOG_FUNC
	
	RRemConInterfaceFeatures features;
	LEAVEIFERRORL(features.Open());
	CleanupClosePushL(features);

	for(TInt i = 0 ; i < aFeaturesSupported.Count() ; i++)
		{
		features.AddOperationL(aFeaturesSupported[i]);
		}
	
	BaseConstructL(features);
	CleanupStack::PopAndDestroy(&features);
	}
