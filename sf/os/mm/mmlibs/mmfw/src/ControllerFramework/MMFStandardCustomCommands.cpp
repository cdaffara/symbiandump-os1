// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mmf/common/mmfaudio.h>
#include "mmfstandardcustomcommands.h"
#include "MMFVideoFrameMessage.h"
#include "MMFSCCPanicCodes.h"
#include <badesca.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfstandardcustomcommandsenums.h>
#include <mmf/common/mmfstandardcustomcommandsimpl.h>
#include <mmf/common/mmfvideoenums.h>
#endif


const TInt KBufExpandSize8 = 8;//two TInts!
const TInt KBufExpandSize32 = 32;

const TInt KBufMimeTypeGranularity = 4;
const TInt KMaxMimeTypeLength = 256;

class TMimeTypeBufferInfo 
	{
public:
	TInt32 count;
	TInt32 bufferLen;
	};

EXPORT_C RMMFAudioPlayDeviceCustomCommands::RMMFAudioPlayDeviceCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFAudioPlayDevice)
	{
	}

EXPORT_C TInt RMMFAudioPlayDeviceCustomCommands::SetVolume(TInt aVolume) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iVolume = aVolume;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioPlayDeviceSetVolume, 
										 configPackage,
										 KNullDesC8);
	}


EXPORT_C TInt RMMFAudioPlayDeviceCustomCommands::GetMaxVolume(TInt& aMaxVolume) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioPlayDeviceGetMaxVolume, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aMaxVolume = configPackage().iMaxVolume;
	return error;
	}

EXPORT_C TInt RMMFAudioPlayDeviceCustomCommands::GetVolume(TInt& aVolume) const 
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioPlayDeviceGetVolume, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aVolume = configPackage().iVolume;
	return error;
	}

EXPORT_C TInt RMMFAudioPlayDeviceCustomCommands::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iRampDuration = aRampDuration;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioPlayDeviceSetVolumeRamp, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioPlayDeviceCustomCommands::SetBalance(TInt aBalance) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iBalance = aBalance;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioPlayDeviceSetBalance, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioPlayDeviceCustomCommands::GetBalance(TInt& aBalance) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioPlayDeviceGetBalance, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aBalance = configPackage().iBalance;
	return error;
	}

EXPORT_C CMMFAudioPlayDeviceCustomCommandParser* CMMFAudioPlayDeviceCustomCommandParser::NewL(MMMFAudioPlayDeviceCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFAudioPlayDeviceCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFAudioPlayDeviceCustomCommandParser::~CMMFAudioPlayDeviceCustomCommandParser()
	{
	}

CMMFAudioPlayDeviceCustomCommandParser::CMMFAudioPlayDeviceCustomCommandParser(MMMFAudioPlayDeviceCustomCommandImplementor& aImplementor) : 
	CMMFCustomCommandParserBase(KUidInterfaceMMFAudioPlayDevice),
	iImplementor(aImplementor)
	{
	}


void CMMFAudioPlayDeviceCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFAudioPlayDevice)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFAudioPlayDeviceCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFAudioPlayDeviceSetVolume:
		complete = DoSetVolumeL(aMessage);
		break;
	case EMMFAudioPlayDeviceGetMaxVolume:
		complete = DoGetMaxVolumeL(aMessage);
		break;
	case EMMFAudioPlayDeviceGetVolume:
		complete = DoGetVolumeL(aMessage);
		break;
	case EMMFAudioPlayDeviceSetVolumeRamp:
		complete = DoSetVolumeRampL(aMessage);
		break;
	case EMMFAudioPlayDeviceSetBalance:
		complete = DoSetBalanceL(aMessage);
		break;
	case EMMFAudioPlayDeviceGetBalance:
		complete = DoGetBalanceL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}


TBool CMMFAudioPlayDeviceCustomCommandParser::DoSetVolumeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MapdSetVolumeL(pckg().iVolume);
	return ETrue;
	}

TBool CMMFAudioPlayDeviceCustomCommandParser::DoGetMaxVolumeL(TMMFMessage& aMessage)
	{
	TInt maxVol = 0;
	iImplementor.MapdGetMaxVolumeL(maxVol);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iMaxVolume = maxVol;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioPlayDeviceCustomCommandParser::DoGetVolumeL(TMMFMessage& aMessage)
	{
	TInt vol = 0;
	iImplementor.MapdGetVolumeL(vol);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iVolume = vol;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioPlayDeviceCustomCommandParser::DoSetVolumeRampL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MapdSetVolumeRampL(pckg().iRampDuration);
	return ETrue;
	}

TBool CMMFAudioPlayDeviceCustomCommandParser::DoSetBalanceL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MapdSetBalanceL(pckg().iBalance);
	return ETrue;
	}

TBool CMMFAudioPlayDeviceCustomCommandParser::DoGetBalanceL(TMMFMessage& aMessage)
	{
	TInt bal = 0;
	iImplementor.MapdGetBalanceL(bal);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iBalance = bal;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}























EXPORT_C RMMFAudioRecordDeviceCustomCommands::RMMFAudioRecordDeviceCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFAudioRecordDevice)
	{
	}

EXPORT_C TInt RMMFAudioRecordDeviceCustomCommands::SetGain(TInt aGain) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iGain = aGain;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioRecordDeviceSetGain, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioRecordDeviceCustomCommands::GetMaxGain(TInt& aMaxGain) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioRecordDeviceGetMaxGain, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);
	if (!error)
		aMaxGain = configPackage().iMaxGain;
	return error;
	}

EXPORT_C TInt RMMFAudioRecordDeviceCustomCommands::GetGain(TInt& aGain) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioRecordDeviceGetGain, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);
	if (!error)
		aGain = configPackage().iGain;
	return error;
	}

EXPORT_C TInt RMMFAudioRecordDeviceCustomCommands::SetBalance(TInt aBalance) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iBalance = aBalance;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioRecordDeviceSetBalance, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioRecordDeviceCustomCommands::GetBalance(TInt& aBalance) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioRecordDeviceGetBalance, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aBalance = configPackage().iBalance;
	return error;
	}

EXPORT_C CMMFAudioRecordDeviceCustomCommandParser* CMMFAudioRecordDeviceCustomCommandParser::NewL(MMMFAudioRecordDeviceCustomCommandImplementor& aImplementor) 
	{
	return new(ELeave) CMMFAudioRecordDeviceCustomCommandParser(aImplementor);
	}

CMMFAudioRecordDeviceCustomCommandParser::CMMFAudioRecordDeviceCustomCommandParser(MMMFAudioRecordDeviceCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFAudioRecordDevice),
	iImplementor(aImplementor)
	{
	}

EXPORT_C CMMFAudioRecordDeviceCustomCommandParser::~CMMFAudioRecordDeviceCustomCommandParser()
	{
	}

void CMMFAudioRecordDeviceCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFAudioRecordDevice)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFAudioRecordDeviceCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFAudioRecordDeviceSetGain:
		complete = DoSetGainL(aMessage);
		break;
	case EMMFAudioRecordDeviceGetMaxGain:
		complete = DoGetMaxGainL(aMessage);
		break;
	case EMMFAudioRecordDeviceGetGain:
		complete = DoGetGainL(aMessage);
		break;
	case EMMFAudioRecordDeviceSetBalance:
		complete = DoSetBalanceL(aMessage);
		break;
	case EMMFAudioRecordDeviceGetBalance:
		complete = DoGetBalanceL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFAudioRecordDeviceCustomCommandParser::DoSetGainL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MardSetGainL(pckg().iGain);
	return ETrue;
	}

TBool CMMFAudioRecordDeviceCustomCommandParser::DoGetMaxGainL(TMMFMessage& aMessage)
	{
	TInt maxGain = 0;
	iImplementor.MardGetMaxGainL(maxGain);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iMaxGain = maxGain;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioRecordDeviceCustomCommandParser::DoGetGainL(TMMFMessage& aMessage)
	{
	TInt gain = 0;
	iImplementor.MardGetGainL(gain);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iGain = gain;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioRecordDeviceCustomCommandParser::DoSetBalanceL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MardSetBalanceL(pckg().iBalance);
	return ETrue;
	}

TBool CMMFAudioRecordDeviceCustomCommandParser::DoGetBalanceL(TMMFMessage& aMessage)
	{
	TInt balance = 0;
	iImplementor.MardGetBalanceL(balance);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iBalance = balance;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}





















EXPORT_C RMMFAudioPlayControllerCustomCommands::RMMFAudioPlayControllerCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFAudioPlayController)
	{
	}

EXPORT_C TInt RMMFAudioPlayControllerCustomCommands::SetPlaybackWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iStartPosition = aStart;
	configPackage().iEndPosition = aEnd;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioPlayControllerSetPlaybackWindow, 
										 configPackage,
										 KNullDesC8);
	}


EXPORT_C TInt RMMFAudioPlayControllerCustomCommands::DeletePlaybackWindow()
	{
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioPlayControllerDeletePlaybackWindow, 
										 KNullDesC8,
										 KNullDesC8);
	}
	
EXPORT_C TInt RMMFAudioPlayControllerCustomCommands::GetLoadingProgress(TInt& aPercentageComplete) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioPlayControllerGetLoadingProgress, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aPercentageComplete = configPackage().iLoadingCompletePercentage;
	return error;
	}



EXPORT_C CMMFAudioPlayControllerCustomCommandParser* CMMFAudioPlayControllerCustomCommandParser::NewL(MMMFAudioPlayControllerCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFAudioPlayControllerCustomCommandParser(aImplementor);
	}

CMMFAudioPlayControllerCustomCommandParser::CMMFAudioPlayControllerCustomCommandParser(MMMFAudioPlayControllerCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFAudioPlayController),
	iImplementor(aImplementor)
	{
	}

EXPORT_C CMMFAudioPlayControllerCustomCommandParser::~CMMFAudioPlayControllerCustomCommandParser()
	{
	}

void CMMFAudioPlayControllerCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFAudioPlayController)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFAudioPlayControllerCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFAudioPlayControllerSetPlaybackWindow:
		complete = DoSetPlaybackWindowL(aMessage);
		break;
	case EMMFAudioPlayControllerDeletePlaybackWindow:
		complete = DoDeletePlaybackWindowL(aMessage);
		break;
	case EMMFAudioPlayControllerGetLoadingProgress:
		complete = DoGetLoadingProgressL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFAudioPlayControllerCustomCommandParser::DoSetPlaybackWindowL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MapcSetPlaybackWindowL(pckg().iStartPosition, pckg().iEndPosition);
	return ETrue;
	}

TBool CMMFAudioPlayControllerCustomCommandParser::DoDeletePlaybackWindowL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MapcDeletePlaybackWindowL();
	return ETrue;
	}

TBool CMMFAudioPlayControllerCustomCommandParser::DoGetLoadingProgressL(TMMFMessage& aMessage)
	{
	TInt progress;
	iImplementor.MapcGetLoadingProgressL(progress);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iLoadingCompletePercentage = progress;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}











EXPORT_C RMMFAudioRecordControllerCustomCommands::RMMFAudioRecordControllerCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFAudioRecordController)
	{
	}

EXPORT_C TInt RMMFAudioRecordControllerCustomCommands::GetRecordTimeAvailable(TTimeIntervalMicroSeconds& aTime) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioRecordControllerGetRecordTimeAvailable, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aTime = configPackage().iRecordTimeAvailable;
	return error;
	}

