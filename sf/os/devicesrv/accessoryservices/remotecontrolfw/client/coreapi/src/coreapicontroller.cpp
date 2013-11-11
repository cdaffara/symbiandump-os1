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
#include <remconcoreapicontroller.h>
#include <remconcoreapicontrollerobserver.h>
#include <remconinterfaceselector.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_CORE);
#endif

EXPORT_C CRemConCoreApiController* CRemConCoreApiController::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
											  MRemConCoreApiControllerObserver& aObserver)
	{
	LOG_STATIC_FUNC;

	CRemConCoreApiController* self = new(ELeave) CRemConCoreApiController(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRemConCoreApiController::CRemConCoreApiController(CRemConInterfaceSelector& aInterfaceSelector, 
							   MRemConCoreApiControllerObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConCoreApiUid), 
						 KRemConCoreApiMaxOperationSpecificDataSize, 
						 aInterfaceSelector,
						 ERemConClientTypeController),
	iObserver(aObserver)
	{
	}

EXPORT_C CRemConCoreApiController::~CRemConCoreApiController()
	{
	LOG_FUNC;
	}

TAny* CRemConCoreApiController::GetInterfaceIf(TUid aUid)
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

void CRemConCoreApiController::MrcibNewMessage(TUint aOperationId, const TDesC8& aData)
	{
	LOG_FUNC;
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	LOG1(_L("\taData.Length = %d"), aData.Length());

	// Get the response error out of aData.
	if ( aData.Length() < KRemConCoreApiResultDataLength )
		{
		LOG(_L("\tdropping response because results data not found"));
		return; // ditch malformed messages
		}
	
	__ASSERT_COMPILE(KRemConCoreApiResultDataLength <= 4);
	TInt err = *reinterpret_cast<const TInt*>(aData.Ptr());

	switch ( aOperationId )
		{
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
	case ERemConCoreApiPlay:
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
	case ERemConCoreApiTuneFunction:
	case ERemConCoreApiSelectDiskFunction:
	case ERemConCoreApiSelectAvInputFunction:
	case ERemConCoreApiSelectAudioInputFunction:
	case ERemConCoreApiF1:
	case ERemConCoreApiF2:
	case ERemConCoreApiF3:
	case ERemConCoreApiF4:
	case ERemConCoreApiF5:
		iObserver.MrccacoResponse((TRemConCoreApiOperationId)aOperationId, err);
		break;

	default:
		break;
		}
	}

void CRemConCoreApiController::SendGenericCommand(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct, 
	TRemConCoreApiOperationId aOperationId)
	{
	iCmdData.Copy((TUint8*)&aButtonAct, KRemConCoreApiButtonDataLength);
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConCoreApiUid),
		(TUint)aOperationId, 
		aNumRemotes,
		ERemConCommand,
		iCmdData);
	}

EXPORT_C void CRemConCoreApiController::Select(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiSelect);
	}

EXPORT_C void CRemConCoreApiController::Up(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiUp);
	}

EXPORT_C void CRemConCoreApiController::Down(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiDown);
	}

EXPORT_C void CRemConCoreApiController::Left(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiLeft);
	}

EXPORT_C void CRemConCoreApiController::Right(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiRight);
	}

EXPORT_C void CRemConCoreApiController::RightUp(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiRightUp);
	}

EXPORT_C void CRemConCoreApiController::RightDown(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiRightDown);
	}

EXPORT_C void CRemConCoreApiController::LeftUp(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiLeftUp);
	}

EXPORT_C void CRemConCoreApiController::LeftDown(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiLeftDown);
	}

EXPORT_C void CRemConCoreApiController::RootMenu(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiRootMenu);
	}

EXPORT_C void CRemConCoreApiController::SetupMenu(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiSetupMenu);
	}

EXPORT_C void CRemConCoreApiController::ContentsMenu(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiContentsMenu);
	}

EXPORT_C void CRemConCoreApiController::FavoriteMenu(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiFavoriteMenu);
	}

EXPORT_C void CRemConCoreApiController::Exit(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiExit);
	}

EXPORT_C void CRemConCoreApiController::_0(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi0);
	}

EXPORT_C void CRemConCoreApiController::_1(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi1);
	}

EXPORT_C void CRemConCoreApiController::_2(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi2);
	}

EXPORT_C void CRemConCoreApiController::_3(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi3);
	}

EXPORT_C void CRemConCoreApiController::_4(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi4);
	}

EXPORT_C void CRemConCoreApiController::_5(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi5);
	}

EXPORT_C void CRemConCoreApiController::_6(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi6);
	}

EXPORT_C void CRemConCoreApiController::_7(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi7);
	}

EXPORT_C void CRemConCoreApiController::_8(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi8);
	}

EXPORT_C void CRemConCoreApiController::_9(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApi9);
	}

EXPORT_C void CRemConCoreApiController::Dot(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiDot);
	}

EXPORT_C void CRemConCoreApiController::Enter(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiEnter);
	}

EXPORT_C void CRemConCoreApiController::Clear(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiClear);
	}

EXPORT_C void CRemConCoreApiController::ChannelUp(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiChannelUp);
	}

EXPORT_C void CRemConCoreApiController::ChannelDown(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiChannelDown);
	}

EXPORT_C void CRemConCoreApiController::PreviousChannel(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiPreviousChannel);
	}