EXPORT_C TInt RMMFAudioRecordControllerCustomCommands::SetMaxDuration(const TTimeIntervalMicroSeconds& aMaxDuration) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iMaxDuration = aMaxDuration;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioRecordControllerSetMaxDuration, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioRecordControllerCustomCommands::SetMaxFileSize(TInt aMaxSize) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iMaxFileSize = aMaxSize;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioRecordControllerSetMaxFileSize, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioRecordControllerCustomCommands::Crop(TBool aToEnd)
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iCropToEnd = aToEnd;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioRecordControllerCrop, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C void RMMFAudioRecordControllerCustomCommands::AddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry)
	{
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize32);
	CleanupStack::PushL(buf);
	RBufWriteStream s;
	s.Open(*buf);
	CleanupClosePushL(s);
	aNewEntry.ExternalizeL(s);
	TPtr8 bufData = buf->Ptr(0);
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFAudioRecordControllerAddMetaDataEntry,
													 bufData,
													 KNullDesC8));
	CleanupStack::PopAndDestroy(2);//s, buf
	}

EXPORT_C TInt RMMFAudioRecordControllerCustomCommands::RemoveMetaDataEntry(TInt aIndex)
	{
	TPckgBuf<TInt> pckg(aIndex);
	return iController.CustomCommandSync(iDestinationPckg,
										 EMMFAudioRecordControllerRemoveMetaDataEntry,
										 pckg,
										 KNullDesC8);
	}

EXPORT_C void RMMFAudioRecordControllerCustomCommands::ReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry)
	{
	TPckgBuf<TInt> indexPckg(aIndex);
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize32);
	CleanupStack::PushL(buf);
	RBufWriteStream s;
	s.Open(*buf);
	CleanupClosePushL(s);
	aNewEntry.ExternalizeL(s);
	TPtr8 bufData = buf->Ptr(0);
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFAudioRecordControllerReplaceMetaDataEntry,
													 bufData,
													 indexPckg));
	CleanupStack::PopAndDestroy(2);//s, buf
	}

EXPORT_C CMMFAudioRecordControllerCustomCommandParser* CMMFAudioRecordControllerCustomCommandParser::NewL(MMMFAudioRecordControllerCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFAudioRecordControllerCustomCommandParser(aImplementor);
	}

CMMFAudioRecordControllerCustomCommandParser::CMMFAudioRecordControllerCustomCommandParser(MMMFAudioRecordControllerCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFAudioRecordController),
	iImplementor(aImplementor)
	{
	}

EXPORT_C CMMFAudioRecordControllerCustomCommandParser::~CMMFAudioRecordControllerCustomCommandParser()
	{
	}

void CMMFAudioRecordControllerCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFAudioRecordController)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFAudioRecordControllerCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFAudioRecordControllerGetRecordTimeAvailable:
		complete = DoGetRecordTimeAvailableL(aMessage);
		break;
	case EMMFAudioRecordControllerSetMaxDuration:
		complete = DoSetMaxDurationL(aMessage);
		break;
	case EMMFAudioRecordControllerSetMaxFileSize:
		complete = DoSetMaxFileSizeL(aMessage);
		break;
	case EMMFAudioRecordControllerCrop:
		complete = DoCropL(aMessage);
		break;
	case EMMFAudioRecordControllerAddMetaDataEntry:
		complete = DoAddMetaDataEntryL(aMessage);
		break;
	case EMMFAudioRecordControllerRemoveMetaDataEntry:
		complete = DoRemoveMetaDataEntryL(aMessage);
		break;
	case EMMFAudioRecordControllerReplaceMetaDataEntry:
		complete = DoReplaceMetaDataEntryL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFAudioRecordControllerCustomCommandParser::DoGetRecordTimeAvailableL(TMMFMessage& aMessage)
	{
	TTimeIntervalMicroSeconds time;
	iImplementor.MarcGetRecordTimeAvailableL(time);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iRecordTimeAvailable = time;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioRecordControllerCustomCommandParser::DoSetMaxDurationL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MarcSetMaxDurationL(pckg().iMaxDuration);
	return ETrue;
	}

TBool CMMFAudioRecordControllerCustomCommandParser::DoSetMaxFileSizeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MarcSetMaxFileSizeL(pckg().iMaxFileSize);
	return ETrue;
	}

TBool CMMFAudioRecordControllerCustomCommandParser::DoCropL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MarcCropL(pckg().iCropToEnd);
	return ETrue;
	}

TBool CMMFAudioRecordControllerCustomCommandParser::DoAddMetaDataEntryL(TMMFMessage& aMessage)
	{
	TInt bufSize = aMessage.SizeOfData1FromClient();
	// Leaving here in order to prevent a panic in the NewLC if the value is negative
	User::LeaveIfError(bufSize); 
	HBufC8* buf = HBufC8::NewLC(bufSize);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData1FromClientL(ptr);
	RDesReadStream stream;
	stream.Open(ptr);
	CleanupClosePushL(stream);
	CMMFMetaDataEntry* metaData = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(metaData);
	metaData->InternalizeL(stream);
	iImplementor.MarcAddMetaDataEntryL(*metaData);
	CleanupStack::PopAndDestroy(3);//metaData, stream, buf
	return ETrue;
	}

TBool CMMFAudioRecordControllerCustomCommandParser::DoRemoveMetaDataEntryL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MarcRemoveMetaDataEntryL(pckg());
	return ETrue;
	}

TBool CMMFAudioRecordControllerCustomCommandParser::DoReplaceMetaDataEntryL(TMMFMessage& aMessage)
	{
	// Get new meta data
	TInt bufSize = aMessage.SizeOfData1FromClient();
	// Leaving here in order to prevent a panic in the NewLC if the value is negative
	User::LeaveIfError(bufSize); 
	HBufC8* buf = HBufC8::NewLC(bufSize);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData1FromClientL(ptr);
	RDesReadStream stream;
	stream.Open(ptr);
	CleanupClosePushL(stream);
	CMMFMetaDataEntry* metaData = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(metaData);
	metaData->InternalizeL(stream);

	
	// Get index to replace
	TPckgBuf<TInt> indexPckg;
	aMessage.ReadData2FromClientL(indexPckg);
	
	iImplementor.MarcReplaceMetaDataEntryL(indexPckg(), *metaData);

	CleanupStack::PopAndDestroy(3);//metaData, stream, buf
	return ETrue;
	}

















EXPORT_C RMMFAudioControllerCustomCommands::RMMFAudioControllerCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFAudioController)
	{
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSourceSampleRate(TUint aSampleRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iSampleRate = aSampleRate;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSourceSampleRate, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSourceNumChannels(TUint aNumChannels) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iChannels = aNumChannels;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSourceNumChannels, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSourceFormat(TUid aFormatUid) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iFormatUid = aFormatUid;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSourceFormat, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSinkSampleRate(TUint aSampleRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iSampleRate = aSampleRate;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSinkSampleRate, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSinkNumChannels(TUint aNumChannels) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iChannels = aNumChannels;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSinkNumChannels, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSinkFormat(TUid aFormatUid) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iFormatUid = aFormatUid;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSinkFormat, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetCodec(TFourCC aSourceDataType, TFourCC aSinkDataType) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iSourceDataTypeCode = aSourceDataType;
	configPackage().iSinkDataTypeCode = aSinkDataType;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetCodec, 
										 configPackage,
										 KNullDesC8);
	}



EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSourceBitRate(TUint aRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iSampleRate = aRate;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSourceBitRate, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSourceDataType(TFourCC aDataType) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iSourceDataTypeCode = aDataType;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSourceDataType, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSinkBitRate(TUint aRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iSampleRate = aRate;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSinkBitRate, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::SetSinkDataType(TFourCC aDataType) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iSinkDataTypeCode = aDataType;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioControllerSetSinkDataType, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSourceSampleRate(TUint& aRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSourceSampleRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aRate = configPackage().iSampleRate;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSourceBitRate(TUint& aRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSourceBitRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aRate = configPackage().iSampleRate;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSourceNumChannels(TUint& aNumChannels) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSourceNumChannels, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aNumChannels = configPackage().iChannels;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSourceFormat(TUid& aFormat) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSourceFormat, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aFormat = configPackage().iFormatUid;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSourceDataType(TFourCC& aDataType) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSourceDataType, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aDataType = configPackage().iSourceDataTypeCode;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSinkSampleRate(TUint& aRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSinkSampleRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aRate = configPackage().iSampleRate;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSinkBitRate(TUint& aRate) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSinkBitRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aRate = configPackage().iSampleRate;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSinkNumChannels(TUint& aNumChannels) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSinkNumChannels, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aNumChannels = configPackage().iChannels;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSinkFormat(TUid& aFormat) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSinkFormat, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aFormat = configPackage().iFormatUid;
	return error;
	}

EXPORT_C TInt RMMFAudioControllerCustomCommands::GetSinkDataType(TFourCC& aDataType) const
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFAudioControllerGetSinkDataType, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aDataType = configPackage().iSinkDataTypeCode;
	return error;
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSourceSampleRatesL(RArray<TUint>& aSupportedRates) const
	{
	DoGetUintArrayL(aSupportedRates, EMMFAudioControllerGetSupportedSourceSampleRates);
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSourceBitRatesL(RArray<TUint>& aSupportedRates) const
	{
	DoGetUintArrayL(aSupportedRates, EMMFAudioControllerGetSupportedSourceBitRates);
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSourceNumChannelsL(RArray<TUint>& aSupportedChannels) const
	{
	DoGetUintArrayL(aSupportedChannels, EMMFAudioControllerGetSupportedSourceNumChannels);
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSourceDataTypesL(RArray<TFourCC>& aSupportedDataTypes) const
	{
	DoGetFourCCArrayL(aSupportedDataTypes, EMMFAudioControllerGetSupportedSourceDataTypes);
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedRates) const
	{
	DoGetUintArrayL(aSupportedRates, EMMFAudioControllerGetSupportedSinkSampleRates);
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSinkBitRatesL(RArray<TUint>& aSupportedRates) const
	{
	DoGetUintArrayL(aSupportedRates, EMMFAudioControllerGetSupportedSinkBitRates);
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels) const
	{
	DoGetUintArrayL(aSupportedChannels, EMMFAudioControllerGetSupportedSinkNumChannels);
	}

EXPORT_C void RMMFAudioControllerCustomCommands::GetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes) const
	{
	DoGetFourCCArrayL(aSupportedDataTypes, EMMFAudioControllerGetSupportedSinkDataTypes);
	}



void RMMFAudioControllerCustomCommands::DoGetUintArrayL(RArray<TUint>& aArray, TMMFAudioControllerMessages aIpc) const
	{
	aArray.Reset();

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 aIpc, 
													 KNullDesC8,
													 KNullDesC8,
													 numberOfElementsPckg));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TUint));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 EMMFAudioControllerCopyArrayData, 
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numberOfElementsPckg(); i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadUint32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}

void RMMFAudioControllerCustomCommands::DoGetFourCCArrayL(RArray<TFourCC>& aArray, TMMFAudioControllerMessages aIpc) const
	{
	aArray.Reset();

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 aIpc, 
													 KNullDesC8,
													 KNullDesC8,
													 numberOfElementsPckg));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TFourCC));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 EMMFAudioControllerCopyArrayData, 
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numberOfElementsPckg(); i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}














EXPORT_C CMMFAudioControllerCustomCommandParser* CMMFAudioControllerCustomCommandParser::NewL(MMMFAudioControllerCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFAudioControllerCustomCommandParser(aImplementor);
	}

CMMFAudioControllerCustomCommandParser::CMMFAudioControllerCustomCommandParser(MMMFAudioControllerCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFAudioController),
	iImplementor(aImplementor)
	{
	}

EXPORT_C CMMFAudioControllerCustomCommandParser::~CMMFAudioControllerCustomCommandParser()
	{
	delete iDataCopyBuffer;
	}

void CMMFAudioControllerCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFAudioController)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}


void CMMFAudioControllerCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFAudioControllerSetSourceSampleRate:
		complete = DoSetSourceSampleRateL(aMessage);
		break;
	case EMMFAudioControllerSetSourceBitRate:
		complete = DoSetSourceBitRateL(aMessage);
		break;
	case EMMFAudioControllerSetSourceNumChannels:
		complete = DoSetSourceNumChannelsL(aMessage);
		break;
	case EMMFAudioControllerSetSourceFormat:
		complete = DoSetSourceFormatL(aMessage);
		break;
	case EMMFAudioControllerSetSourceDataType:
		complete = DoSetSourceDataTypeL(aMessage);
		break;
	case EMMFAudioControllerSetSinkSampleRate:
		complete = DoSetSinkSampleRateL(aMessage);
		break;
	case EMMFAudioControllerSetSinkBitRate:
		complete = DoSetSinkBitRateL(aMessage);
		break;
	case EMMFAudioControllerSetSinkNumChannels:
		complete = DoSetSinkNumChannelsL(aMessage);
		break;
	case EMMFAudioControllerSetSinkFormat:
		complete = DoSetSinkFormatL(aMessage);
		break;
	case EMMFAudioControllerSetSinkDataType:
		complete = DoSetSinkDataTypeL(aMessage);
		break;
	case EMMFAudioControllerSetCodec:
		complete = DoSetCodecL(aMessage);
		break;
	case EMMFAudioControllerGetSourceSampleRate:
		complete = DoGetSourceSampleRateL(aMessage);
		break;
	case EMMFAudioControllerGetSourceBitRate:
		complete = DoGetSourceBitRateL(aMessage);
		break;
	case EMMFAudioControllerGetSourceNumChannels:
		complete = DoGetSourceNumChannelsL(aMessage);
		break;
	case EMMFAudioControllerGetSourceFormat:
		complete = DoGetSourceFormatL(aMessage);
		break;
	case EMMFAudioControllerGetSourceDataType:
		complete = DoGetSourceDataTypeL(aMessage);
		break;
	case EMMFAudioControllerGetSinkSampleRate:
		complete = DoGetSinkSampleRateL(aMessage);
		break;
	case EMMFAudioControllerGetSinkBitRate:
		complete = DoGetSinkBitRateL(aMessage);
		break;
	case EMMFAudioControllerGetSinkNumChannels:
		complete = DoGetSinkNumChannelsL(aMessage);
		break;
	case EMMFAudioControllerGetSinkFormat:
		complete = DoGetSinkFormatL(aMessage);
		break;
	case EMMFAudioControllerGetSinkDataType:
		complete = DoGetSinkDataTypeL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSourceSampleRates:
		complete = DoGetSupportedSourceSampleRatesL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSourceBitRates:
		complete = DoGetSupportedSourceBitRatesL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSourceNumChannels:
		complete = DoGetSupportedSourceNumChannelsL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSourceDataTypes:
		complete = DoGetSupportedSourceDataTypesL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSinkSampleRates:
		complete = DoGetSupportedSinkSampleRatesL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSinkBitRates:
		complete = DoGetSupportedSinkBitRatesL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSinkNumChannels:
		complete = DoGetSupportedSinkNumChannelsL(aMessage);
		break;
	case EMMFAudioControllerGetSupportedSinkDataTypes:
		complete = DoGetSupportedSinkDataTypesL(aMessage);
		break;
	case EMMFAudioControllerCopyArrayData:
		complete = DoCopyArrayDataL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSourceSampleRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSourceSampleRateL(pckg().iSampleRate);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSourceNumChannelsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSourceNumChannelsL(pckg().iChannels);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSourceFormatL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSourceFormatL(pckg().iFormatUid);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSinkSampleRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSinkSampleRateL(pckg().iSampleRate);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSinkNumChannelsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSinkNumChannelsL(pckg().iChannels);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSinkFormatL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSinkFormatL(pckg().iFormatUid);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetCodecL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetCodecL(pckg().iSourceDataTypeCode, pckg().iSinkDataTypeCode);
	return ETrue;
	}


TBool CMMFAudioControllerCustomCommandParser::DoSetSourceBitRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSourceBitRateL(pckg().iSampleRate);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSourceDataTypeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSourceDataTypeL(pckg().iSourceDataTypeCode);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSinkBitRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSinkBitRateL(pckg().iSampleRate);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoSetSinkDataTypeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MacSetSinkDataTypeL(pckg().iSinkDataTypeCode);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSourceSampleRateL(TMMFMessage& aMessage)
	{
	TUint rate = 0;
	iImplementor.MacGetSourceSampleRateL(rate);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iSampleRate = rate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSourceBitRateL(TMMFMessage& aMessage)
	{
	TUint rate = 0;
	iImplementor.MacGetSourceBitRateL(rate);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iSampleRate = rate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSourceNumChannelsL(TMMFMessage& aMessage)
	{
	TUint channels = 0;
	iImplementor.MacGetSourceNumChannelsL(channels);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iChannels = channels;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSourceFormatL(TMMFMessage& aMessage)
	{
	TUid format;
	iImplementor.MacGetSourceFormatL(format);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iFormatUid = format;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSourceDataTypeL(TMMFMessage& aMessage)
	{
	TFourCC fourCC;
	iImplementor.MacGetSourceDataTypeL(fourCC);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iSourceDataTypeCode = fourCC;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSinkSampleRateL(TMMFMessage& aMessage)
	{
	TUint rate = 0;
	iImplementor.MacGetSinkSampleRateL(rate);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iSampleRate = rate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSinkBitRateL(TMMFMessage& aMessage)
	{
	TUint rate = 0;
	iImplementor.MacGetSinkBitRateL(rate);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iSampleRate = rate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSinkNumChannelsL(TMMFMessage& aMessage)
	{
	TUint channels = 0;
	iImplementor.MacGetSinkNumChannelsL(channels);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iChannels = channels;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSinkFormatL(TMMFMessage& aMessage)
	{
	TUid format;
	iImplementor.MacGetSinkFormatL(format);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iFormatUid = format;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSinkDataTypeL(TMMFMessage& aMessage)
	{
	TFourCC fourCC;
	iImplementor.MacGetSinkDataTypeL(fourCC);
	TPckgBuf<TMMFAudioConfig> pckg;
	pckg().iSinkDataTypeCode = fourCC;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSourceSampleRatesL(TMMFMessage& aMessage)
	{
	RArray<TUint> rates;
	CleanupClosePushL(rates);
	iImplementor.MacGetSupportedSourceSampleRatesL(rates);

	DoCreateBufFromUintArrayL(rates);

	TPckgBuf<TInt> pckg;
	pckg() = rates.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//rates
	return ETrue;
	}

void CMMFAudioControllerCustomCommandParser::DoCreateBufFromUintArrayL(RArray<TUint>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;

	iDataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<aArray.Count();i++)
		stream.WriteUint32L(aArray[i]);
	CleanupStack::PopAndDestroy();//stream
	}

void CMMFAudioControllerCustomCommandParser::DoCreateBufFromFourCCArrayL(RArray<TFourCC>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;

	iDataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<aArray.Count();i++)
		{
		stream.WriteInt32L(aArray[i].FourCC());
		}
	CleanupStack::PopAndDestroy();//stream
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSourceBitRatesL(TMMFMessage& aMessage)
	{
	RArray<TUint> rates;
	CleanupClosePushL(rates);
	iImplementor.MacGetSupportedSourceBitRatesL(rates);

	DoCreateBufFromUintArrayL(rates);

	TPckgBuf<TInt> pckg;
	pckg() = rates.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//rates
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSourceNumChannelsL(TMMFMessage& aMessage)
	{
	RArray<TUint> array;
	CleanupClosePushL(array);
	iImplementor.MacGetSupportedSourceNumChannelsL(array);

	DoCreateBufFromUintArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSourceDataTypesL(TMMFMessage& aMessage)
	{
	RArray<TFourCC> array;
	CleanupClosePushL(array);
	iImplementor.MacGetSupportedSourceDataTypesL(array);

	DoCreateBufFromFourCCArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSinkSampleRatesL(TMMFMessage& aMessage)
	{
	RArray<TUint> array;
	CleanupClosePushL(array);
	iImplementor.MacGetSupportedSinkSampleRatesL(array);

	DoCreateBufFromUintArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSinkBitRatesL(TMMFMessage& aMessage)
	{
	RArray<TUint> array;
	CleanupClosePushL(array);
	iImplementor.MacGetSupportedSinkBitRatesL(array);

	DoCreateBufFromUintArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSinkNumChannelsL(TMMFMessage& aMessage)
	{
	RArray<TUint> array;
	CleanupClosePushL(array);
	iImplementor.MacGetSupportedSinkNumChannelsL(array);

	DoCreateBufFromUintArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoGetSupportedSinkDataTypesL(TMMFMessage& aMessage)
	{
	RArray<TFourCC> array;
	CleanupClosePushL(array);
	iImplementor.MacGetSupportedSinkDataTypesL(array);

	DoCreateBufFromFourCCArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}

TBool CMMFAudioControllerCustomCommandParser::DoCopyArrayDataL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		User::Leave(KErrNotReady);
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
	return ETrue;
	}




TBool CMMFVideoRecordControllerCustomCommandParser::DoCopyCDesC8ArrayDataL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		User::Leave(KErrNotReady);
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
	return ETrue;
	}
























EXPORT_C RMMFVideoControllerCustomCommands::RMMFVideoControllerCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoController)
	{
	}

EXPORT_C TInt RMMFVideoControllerCustomCommands::GetFrameRate(TReal32& aFramesPerSecond) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoControllerGetFrameRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aFramesPerSecond = configPackage().iFramesPerSecond;
	return error;
	}

EXPORT_C TInt RMMFVideoControllerCustomCommands::SetFrameRate(TReal32 aFramesPerSecond) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iFramesPerSecond = aFramesPerSecond;
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoControllerSetFrameRate, 
												configPackage,
												KNullDesC8);
	}


EXPORT_C void RMMFVideoPlayControllerCustomCommands::GetFrame(CFbsBitmap& aBitmap,TRequestStatus& aStatus)
	{
	TInt handle = aBitmap.Handle();
	
	iConfigPackage().iFrameBitmapServerHandle = handle;
	iController.CustomCommandAsync(iDestinationPckg, 
										 EMMFVideoPlayControllerGetFrame, 
										 iConfigPackage,
										 KNullDesC8,
										 aStatus);
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::UpdateDisplayRegion(const TRegion& aRegion) const
	{
	TPckgBuf<TInt> numberOfRectsPckg;
	numberOfRectsPckg() = aRegion.Count();
	const TRect* rects = aRegion.RectangleList();
	TPtrC8 rectMemory(REINTERPRET_CAST(TUint8*,(void*) rects), numberOfRectsPckg() * sizeof(TRect));

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlayControllerUpdateDisplayRegion, 
										 numberOfRectsPckg,
										 rectMemory);
	}


EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::DirectScreenAccessEvent(const TMMFDSAEvent aDSAEvent) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iDSAEvent = (TInt)aDSAEvent;
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerDSAEvent, 
												configPackage,
												KNullDesC8);
	}
	
	
EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::Play(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iStartPosition = aStart;
	configPackage().iEndPosition = aEnd;
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerPlay, 
												configPackage,
												KNullDesC8);
	}
	
EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::RefreshFrame() const
	{
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerRefreshFrame, 
												KNullDesC8,
												KNullDesC8);
	}
	
EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::GetLoadingProgress(TInt& aPercentageComplete) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerGetLoadingProgress, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aPercentageComplete = configPackage().iLoadingCompletePercentage;
	return error;
	}




EXPORT_C TInt RMMFVideoControllerCustomCommands::GetVideoFrameSize(TSize& aVideoFrameSize) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoControllerGetVideoFrameSize, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aVideoFrameSize = configPackage().iVideoFrameSize;
	return error;
	}

EXPORT_C TInt RMMFVideoControllerCustomCommands::GetAudioBitRate(TInt& aBitRate) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoControllerGetAudioBitRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aBitRate = configPackage().iAudioBitRate;
	return error;
	}

EXPORT_C TInt RMMFVideoControllerCustomCommands::GetVideoBitRate(TInt& aBitRate) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoControllerGetVideoBitRate, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aBitRate = configPackage().iVideoBitRate;
	return error;
	}
EXPORT_C TInt RMMFVideoControllerCustomCommands::GetAudioCodec(TFourCC& aCodec) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoControllerGetAudioCodec, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aCodec = configPackage().iAudioCodec;
	return error;
	}