EXPORT_C void CRemConCoreApiController::SoundSelect(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiSoundSelect);
	}

EXPORT_C void CRemConCoreApiController::InputSelect(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiInputSelect);
	}

EXPORT_C void CRemConCoreApiController::DisplayInformation(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiDisplayInformation);
	}

EXPORT_C void CRemConCoreApiController::Help(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiHelp);
	}

EXPORT_C void CRemConCoreApiController::PageUp(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiPageUp);
	}

EXPORT_C void CRemConCoreApiController::PageDown(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiPageDown);
	}

EXPORT_C void CRemConCoreApiController::Power(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiPower);
	}

EXPORT_C void CRemConCoreApiController::VolumeUp(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiVolumeUp);
	}

EXPORT_C void CRemConCoreApiController::VolumeDown(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiVolumeDown);
	}

EXPORT_C void CRemConCoreApiController::Mute(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiMute);
	}

EXPORT_C void CRemConCoreApiController::Play(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct,
	TRemConCoreApiPlaybackSpeed aSpeed)
	{
	LOG_FUNC;
	
	iCmdData.Copy((TUint8*)&aButtonAct, KRemConCoreApiButtonDataLength);
	iCmdData.AppendFormat(_L8("%04x"), aSpeed);
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConCoreApiUid),
		(TUint)ERemConCoreApiPlay, 
		aNumRemotes,
		ERemConCommand,
		iCmdData);
	}

EXPORT_C void CRemConCoreApiController::Stop(TRequestStatus& aStatus, 
											 TUint& aNumRemotes,
											 TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;

	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiStop);
	}

EXPORT_C void CRemConCoreApiController::Pause(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiPause);
	}

EXPORT_C void CRemConCoreApiController::Record(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiRecord);
	}

EXPORT_C void CRemConCoreApiController::Rewind(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiRewind);
	}

EXPORT_C void CRemConCoreApiController::FastForward(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiFastForward);
	}

EXPORT_C void CRemConCoreApiController::Eject(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiEject);
	}

EXPORT_C void CRemConCoreApiController::Forward(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiForward);
	}

EXPORT_C void CRemConCoreApiController::Backward(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiBackward);
	}

EXPORT_C void CRemConCoreApiController::Angle(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiAngle);
	}

EXPORT_C void CRemConCoreApiController::Subpicture(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiSubpicture);
	}

EXPORT_C void CRemConCoreApiController::PausePlayFunction(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiPausePlayFunction);
	}

EXPORT_C void CRemConCoreApiController::RestoreVolumeFunction(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiRestoreVolumeFunction);
	}

EXPORT_C void CRemConCoreApiController::TuneFunction(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TBool aTwoPart, 
	TUint aMajorChannel, 
	TUint aMinorChannel,
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;

	iCmdData.Copy((TUint8*)&aButtonAct, KRemConCoreApiButtonDataLength);
	iCmdData.AppendFormat(_L8("%d%d%d"), aTwoPart, aMajorChannel, aMinorChannel);
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConCoreApiUid),
		(TUint)ERemConCoreApiTuneFunction, 
		aNumRemotes,
		ERemConCommand,
		iCmdData);
	}

EXPORT_C void CRemConCoreApiController::SelectDiskFunction(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TUint aDisk,
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	iCmdData.Copy((TUint8*)&aButtonAct, KRemConCoreApiButtonDataLength);
	iCmdData.AppendFormat(_L8("%d"), aDisk);
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConCoreApiUid),
		(TUint)ERemConCoreApiSelectDiskFunction, 
		aNumRemotes,
		ERemConCommand,
		iCmdData);
	}

EXPORT_C void CRemConCoreApiController::SelectAvInputFunction(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TUint8 aAvInputSignalNumber,
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	iCmdData.Copy((TUint8*)&aButtonAct, KRemConCoreApiButtonDataLength);
	iCmdData.AppendFormat(_L8("%d"), aAvInputSignalNumber);
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConCoreApiUid),
		(TUint)ERemConCoreApiSelectAvInputFunction, 
		aNumRemotes,
		ERemConCommand,
		iCmdData);
	}

EXPORT_C void CRemConCoreApiController::SelectAudioInputFunction(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TUint8 aAudioInputSignalNumber,
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	iCmdData.Copy((TUint8*)&aButtonAct, KRemConCoreApiButtonDataLength);
	iCmdData.AppendFormat(_L8("%d"), aAudioInputSignalNumber);
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConCoreApiUid),
		(TUint)ERemConCoreApiSelectAudioInputFunction, 
		aNumRemotes,
		ERemConCommand,
		iCmdData);
	}

EXPORT_C void CRemConCoreApiController::F1(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiF1);
	}

EXPORT_C void CRemConCoreApiController::F2(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiF2);
	}

EXPORT_C void CRemConCoreApiController::F3(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiF3);
	}

EXPORT_C void CRemConCoreApiController::F4(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiF4);
	}

EXPORT_C void CRemConCoreApiController::F5(TRequestStatus& aStatus, 
	TUint& aNumRemotes, 
	TRemConCoreApiButtonAction aButtonAct)
	{
	LOG_FUNC;
	
	SendGenericCommand(aStatus, aNumRemotes, aButtonAct, ERemConCoreApiF5);
	}