EXPORT_C void Reserved1( void )
	{
	// dummy reserved function to replace GetVideoCodec() which was removed.
	// this should never be called so generate a panic
	Panic( ENoGetVideoCodec );
	}

EXPORT_C void Reserved2( void )
	{
	// dummy reserved function to replace GetSupportedSourceVideoTypes() which was removed.
	// this should never be called so generate a panic
	Panic( ENoGetSourceVideoTypes );
	}

EXPORT_C void Reserved3( void )
	{
	// dummy reserved function to replace GetSupportedSourceAudioTypes() which was removed.
	// this should never be called so generate a panic
	Panic( ENoGetSourceAudioTypes );
	}


EXPORT_C TInt RMMFVideoControllerCustomCommands::GetVideoMimeType(TDes8& aMimeType) const
	{
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoControllerGetVideoMimeType, 
										 KNullDesC8,										 
										 KNullDesC8,
										 aMimeType);
	return err;
	}


void RMMFVideoRecordControllerCustomCommands::DoGetFourCCArrayL(RArray<TFourCC>& aArray) const
	{
	aArray.Reset();

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 EMMFVideoRecordControllerGetSupportedSinkAudioTypes, 
													 KNullDesC8,
													 KNullDesC8,
													 numberOfElementsPckg));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TFourCC));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 EMMFVideoRecordControllerCopyFourCCArrayData, 
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numberOfElementsPckg(); i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2);//stream, buf
	}


void RMMFVideoRecordControllerCustomCommands::DoGetCDesC8ArrayL(CDesC8Array& aArray, TMMFVideoRecordControllerMessages aIpc) const
	{
	aArray.Reset();

	TPckgBuf<TMimeTypeBufferInfo> bufferInfoPckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 aIpc, 
													 KNullDesC8,
													 KNullDesC8,
													 bufferInfoPckg));

	// allocate a buffer of size dictated by server side
	HBufC8* buf = HBufC8::NewLC(bufferInfoPckg().bufferLen);
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 EMMFVideoRecordControllerCopyDescriptorArrayData, 
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	TInt32 len;

	for (TInt i=0; i < bufferInfoPckg().count; i++)
		{
		User::LeaveIfError(len = stream.ReadInt32L());

		HBufC8* tempDesc = HBufC8::NewLC(len);
		TPtr8 tempPtr = tempDesc->Des();

		stream.ReadL(tempPtr, len);
		aArray.AppendL(tempPtr);

		CleanupStack::PopAndDestroy(tempDesc);
		}

	CleanupStack::PopAndDestroy(2);//stream, buf

	}


EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::SetDisplayWindow(const TRect& aWindowRect,
																	  const TRect& aClipRect) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iWindowRect = aWindowRect;
 	configPackage().iClipRect = aClipRect;

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlayControllerSetDisplayWindow, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::GetAudioEnabled(TBool& aEnabled) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlayControllerGetAudioEnabled, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		aEnabled = configPackage().iAudioEnabled;
	return err;
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::Prepare()
	{
	return iController.CustomCommandSync(iDestinationPckg,
										EMMFVideoPlayControllerPrepare,
										KNullDesC8,
										KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::SetRotation(TVideoRotation aRotation) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iVideoRotation = aRotation;
	
	return iController.CustomCommandSync(iDestinationPckg,
										EMMFVideoPlayControllerSetRotation,
										configPackage,
										KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::GetRotation(TVideoRotation& aRotation) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlayControllerGetRotation, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		aRotation = configPackage().iVideoRotation;
	return err;
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::SetScaleFactor(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iWidthScalePercentage = aWidthPercentage;
	configPackage().iHeightScalePercentage = aHeightPercentage;
	configPackage().iAntiAliasFiltering = aAntiAliasFiltering;
	
	return iController.CustomCommandSync(iDestinationPckg,
										EMMFVideoPlayControllerSetScaleFactor,
										configPackage,
										KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::GetScaleFactor(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlayControllerGetScaleFactor, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		{
		aWidthPercentage = configPackage().iWidthScalePercentage;
		aHeightPercentage = configPackage().iHeightScalePercentage;
		aAntiAliasFiltering = configPackage().iAntiAliasFiltering;
		}
	return err;
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::SetCropRegion(const TRect& aCropRegion) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iCropRectangle = aCropRegion;
	
	return iController.CustomCommandSync(iDestinationPckg,
										EMMFVideoPlayControllerSetCropRegion,
										configPackage,
										KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerCustomCommands::GetCropRegion(TRect& aCropRegion) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlayControllerGetCropRegion, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		{
		aCropRegion = configPackage().iCropRectangle;
		}
	return err;
	}

EXPORT_C RMMFVideoRecordControllerCustomCommands::RMMFVideoRecordControllerCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoRecordController)
	{
	}


EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetVideoFormat(TUid aFormatUid) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iFormatUid = aFormatUid;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetVideoFormat, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetVideoCodec(const TDesC8& aVideoCodec) const
	{
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetVideoCodec, 
										 aVideoCodec,
										 KNullDesC8);
	}


EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetAudioCodec(TFourCC aAudioCodec) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iAudioCodec = aAudioCodec;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetAudioCodec, 
										 configPackage,
										 KNullDesC8);
	}


EXPORT_C void RMMFVideoRecordControllerCustomCommands::AddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry) const
	{
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize32);
	CleanupStack::PushL(buf);
	RBufWriteStream s;
	s.Open(*buf);
	CleanupClosePushL(s);
	aNewEntry.ExternalizeL(s);
	TPtr8 bufData = buf->Ptr(0);
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFVideoRecordControllerAddMetaDataEntry,
													 bufData,
													 KNullDesC8));
	CleanupStack::PopAndDestroy(2);//s, buf
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::RemoveMetaDataEntry(TInt aIndex) const
	{
	TPckgBuf<TInt> pckg(aIndex);
	return iController.CustomCommandSync(iDestinationPckg,
										 EMMFVideoRecordControllerRemoveMetaDataEntry,
										 pckg,
										 KNullDesC8);
	}

EXPORT_C void RMMFVideoRecordControllerCustomCommands::ReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry) const
	{
	TPckgBuf<TInt> indexPckg(aIndex);
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize32);
	CleanupStack::PushL(buf);
	RBufWriteStream s;
	s.Open(*buf);
	CleanupClosePushL(s);
	aNewEntry.ExternalizeL(s);
	TPtr8 bufData = buf->Ptr(0);
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFVideoRecordControllerReplaceMetaDataEntry,
													 bufData,
													 indexPckg));
	CleanupStack::PopAndDestroy(2);//s, buf
	}


EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetMaxFileSize(TInt aMaxSize) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	configPackage().iMaxFileSize = aMaxSize;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetMaxFileSize, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetVideoBitRate(TInt aRate) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	configPackage().iVideoBitRate = aRate;

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetVideoBitRate, 
										 configPackage,
										 KNullDesC8);

	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetAudioBitRate(TInt aRate) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	configPackage().iAudioBitRate = aRate;

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetAudioBitRate, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetVideoFrameSize(TSize aSize) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	configPackage().iVideoFrameSize = aSize;

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetVideoFrameSize, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetAudioEnabled(TBool aEnabled) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	configPackage().iAudioEnabled = aEnabled;

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetAudioEnabled, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::Prepare() const
	{

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerPrepare, 
										 KNullDesC8,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::SetCameraHandle(TInt aCameraHandle) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	configPackage().iCameraHandle = aCameraHandle;

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerSetCameraHandle, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::GetRecordTimeAvailable(TTimeIntervalMicroSeconds& aTime) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoRecordControllerGetRecordTimeAvailable, 
												KNullDesC8,
												KNullDesC8,
												configPackage);
	if (!error)
		aTime = configPackage().iRecordTimeAvailable;
	return error;
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::GetSupportedSinkVideoTypes(CDesC8Array& aSupportedDataTypes) const
	{
	TInt err;
	TRAP(err, DoGetCDesC8ArrayL(aSupportedDataTypes, EMMFVideoRecordControllerGetSupportedSinkVideoTypes));
	return err;
	}

EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::GetSupportedSinkAudioTypes(RArray<TFourCC>& aSupportedDataTypes) const
	{
	TInt err;
	TRAP(err, DoGetFourCCArrayL(aSupportedDataTypes));
	return err;
	}


// New method as part of INC23777.
EXPORT_C TInt RMMFVideoRecordControllerCustomCommands::GetAudioEnabled(TBool& aEnabled) const
	{
	TPckgBuf<TMMFVideoConfig> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoRecordControllerGetAudioEnabled, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		aEnabled = configPackage().iAudioEnabled;
	return err;
	}

EXPORT_C CMMFVideoControllerCustomCommandParser* CMMFVideoControllerCustomCommandParser::NewL(MMMFVideoControllerCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoControllerCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoControllerCustomCommandParser::~CMMFVideoControllerCustomCommandParser()
	{
	}

CMMFVideoControllerCustomCommandParser::CMMFVideoControllerCustomCommandParser(MMMFVideoControllerCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoController),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoControllerCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoController)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoControllerCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoControllerGetAudioBitRate:
		complete = DoGetAudioBitRateL(aMessage);
		break;
	case EMMFVideoControllerGetVideoBitRate:
		complete = DoGetVideoBitRateL(aMessage);
		break;
	case EMMFVideoControllerGetAudioCodec:
		complete = DoGetAudioCodecL(aMessage);
		break;
	case EMMFVideoControllerGetVideoFrameSize:
		complete = DoGetVideoFrameSizeL(aMessage);
		break;
	case EMMFVideoControllerSetFrameRate:
		complete = DoSetFrameRateL(aMessage);
		break;	
	case EMMFVideoControllerGetFrameRate:
		complete = DoGetFrameRateL(aMessage);
		break;
	case EMMFVideoControllerGetVideoMimeType:
		complete = DoGetVideoMimeTypeL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFVideoControllerCustomCommandParser::DoGetVideoFrameSizeL(TMMFMessage& aMessage)
	{
	TSize size;
	iImplementor.MvcGetVideoFrameSizeL(size);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iVideoFrameSize = size;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoControllerCustomCommandParser::DoGetAudioCodecL(TMMFMessage& aMessage)
	{
	TFourCC audioCodec;
	iImplementor.MvcGetAudioCodecL(audioCodec);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iAudioCodec = audioCodec;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoControllerCustomCommandParser::DoGetVideoBitRateL(TMMFMessage& aMessage)
	{
	TInt videoBitRate;
	iImplementor.MvcGetVideoBitRateL(videoBitRate);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iVideoBitRate = videoBitRate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoControllerCustomCommandParser::DoGetAudioBitRateL(TMMFMessage& aMessage)
	{
	TInt audioBitRate;
	iImplementor.MvcGetAudioBitRateL(audioBitRate);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iAudioBitRate = audioBitRate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoControllerCustomCommandParser::DoSetFrameRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvcSetFrameRateL(pckg().iFramesPerSecond);
	return ETrue;
	}

TBool CMMFVideoControllerCustomCommandParser::DoGetFrameRateL(TMMFMessage& aMessage)
	{
	TReal32 frameRate = 0;
	iImplementor.MvcGetFrameRateL(frameRate);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iFramesPerSecond = frameRate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoControllerCustomCommandParser::DoGetVideoMimeTypeL(TMMFMessage& aMessage)
	{
	TBuf8<KMaxMimeTypeLength> mimeType;
	iImplementor.MvcGetVideoMimeTypeL(mimeType);
	
	aMessage.WriteDataToClientL(mimeType);
	return ETrue;
	}


TBool CMMFVideoRecordControllerCustomCommandParser::DoGetSupportedSinkAudioTypesL(TMMFMessage& aMessage)
	{
	RArray<TFourCC> array;
	CleanupClosePushL(array);
	iImplementor.MvrcGetSupportedSinkAudioTypesL(array);

	DoCreateBufFromFourCCArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}


TBool CMMFVideoRecordControllerCustomCommandParser::DoGetSupportedSinkVideoTypesL(TMMFMessage& aMessage)
	{
	CDesC8ArrayFlat* array = new (ELeave) CDesC8ArrayFlat(KBufMimeTypeGranularity);
	CleanupStack::PushL(array);

	iImplementor.MvrcGetSupportedSinkVideoTypesL(*array);

	TInt32 len = DoCreateBufFromCDesC8ArrayL(*array);

	TPckgBuf<TMimeTypeBufferInfo> pckg;
	pckg().count = array->Count();
	pckg().bufferLen = len;
	
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy();//array
	return ETrue;
	}

void CMMFVideoRecordControllerCustomCommandParser::DoCreateBufFromFourCCArrayL(RArray<TFourCC>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;

	iDataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<aArray.Count();i++)
		{
		stream.WriteInt32L(aArray[i].FourCC());
		}
	CleanupStack::PopAndDestroy();//stream
	}

TInt32 CMMFVideoRecordControllerCustomCommandParser::DoCreateBufFromCDesC8ArrayL(CDesC8Array& aArray)
	{
	TInt32 bufferLen = 0;
	TInt32 len = 0;

	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;

	iDataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i = 0; i < aArray.Count(); i++)
		{
		len = aArray[i].Length();
		stream.WriteInt32L(len);
		stream.WriteL(aArray[i]);

		bufferLen += (len + sizeof(TInt32));;	// get a cumulative total buffer size
		}
	CleanupStack::PopAndDestroy();//stream

	return bufferLen;
	}


// --------------------------------------------------------------------------------
EXPORT_C RMMFVideoPlayControllerCustomCommands::RMMFVideoPlayControllerCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoPlayController)
	{
	}

EXPORT_C CMMFVideoPlayControllerCustomCommandParser* CMMFVideoPlayControllerCustomCommandParser::NewL(MMMFVideoPlayControllerCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoPlayControllerCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoPlayControllerCustomCommandParser::~CMMFVideoPlayControllerCustomCommandParser()
	{
	delete iVideoFrameMessage;
	}

CMMFVideoPlayControllerCustomCommandParser::CMMFVideoPlayControllerCustomCommandParser(MMMFVideoPlayControllerCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoPlayController),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoPlayControllerCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoPlayController)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoPlayControllerCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoPlayControllerGetFrame:
		complete = DoGetFrameL(aMessage);
		break;
	case EMMFVideoPlayControllerSetDisplayWindow:
		complete = DoSetDisplayWindowL(aMessage);
		break;
	case EMMFVideoPlayControllerGetAudioEnabled:
		complete = DoGetAudioEnabledL(aMessage);
		break;
	case EMMFVideoPlayControllerUpdateDisplayRegion:
		complete = DoUpdateDisplayRegionL(aMessage);
		break;
	case EMMFVideoPlayControllerDSAEvent:
		complete = DoDirectScreenAccessEventL(aMessage);
		break;
	case EMMFVideoPlayControllerPlay:
		complete = DoPlayL(aMessage);
		break;
	case EMMFVideoPlayControllerRefreshFrame:
		complete = DoRefreshFrameL(aMessage);
		break;
	case EMMFVideoPlayControllerGetLoadingProgress:
		complete = DoGetLoadingProgressL(aMessage);
		break;
	case EMMFVideoPlayControllerPrepare:
		complete = DoPrepareL(aMessage);
		break;
	case EMMFVideoPlayControllerSetRotation:
		complete = DoSetRotationL(aMessage);
		break;
	case EMMFVideoPlayControllerGetRotation:
		complete = DoGetRotationL(aMessage);
		break;
	case EMMFVideoPlayControllerSetScaleFactor:
		complete = DoSetScaleFactorL(aMessage);
		break;
	case EMMFVideoPlayControllerGetScaleFactor:
		complete = DoGetScaleFactorL(aMessage);
		break;
	case EMMFVideoPlayControllerSetCropRegion:
		complete = DoSetCropRegionL(aMessage);
		break;
	case EMMFVideoPlayControllerGetCropRegion:
		complete = DoGetCropRegionL(aMessage);
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoUpdateDisplayRegionL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> numberOfRectsPckg;
	aMessage.ReadData1FromClientL(numberOfRectsPckg);
	TUint rectSize = numberOfRectsPckg() * sizeof(TRect);
	TUint8* rectMemory = STATIC_CAST(TUint8*, User::AllocLC(rectSize));
	TPtr8 rectMemoryPtr(rectMemory,rectSize);
	aMessage.ReadData2FromClientL(rectMemoryPtr);
	TRect* rects = REINTERPRET_CAST(TRect*, rectMemory);
	RRegion region(numberOfRectsPckg(), rects);
	CleanupStack::Pop(rectMemory); // rectMemory now owned by region
	CleanupClosePushL(region);
	iImplementor.MvpcUpdateDisplayRegionL(region);
	CleanupStack::PopAndDestroy();//region

	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoGetFrameL(TMMFMessage& aMessage)
	{
	delete iVideoFrameMessage;
	iVideoFrameMessage = NULL;

	iVideoFrameMessage = CMMFVideoFrameMessage::NewL(aMessage);
	iImplementor.MvpcGetFrameL(*iVideoFrameMessage);
	return EFalse;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoSetDisplayWindowL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpcSetDisplayWindowL(pckg().iWindowRect, pckg().iClipRect);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoGetAudioEnabledL(TMMFMessage& aMessage)
	{
	TBool enabled;
	iImplementor.MvpcGetAudioEnabledL(enabled);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iAudioEnabled = enabled;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoDirectScreenAccessEventL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpcDirectScreenAccessEventL((TMMFDSAEvent)pckg().iDSAEvent);
	return ETrue;
	}
	
TBool CMMFVideoPlayControllerCustomCommandParser::DoPlayL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpcPlayL(pckg().iStartPosition, pckg().iEndPosition);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoRefreshFrameL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MvpcRefreshFrameL();
	return ETrue;
	}
	
TBool CMMFVideoPlayControllerCustomCommandParser::DoGetLoadingProgressL(TMMFMessage& aMessage)
	{
	TInt progress;
	iImplementor.MvpcGetLoadingProgressL(progress);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iLoadingCompletePercentage = progress;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoPrepareL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MvpcPrepare();
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoSetRotationL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpcSetRotationL(pckg().iVideoRotation);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoGetRotationL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	iImplementor.MvpcGetRotationL(pckg().iVideoRotation);
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoSetScaleFactorL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpcSetScaleFactorL(pckg().iWidthScalePercentage, pckg().iHeightScalePercentage, pckg().iAntiAliasFiltering);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoGetScaleFactorL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	iImplementor.MvpcGetScaleFactorL(pckg().iWidthScalePercentage, pckg().iHeightScalePercentage, pckg().iAntiAliasFiltering);
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoSetCropRegionL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpcSetCropRegionL(pckg().iCropRectangle);
	return ETrue;
	}

TBool CMMFVideoPlayControllerCustomCommandParser::DoGetCropRegionL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	iImplementor.MvpcGetCropRegionL(pckg().iCropRectangle);
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

// --------------------------------------------------------------------------------
EXPORT_C CMMFVideoRecordControllerCustomCommandParser* CMMFVideoRecordControllerCustomCommandParser::NewL(MMMFVideoRecordControllerCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoRecordControllerCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoRecordControllerCustomCommandParser::~CMMFVideoRecordControllerCustomCommandParser()
	{
	delete iDataCopyBuffer;
	}

CMMFVideoRecordControllerCustomCommandParser::CMMFVideoRecordControllerCustomCommandParser(MMMFVideoRecordControllerCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoRecordController),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoRecordControllerCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoRecordController)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoRecordControllerCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoRecordControllerSetVideoFormat:
		complete = DoSetVideoFormatL(aMessage);
		break;
	case EMMFVideoRecordControllerSetAudioBitRate:
		complete = DoSetAudioBitRateL(aMessage);
		break;
	case EMMFVideoRecordControllerSetVideoBitRate:
		complete = DoSetVideoBitRateL(aMessage);
		break;
	case EMMFVideoRecordControllerSetAudioCodec:
		complete = DoSetAudioCodecL(aMessage);
		break;
	case EMMFVideoRecordControllerSetVideoCodec:
		complete = DoSetVideoCodecL(aMessage);
		break;
	case EMMFVideoRecordControllerAddMetaDataEntry:
		complete = DoAddMetaDataEntryL(aMessage);
		break;
	case EMMFVideoRecordControllerRemoveMetaDataEntry:
		complete = DoRemoveMetaDataEntryL(aMessage);
		break;
	case EMMFVideoRecordControllerReplaceMetaDataEntry:
		complete = DoReplaceMetaDataEntryL(aMessage);
		break;
	case EMMFVideoRecordControllerSetMaxFileSize:
		complete = DoSetMaxFileSizeL(aMessage);
		break;
	case EMMFVideoRecordControllerSetVideoFrameSize:
		complete = DoSetVideoFrameSizeL(aMessage);
		break;
	case EMMFVideoRecordControllerSetAudioEnabled:
		complete = DoSetAudioEnabledL(aMessage);
		break;
	case EMMFVideoRecordControllerPrepare:
		complete = DoPrepareL(aMessage);
		break;
	case EMMFVideoRecordControllerSetCameraHandle:
		complete = DoSetCameraHandleL(aMessage);
		break;
	case EMMFVideoRecordControllerGetRecordTimeAvailable:
		complete = DoGetRecordTimeAvailableL(aMessage);
		break;
	case EMMFVideoRecordControllerGetSupportedSinkAudioTypes:
		complete = DoGetSupportedSinkAudioTypesL(aMessage);
		break;
	case EMMFVideoRecordControllerGetSupportedSinkVideoTypes:
		complete = DoGetSupportedSinkVideoTypesL(aMessage);
		break;
	case EMMFVideoRecordControllerCopyDescriptorArrayData:
		complete = DoCopyCDesC8ArrayDataL(aMessage);
		break;
	case EMMFVideoRecordControllerCopyFourCCArrayData:
		complete = DoCopyFourCCArrayDataL(aMessage);
		break;
	case EMMFVideoRecordControllerGetAudioEnabled:  //INC23777
		complete = DoGetAudioEnabledL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}





TBool CMMFVideoRecordControllerCustomCommandParser::DoSetVideoBitRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetVideoBitRateL(pckg().iVideoBitRate);
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoSetAudioBitRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetAudioBitRateL(pckg().iAudioBitRate);
	return ETrue;
	}


TBool CMMFVideoRecordControllerCustomCommandParser::DoSetVideoCodecL(TMMFMessage& aMessage)
	{
	TBuf8<KMaxMimeTypeLength> buf;
	aMessage.ReadData1FromClientL(buf);
	iImplementor.MvrcSetVideoCodecL(buf);
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoSetAudioCodecL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetAudioCodecL(pckg().iAudioCodec);
	return ETrue;
	}


TBool CMMFVideoRecordControllerCustomCommandParser::DoSetVideoFormatL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetVideoFormatL(pckg().iFormatUid);
	return ETrue;
	}




TBool CMMFVideoRecordControllerCustomCommandParser::DoAddMetaDataEntryL(TMMFMessage& aMessage)
	{
	TInt bufSize = aMessage.SizeOfData1FromClient();
	// Leaving here in order to prevent a panic in the NewLC if the value is negative
	User::LeaveIfError(bufSize); 
	HBufC8* buf = HBufC8::NewLC(bufSize);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData1FromClientL(ptr);
	RDesReadStream stream;
	stream.Open(ptr);
	CleanupClosePushL(stream);
	CMMFMetaDataEntry* metaData = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(metaData);
	metaData->InternalizeL(stream);
	iImplementor.MvrcAddMetaDataEntryL(*metaData);
	CleanupStack::PopAndDestroy(3);//metaData, stream, buf
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoRemoveMetaDataEntryL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcRemoveMetaDataEntryL(pckg());
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoReplaceMetaDataEntryL(TMMFMessage& aMessage)
	{
	// Get new meta data
	TInt bufSize = aMessage.SizeOfData1FromClient();
	// Leaving here in order to prevent a panic in the NewLC if the value is negative
	User::LeaveIfError(bufSize); 
	HBufC8* buf = HBufC8::NewLC(bufSize);
	TPtr8 ptr = buf->Des();
	aMessage.ReadData1FromClientL(ptr);
	RDesReadStream stream;
	stream.Open(ptr);
	CleanupClosePushL(stream);
	CMMFMetaDataEntry* metaData = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(metaData);
	metaData->InternalizeL(stream);

	// Get index to replace
	TPckgBuf<TInt> indexPckg;
	aMessage.ReadData2FromClientL(indexPckg);
	
	iImplementor.MvrcReplaceMetaDataEntryL(indexPckg(), *metaData);

	CleanupStack::PopAndDestroy(3);//metaData, stream, buf
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoSetMaxFileSizeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetMaxFileSizeL(pckg().iMaxFileSize);
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoSetVideoFrameSizeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetVideoFrameSizeL(pckg().iVideoFrameSize);
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoSetAudioEnabledL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetAudioEnabledL(pckg().iAudioEnabled);
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoPrepareL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MvrcPrepareL();
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoSetCameraHandleL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrcSetCameraHandleL(pckg().iCameraHandle);
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoGetRecordTimeAvailableL(TMMFMessage& aMessage)
	{
	TTimeIntervalMicroSeconds time;
	iImplementor.MvrcGetRecordTimeAvailableL(time);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iRecordTimeAvailable = time;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoRecordControllerCustomCommandParser::DoCopyFourCCArrayDataL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		User::Leave(KErrNotReady);
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
	return ETrue;
	}

//INC23777
TBool CMMFVideoRecordControllerCustomCommandParser::DoGetAudioEnabledL(TMMFMessage& aMessage)
	{
	TBool enabled;
	iImplementor.MvrcGetAudioEnabledL(enabled);
	TPckgBuf<TMMFVideoConfig> pckg;
	pckg().iAudioEnabled = enabled;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

//--------------------------------------------------------------------------------------
EXPORT_C CMMFVideoDRMExtCustomCommandParser* CMMFVideoDRMExtCustomCommandParser::NewL(MMMFVideoDRMExtCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoDRMExtCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoDRMExtCustomCommandParser::~CMMFVideoDRMExtCustomCommandParser()
	{
	delete iVideoFrameMessage;
	}

CMMFVideoDRMExtCustomCommandParser::CMMFVideoDRMExtCustomCommandParser(MMMFVideoDRMExtCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoDRMExt),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoDRMExtCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoDRMExt)
		{
		switch (aMessage.Function())
			{
		case EMMFVideoDRMExtGetFrame:
			TRAPD(err, DoGetFrameL(aMessage));
			if (err!=KErrNone) // asynchronous, so only complete message if error occurred
				aMessage.Complete(err);
			break;
		default:
			aMessage.Complete(KErrNotSupported);
			break;
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoDRMExtCustomCommandParser::DoGetFrameL(TMMFMessage& aMessage)
	{
	delete iVideoFrameMessage;
	iVideoFrameMessage = NULL;

	iVideoFrameMessage = CMMFVideoFrameMessage::NewL(aMessage);
	TPckgBuf<ContentAccess::TIntent> intentPckg;
	aMessage.ReadData2FromClientL(intentPckg);
	iImplementor.MvdeGetFrameL(*iVideoFrameMessage, intentPckg());
	}

EXPORT_C RMMFVideoDRMExtCustomCommands::RMMFVideoDRMExtCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoDRMExt)
	{
	}

EXPORT_C void RMMFVideoDRMExtCustomCommands::GetFrame(CFbsBitmap& aBitmap, ContentAccess::TIntent aIntent, TRequestStatus& aStatus)
	{
	iConfigPackage().iFrameBitmapServerHandle = aBitmap.Handle();
	iIntentPackage() = aIntent;
	iController.CustomCommandAsync(iDestinationPckg, 
								   EMMFVideoDRMExtGetFrame, 
								   iConfigPackage,
								   iIntentPackage,
								   aStatus);
	}

//------------------------------------------------------------------------------
EXPORT_C RMMFResourceNotificationCustomCommands::RMMFResourceNotificationCustomCommands(RMMFController& aController) : 
RMMFCustomCommandsBase(aController,KMMFEventCategoryAudioResourceAvailable)
	{
	}

EXPORT_C TInt RMMFResourceNotificationCustomCommands::RegisterAsClient(TUid aEventType,const TDesC8& aNotificationRegistrationData)
	{
	TPckgBuf<TMMFAudioConfig> configPackage;
	configPackage().iEventType = aEventType;
	configPackage().iNotificationRegistrationData = aNotificationRegistrationData;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFAudioResourceRegisterNotification, 
										configPackage,
										KNullDesC8);
	}
	
EXPORT_C TInt RMMFResourceNotificationCustomCommands::CancelRegisterAsClient(TUid aEventType)
	{
 	TPckgBuf<TMMFAudioConfig> configPackage;
 	configPackage().iEventType = aEventType;
 	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFAudioResourceCancelRegisterNotification, 
										configPackage,
										KNullDesC8);

	}

EXPORT_C TInt RMMFResourceNotificationCustomCommands::GetResourceNotificationData(TUid aEventType,TDes8& aNotificationData)
	{	
	TPckgBuf<TMMFAudioConfig> configPackage;
 	configPackage().iEventType = aEventType;
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFAudioResourceGetNotificationData, 
										configPackage,
										KNullDesC8,
										aNotificationData);
	}

EXPORT_C TInt RMMFResourceNotificationCustomCommands::WillResumePlay()
	{	
	return iController.CustomCommandSync(iDestinationPckg, 
										EMMFAudioResourceWillResumePlay, 
										KNullDesC8,
										KNullDesC8);	 
	}

EXPORT_C CMMFResourceNotificationCustomCommandParser* CMMFResourceNotificationCustomCommandParser::NewL(MMMFResourceNotificationCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFResourceNotificationCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFResourceNotificationCustomCommandParser::~CMMFResourceNotificationCustomCommandParser()
	{
	}

CMMFResourceNotificationCustomCommandParser::CMMFResourceNotificationCustomCommandParser( MMMFResourceNotificationCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KMMFEventCategoryAudioResourceAvailable),
	iImplementor(aImplementor)
	{
	}

void CMMFResourceNotificationCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KMMFEventCategoryAudioResourceAvailable)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			{
			aMessage.Complete(error);	
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFResourceNotificationCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFAudioResourceRegisterNotification:
		complete = DoRegisterAsClientL(aMessage);
		break;
	case EMMFAudioResourceCancelRegisterNotification:
		complete = DoCancelRegisterAsClientL(aMessage);
		break;
	case EMMFAudioResourceGetNotificationData:
		complete = DoGetResourceNotificationDataL(aMessage);
		break;
	case EMMFAudioResourceWillResumePlay:
		complete = DoWillResumePlayL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		{
		aMessage.Complete(KErrNone);	
		}
	}

EXPORT_C TBool CMMFResourceNotificationCustomCommandParser::DoRegisterAsClientL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MarnRegisterAsClientL(pckg().iEventType, pckg().iNotificationRegistrationData);
	return ETrue;
	}
	
EXPORT_C TBool CMMFResourceNotificationCustomCommandParser::DoCancelRegisterAsClientL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MarnCancelRegisterAsClientL(pckg().iEventType);
	return ETrue;
	}
	
EXPORT_C TBool CMMFResourceNotificationCustomCommandParser::DoGetResourceNotificationDataL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MarnGetResourceNotificationDataL(pckg().iEventType, pckg().iNotificationData);
	TPtrC8 tmp(pckg().iNotificationData);
	aMessage.WriteDataToClientL(pckg().iNotificationData);
	return ETrue;
	}

EXPORT_C TBool CMMFResourceNotificationCustomCommandParser::DoWillResumePlayL(TMMFMessage& aMessage)
	{
	iImplementor.MarnWillResumePlayL();
	aMessage.Complete(KErrNone);
	return EFalse;
	}

EXPORT_C RMMFVideoSetInitScreenCustomCommands::RMMFVideoSetInitScreenCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoSetInitScreen)
	{
	}

EXPORT_C TInt RMMFVideoSetInitScreenCustomCommands::SetInitScreenNumber(TInt aScreenNumber)
	{
	TPckgBuf<TInt> configPackage;
	configPackage() = aScreenNumber;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoSetInitScreenNumber,
										 configPackage,
										 KNullDesC8);
	}
		
EXPORT_C CMMFVideoSetInitScreenCustomCommandParser* CMMFVideoSetInitScreenCustomCommandParser::NewL(MMMFVideoSetInitScreenCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoSetInitScreenCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoSetInitScreenCustomCommandParser::~CMMFVideoSetInitScreenCustomCommandParser()
	{
	}

CMMFVideoSetInitScreenCustomCommandParser::CMMFVideoSetInitScreenCustomCommandParser(MMMFVideoSetInitScreenCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoSetInitScreen),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoSetInitScreenCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoSetInitScreen)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			{
			aMessage.Complete(error);	
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}
	
void CMMFVideoSetInitScreenCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoSetInitScreenNumber:
		complete = DoSetInitScreenNumberL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		{
		aMessage.Complete(KErrNone);	
		}
	}

TBool CMMFVideoSetInitScreenCustomCommandParser::DoSetInitScreenNumberL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvsdSetInitScreenNumber(pckg());
	return ETrue;
	}

_LIT(KMMFStandardCustomCommandsPanicCategory, "MMFStandardCustomCommands");
GLDEF_C void Panic(TMmfSCCPanic aError)
	{
	User::Panic(KMMFStandardCustomCommandsPanicCategory, aError);
	}


EXPORT_C RMMFVideoPixelAspectRatioCustomCommands::RMMFVideoPixelAspectRatioCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoPixelAspectRatio)
	{
	}

EXPORT_C TInt RMMFVideoPixelAspectRatioCustomCommands::SetPixelAspectRatio(const TVideoAspectRatio& aAspectRatio)
	{
	TPckgBuf<TVideoAspectRatio> configPackage;
	configPackage() = aAspectRatio;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoSetPixelAspectRatio,
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPixelAspectRatioCustomCommands::GetPixelAspectRatio(TVideoAspectRatio& aAspectRatio) const
	{
	TPckgBuf<TVideoAspectRatio> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoGetPixelAspectRatio, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		{
		aAspectRatio = configPackage();
		}		
	return err;
	}

EXPORT_C void RMMFVideoPixelAspectRatioCustomCommands::GetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios) const
	{
	DoGetVideoPixelAspectRatioArrayL(aAspectRatios, EMMFVideoGetSupportedPixelAspectRatios);
	}

void RMMFVideoPixelAspectRatioCustomCommands::DoGetVideoPixelAspectRatioArrayL(RArray<TVideoAspectRatio>& aArray, TMMFVideoPixelAspectRatioMessages aIpc) const
	{
	aArray.Reset();

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 aIpc, 
													 KNullDesC8,
													 KNullDesC8,
													 numberOfElementsPckg));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TVideoAspectRatio));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg,
													 EMMFVideoPixelAspectRatioCopyArrayData, 
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
	RDesReadStream stream(ptr);
	stream.Open(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numberOfElementsPckg(); i++)
		{
		User::LeaveIfError(aArray.Append(TVideoAspectRatio(stream.ReadInt32L(), stream.ReadInt32L())));
		}

	CleanupStack::PopAndDestroy(2, buf);//stream, buf
	}
		
EXPORT_C CMMFVideoPixelAspectRatioCustomCommandParser* CMMFVideoPixelAspectRatioCustomCommandParser::NewL(MMMFVideoPixelAspectRatioCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoPixelAspectRatioCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoPixelAspectRatioCustomCommandParser::~CMMFVideoPixelAspectRatioCustomCommandParser()
	{
	if(iDataCopyBuffer)
		{
		delete iDataCopyBuffer;
		}	
	}

CMMFVideoPixelAspectRatioCustomCommandParser::CMMFVideoPixelAspectRatioCustomCommandParser(MMMFVideoPixelAspectRatioCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoPixelAspectRatio),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoPixelAspectRatioCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoPixelAspectRatio)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			{
			aMessage.Complete(error);	
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}
	
void CMMFVideoPixelAspectRatioCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoSetPixelAspectRatio:
		complete = DoSetPixelAspectRatioL(aMessage);
		break;
	case EMMFVideoGetPixelAspectRatio:
		complete = DoGetPixelAspectRatioL(aMessage);
		break;
	case EMMFVideoGetSupportedPixelAspectRatios:
		complete = DoGetSupportedPixelAspectRatiosL(aMessage);
		break;
	case EMMFVideoPixelAspectRatioCopyArrayData:
		complete = DoCopyArrayDataL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		{
		aMessage.Complete(KErrNone);	
		}
	}

TBool CMMFVideoPixelAspectRatioCustomCommandParser::DoSetPixelAspectRatioL(TMMFMessage& aMessage)
	{
	TPckgBuf<TVideoAspectRatio> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvparSetPixelAspectRatioL(pckg());
	return ETrue;
	}

TBool CMMFVideoPixelAspectRatioCustomCommandParser::DoGetPixelAspectRatioL(TMMFMessage& aMessage)
	{
	TVideoAspectRatio aspectRatio;
	iImplementor.MvparGetPixelAspectRatioL(aspectRatio);
	TPckgBuf<TVideoAspectRatio> pckg;
	pckg() = aspectRatio;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoPixelAspectRatioCustomCommandParser::DoGetSupportedPixelAspectRatiosL(TMMFMessage& aMessage)
	{
	RArray<TVideoAspectRatio> array;
	CleanupClosePushL(array);
	iImplementor.MvparGetSupportedPixelAspectRatiosL(array);

	DoCreateBufFromVideoAspectRatioArrayL(array);

	TPckgBuf<TInt> pckg;
	pckg() = array.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy(&array);
	return ETrue;
	}

void CMMFVideoPixelAspectRatioCustomCommandParser::DoCreateBufFromVideoAspectRatioArrayL(RArray<TVideoAspectRatio>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;

	iDataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<aArray.Count();i++)
		{
		stream.WriteInt32L(aArray[i].iNumerator);
		stream.WriteInt32L(aArray[i].iDenominator);
		}
	CleanupStack::PopAndDestroy(&stream);
	}

TBool CMMFVideoPixelAspectRatioCustomCommandParser::DoCopyArrayDataL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		{
		User::Leave(KErrNotReady);
		}		
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
	return ETrue;
	}

EXPORT_C RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoAudioSamplingRateAndChannelConfig)
	{
	}

EXPORT_C TInt RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::SetAudioChannels(const TUint aNumChannels)
	{
	TPckgBuf<TUint> configPackage;
	configPackage() = aNumChannels;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoSetAudioChannels,
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::GetAudioChannels(TUint& aAudioChannels) const
	{
	TPckgBuf<TUint> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoGetAudioChannels, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		{
		aAudioChannels = configPackage();
		}		
	return err;
	}

EXPORT_C void RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::GetSupportedAudioChannelsL(RArray<TUint>& aChannels) const
	{
	DoGetUintArrayL(aChannels, EMMFVideoGetSupportedAudioChannels);
	}

EXPORT_C TInt RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::SetAudioSampleRate(const TUint aSampleRate)
	{
	TPckgBuf<TUint> configPackage;
	configPackage() = aSampleRate;
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoSetAudioSampleRate,
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::GetAudioSampleRate(TUint& aSampleRate) const
	{
	TPckgBuf<TUint> configPackage;
 	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoGetAudioSampleRate, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		{
		aSampleRate = configPackage();
		}		
	return err;
	}

EXPORT_C void RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::GetSupportedAudioSampleRatesL(RArray<TUint>& aSampleRates) const
	{
	DoGetUintArrayL(aSampleRates, EMMFVideoGetSupportedAudioSampleRates);
	}

void RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands::DoGetUintArrayL(RArray<TUint>& aArray, TMMFVideoAudioSamplingRateAndChannelConfigMessages aIpc) const
	{
	aArray.Reset();

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 aIpc, 
													 KNullDesC8,
													 KNullDesC8,
													 numberOfElementsPckg));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TUint));
	TPtr8 ptr = buf->Des();

	User::LeaveIfError(iController.CustomCommandSync(iDestinationPckg, 
													 EMMFVideoAudioSamplingRateAndChannelConfigCopyArrayData, 
													 KNullDesC8,
													 KNullDesC8,
													 ptr));
	RDesReadStream stream(ptr);
	stream.Open(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numberOfElementsPckg(); i++)
		{
		User::LeaveIfError(aArray.Append(stream.ReadUint32L()));
		}

	CleanupStack::PopAndDestroy(2, buf);//stream, buf
	}

		
EXPORT_C CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser* CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::NewL(MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::~CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser()
	{
	if(iDataCopyBuffer)
		{
		delete iDataCopyBuffer;
		}	
	}

CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser(MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoAudioSamplingRateAndChannelConfig),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoAudioSamplingRateAndChannelConfig)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			{
			aMessage.Complete(error);	
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}
	
void CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoSetAudioChannels:
		complete = DoSetAudioChannelsL(aMessage);
		break;
	case EMMFVideoGetAudioChannels:
		complete = DoGetAudioChannelsL(aMessage);
		break;
	case EMMFVideoGetSupportedAudioChannels:
		complete = DoGetSupportedAudioChannelsL(aMessage);
		break;
	case EMMFVideoSetAudioSampleRate:
		complete = DoSetAudioSampleRateL(aMessage);
		break;
	case EMMFVideoGetAudioSampleRate:
		complete = DoGetAudioSampleRateL(aMessage);
		break;
	case EMMFVideoGetSupportedAudioSampleRates:
		complete = DoGetSupportedAudioSampleRatesL(aMessage);
		break;
	case EMMFVideoAudioSamplingRateAndChannelConfigCopyArrayData:
		complete = DoCopyArrayDataL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		{
		aMessage.Complete(KErrNone);	
		}
	}


TBool CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoSetAudioChannelsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TUint> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvasrccSetAudioChannelsL(pckg());
	return ETrue;
	}
	
TBool CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoGetAudioChannelsL(TMMFMessage& aMessage)
	{
	TUint channels = 0;
	iImplementor.MvasrccGetAudioChannelsL(channels);
	TPckgBuf<TUint> pckg;
	pckg() = channels;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}
	
TBool CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoGetSupportedAudioChannelsL(TMMFMessage& aMessage)
	{
	RArray<TUint> audioChannels;
	CleanupClosePushL(audioChannels);
	iImplementor.MvasrccGetSupportedAudioChannelsL(audioChannels);

	DoCreateBufFromUintArrayL(audioChannels);

	TPckgBuf<TInt> pckg;
	pckg() = audioChannels.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy(&audioChannels);
	return ETrue;
	}

TBool CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoSetAudioSampleRateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TUint> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvasrccSetAudioSampleRateL(pckg());
	return ETrue;
	}

TBool CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoGetAudioSampleRateL(TMMFMessage& aMessage)
	{
	TUint sampleRate = 0;
	iImplementor.MvasrccGetAudioSampleRateL(sampleRate);
	TPckgBuf<TUint> pckg;
	pckg() = sampleRate;
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoGetSupportedAudioSampleRatesL(TMMFMessage& aMessage)
	{
	RArray<TUint> sampleRates;
	CleanupClosePushL(sampleRates);
	iImplementor.MvasrccGetSupportedAudioSampleRatesL(sampleRates);

	DoCreateBufFromUintArrayL(sampleRates);

	TPckgBuf<TInt> pckg;
	pckg() = sampleRates.Count();
	aMessage.WriteDataToClientL(pckg);

	CleanupStack::PopAndDestroy(&sampleRates);
	return ETrue;
	}

void CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoCreateBufFromUintArrayL(RArray<TUint>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;

	iDataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<aArray.Count();i++)
		{
		stream.WriteUint32L(aArray[i]);
		}		
	CleanupStack::PopAndDestroy(&stream);
	}

TBool CMMFVideoAudioSamplingRateAndChannelConfigCustomCommandParser::DoCopyArrayDataL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		{
		User::Leave(KErrNotReady);
		}		
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
	return ETrue;
	}


EXPORT_C CMMFVideoPlayControllerExtCustomCommandParser* CMMFVideoPlayControllerExtCustomCommandParser::NewL(MMMFVideoPlayControllerExtCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoPlayControllerExtCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoPlayControllerExtCustomCommandParser::~CMMFVideoPlayControllerExtCustomCommandParser()
	{
	}

CMMFVideoPlayControllerExtCustomCommandParser::CMMFVideoPlayControllerExtCustomCommandParser(MMMFVideoPlayControllerExtCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoPlayExt),
	iImplementor(aImplementor)	
	{
	}

void CMMFVideoPlayControllerExtCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoPlayExt)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			{
			aMessage.Complete(error);	
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoPlayControllerExtCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;

	switch (aMessage.Function())
		{
		case EMMFVideoPlayControllerSetPlayVelocity:
			complete = DoSetPlayVelocityL(aMessage);
			break;
		case EMMFVideoPlayControllerPlayVelocity:
			complete = DoPlayVelocityL(aMessage);
			break;
		case EMMFVideoPlayControllerStepFrame:
			complete = DoStepFrameL(aMessage);
			break;
		case EMMFVideoPlayControllerGetPlayRateCapabilities:
			complete = DoGetPlayRateCapabilitiesL(aMessage);
			break;
		case EMMFVideoPlayControllerSetVideoEnabled:
			complete = DoSetVideoEnabledL(aMessage);
			break;
		case EMMFVideoPlayControllerVideoEnabled:
			complete = DoVideoEnabledL(aMessage);
			break;
		case EMMFVideoPlayControllerSetAudioEnabled:
			complete = DoSetAudioEnabledL(aMessage);
			break;
		case EMMFVideoPlayControllerSetAutoScale:
			complete = DoSetAutoScaleL(aMessage);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	
	if (complete)
		{
		aMessage.Complete(KErrNone);
		}
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoSetPlayVelocityL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpecSetPlayVelocityL(pckg());
	
	return ETrue;
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoPlayVelocityL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	
	pckg() = iImplementor.MvpecPlayVelocityL();

	aMessage.WriteDataToClientL(pckg);
	
	return ETrue;
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoStepFrameL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpecStepFrameL(pckg());

	return ETrue;
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoGetPlayRateCapabilitiesL(TMMFMessage& aMessage)
	{
	TPckgBuf<TVideoPlayRateCapabilities> pckg;
	
	iImplementor.MvpecGetPlayRateCapabilitiesL(pckg());

	aMessage.WriteDataToClientL(pckg);
	
	return ETrue;
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoSetVideoEnabledL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpecSetVideoEnabledL(pckg());
	
	return ETrue;	
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoVideoEnabledL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckg;	
	
	pckg() = iImplementor.MvpecVideoEnabledL();

	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoSetAudioEnabledL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpecSetAudioEnabledL(pckg());
	
	return ETrue;
	}

TBool CMMFVideoPlayControllerExtCustomCommandParser::DoSetAutoScaleL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoPlayAutoScaleParams> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpecSetAutoScaleL(pckg().iScaleType,pckg().iHorizPos , pckg().iVertPos );
	
	return ETrue;
	}

EXPORT_C RMMFVideoPlayControllerExtCustomCommands::RMMFVideoPlayControllerExtCustomCommands(RMMFController& aController) :
RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoPlayExt)
	{
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::SetPlayVelocity(TInt aVelocity)
	{
	TPckgBuf<TInt> pckg(aVelocity);	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerSetPlayVelocity, 
												pckg,
												KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::PlayVelocity(TInt &aVelocity) const
	{
	TPckgBuf<TInt> pckg;	
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerPlayVelocity, 
												KNullDesC8,
												KNullDesC8,
												pckg);
	if (error == KErrNone)
		{	
		aVelocity = pckg();
		}
	return error;
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::StepFrame(TInt aStep)
	{
	TPckgBuf<TInt> pckg(aStep);	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerStepFrame, 
												pckg,
												KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::GetPlayRateCapabilities(TVideoPlayRateCapabilities& aCapabilities) const
	{
	TPckgBuf<TVideoPlayRateCapabilities> pckg;	
	TInt error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerGetPlayRateCapabilities, 
												KNullDesC8,
												KNullDesC8,
												pckg);
	if (!error)
		{
		aCapabilities = pckg();
		}
	return error;
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::SetVideoEnabled(TBool aVideoEnabled)
	{
	TPckgBuf<TBool> pckg(aVideoEnabled);	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerSetVideoEnabled, 
												pckg,
												KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::VideoEnabled(TBool &aVideoEnabled) const
	{
	TPckgBuf<TBool> pckg;	
	TInt error =  iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerVideoEnabled, 
												KNullDesC8,
												KNullDesC8,
												pckg);
	if (error == KErrNone)
		{
		aVideoEnabled = pckg();
		}
	return error;
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::SetAudioEnabled(TBool aAudioEnabled)
	{
	TPckgBuf<TBool> pckg(aAudioEnabled);	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerSetAudioEnabled, 
												pckg,
												KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlayControllerExtCustomCommands::SetAutoScale(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
	TPckgBuf<TMMFVideoPlayAutoScaleParams> pckg;	

	pckg().iScaleType = aScaleType;
	pckg().iHorizPos = aHorizPos;
	pckg().iVertPos = aVertPos;
	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoPlayControllerSetAutoScale, 
												pckg,
												KNullDesC8);
	}


EXPORT_C CMMFVideoRecordControllerExtCustomCommandParser* CMMFVideoRecordControllerExtCustomCommandParser::NewL(MMMFVideoRecordControllerExtCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoRecordControllerExtCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoRecordControllerExtCustomCommandParser::~CMMFVideoRecordControllerExtCustomCommandParser()
	{
	}

CMMFVideoRecordControllerExtCustomCommandParser::CMMFVideoRecordControllerExtCustomCommandParser(MMMFVideoRecordControllerExtCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoRecorderExt),
	iImplementor(aImplementor)	
	{
	}

void CMMFVideoRecordControllerExtCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoRecorderExt)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			{
			aMessage.Complete(error);	
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoRecordControllerExtCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoRecordControllerSetVideoEnabled:
		complete = DoSetVideoEnabledL(aMessage);
		break;
	case EMMFVideoRecordControllerVideoEnabled:
		complete = DoVideoEnabledL(aMessage);
		break;
	case EMMFVideoRecordControllerSetVideoQuality:
		complete = DoSetVideoQualityL(aMessage);
		break;
	case EMMFVideoRecordControllerVideoQuality:
		complete = DoVideoQualityL(aMessage);
		break;
	case EMMFVideoRecordControllerSetVideoFrameRateFixed:
		complete = DoSetVideoFrameRateFixedL(aMessage);
		break;
	case EMMFVideoRecordControllerVideoFrameRateFixed:
		complete = DoVideoFrameRateFixedL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		{
		aMessage.Complete(KErrNone);
		}
	}

TBool CMMFVideoRecordControllerExtCustomCommandParser::DoSetVideoEnabledL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrecSetVideoEnabledL(pckg());
	
	return ETrue;
	}
	
TBool CMMFVideoRecordControllerExtCustomCommandParser::DoVideoEnabledL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;	
	
	pckg() = iImplementor.MvrecVideoEnabledL();
	
	aMessage.WriteDataToClientL(pckg);
	
	return ETrue;
	}
	
TBool CMMFVideoRecordControllerExtCustomCommandParser::DoSetVideoQualityL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrecSetVideoQualityL(pckg());
	
	return ETrue;
	}
	
TBool CMMFVideoRecordControllerExtCustomCommandParser::DoVideoQualityL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	
	pckg() = iImplementor.MvrecVideoQualityL();
	
	aMessage.WriteDataToClientL(pckg);
	
	return ETrue;
	}
	
TBool CMMFVideoRecordControllerExtCustomCommandParser::DoSetVideoFrameRateFixedL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckg;
	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvrecSetVideoFrameRateFixedL(pckg());
	
	return ETrue;	
	}
	
TBool CMMFVideoRecordControllerExtCustomCommandParser::DoVideoFrameRateFixedL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckg;	
	
	pckg() = iImplementor.MvrecVideoFrameRateFixedL();
	
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}


EXPORT_C RMMFVideoRecordControllerExtCustomCommands::RMMFVideoRecordControllerExtCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoRecorderExt)
	{
	}

EXPORT_C TInt RMMFVideoRecordControllerExtCustomCommands::SetVideoEnabled(TBool aEnabled)
	{
	TPckgBuf<TBool> pckg(aEnabled);	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoRecordControllerSetVideoEnabled, 
												pckg,
												KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerExtCustomCommands::VideoEnabled(TBool &aEnabled) const
	{
	TPckgBuf<TBool> pckg(EFalse);	
	TInt error;

	error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoRecordControllerVideoEnabled, 
												KNullDesC8,
												KNullDesC8,
												pckg);
	if (error == KErrNone)
		{
		aEnabled = pckg();
		}
	return error;
	}

EXPORT_C TInt RMMFVideoRecordControllerExtCustomCommands::SetVideoQuality(TInt aQuality)
	{
	TPckgBuf<TInt> pckg(aQuality);	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoRecordControllerSetVideoQuality, 
												pckg,
												KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerExtCustomCommands::VideoQuality(TInt &aQuality) const
	{
	TPckgBuf<TInt> pckg;	
	TInt error;

	error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoRecordControllerVideoQuality, 
												KNullDesC8,
												KNullDesC8,
												pckg);
	if (error == KErrNone)
		{
		aQuality = pckg();
		}
	return error;
	}

EXPORT_C TInt RMMFVideoRecordControllerExtCustomCommands::SetVideoFrameRateFixed(TBool aFixedFrameRate)
	{
	TPckgBuf<TBool> pckg(aFixedFrameRate);	
	return iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoRecordControllerSetVideoFrameRateFixed, 
												pckg,
												KNullDesC8);
	}

EXPORT_C TInt RMMFVideoRecordControllerExtCustomCommands::VideoFrameRateFixed(TBool &aFixedFrameRate) const
	{
	TPckgBuf<TBool> pckg;	
	TInt error;

	error = iController.CustomCommandSync(iDestinationPckg, 
												EMMFVideoRecordControllerVideoFrameRateFixed, 
												KNullDesC8,
												KNullDesC8,
												pckg);
	if (error == KErrNone)
		{
		aFixedFrameRate = pckg();
		}
	return error;
	}

EXPORT_C TMMFAudioSetRepeatsConfig::TMMFAudioSetRepeatsConfig()
	:iRepeatNumberOfTimes(0), iTrailingSilence(0), iReserved1(0)
	{
	}
	
EXPORT_C RMMFAudioPlayControllerSetRepeatsCustomCommands::RMMFAudioPlayControllerSetRepeatsCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFAudioPlaySetRepeatsController)
	{
	}
	
EXPORT_C TInt RMMFAudioPlayControllerSetRepeatsCustomCommands::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	TPckgBuf<TMMFAudioSetRepeatsConfig> configPackage;
	configPackage().iRepeatNumberOfTimes = aRepeatNumberOfTimes;
	configPackage().iTrailingSilence = aTrailingSilence;
	
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFAudioPlayControllerSetRepeats, 
										 configPackage,
										 KNullDesC8);
	}
	
EXPORT_C CMMFAudioPlayControllerSetRepeatsCustomCommandParser* CMMFAudioPlayControllerSetRepeatsCustomCommandParser::NewL(MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFAudioPlayControllerSetRepeatsCustomCommandParser(aImplementor);
	}

CMMFAudioPlayControllerSetRepeatsCustomCommandParser::CMMFAudioPlayControllerSetRepeatsCustomCommandParser(MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFAudioPlaySetRepeatsController),	iImplementor(aImplementor)
	{
	}

EXPORT_C CMMFAudioPlayControllerSetRepeatsCustomCommandParser::~CMMFAudioPlayControllerSetRepeatsCustomCommandParser()
	{
	}

void CMMFAudioPlayControllerSetRepeatsCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFAudioPlaySetRepeatsController)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			{
			aMessage.Complete(error);
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFAudioPlayControllerSetRepeatsCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFAudioPlayControllerSetRepeats:
		complete = DoSetRepeatsL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		{
		aMessage.Complete(KErrNone);
		}
	}

TBool CMMFAudioPlayControllerSetRepeatsCustomCommandParser::DoSetRepeatsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFAudioSetRepeatsConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	User::LeaveIfError(iImplementor.MapcSetRepeats(pckg().iRepeatNumberOfTimes, pckg().iTrailingSilence));
	return ETrue;
	}
