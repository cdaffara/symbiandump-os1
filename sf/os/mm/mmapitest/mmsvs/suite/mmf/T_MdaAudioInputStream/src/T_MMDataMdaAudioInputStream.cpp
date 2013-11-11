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


#include "T_MMDataMdaAudioInputStream.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KBufferSize,				"buffersize");
_LIT(KOutputFile,				"outputfile");
_LIT(KBalance,					"balance");
_LIT(KBitRate,					"bitrate");
_LIT(KGain,						"gain");
_LIT(KAudioCapsP1,				"audioCapsP1");
_LIT(KAudioCapsP2,				"audioCapsP2");

_LIT(KCmdDestructor,			"~");
_LIT(KCmdBitRateL,				"BitRateL");
_LIT(KCmdCustomInterface,		"CustomInterface");
_LIT(KCmdDataType,				"DataType");
_LIT(KCmdGain,					"Gain");
_LIT(KCmdGetBalanceL,			"GetBalanceL");
_LIT(KCmdGetBytes,				"GetBytes");
_LIT(KCmdGetSupportedBitRatesL,	"GetSupportedBitRatesL");
_LIT(KCmdMaxGain,				"MaxGain");
_LIT(KCmdNewL,					"NewL");
_LIT(KCmdOpen,					"Open");
_LIT(KCmdPosition,				"Position");
_LIT(KCmdReadL,					"ReadL");
_LIT(KCmdSetAudioPropertiesL,	"SetAudioPropertiesL");
_LIT(KCmdSetBalanceL,			"SetBalanceL");
_LIT(KCmdSetBitRateL,			"SetBitRateL");
_LIT(KCmdSetDataTypeL,			"SetDataTypeL");
_LIT(KCmdSetGain,				"SetGain");
_LIT(KCmdSetPriority,			"SetPriority");
_LIT(KCmdStop,					"Stop");
/*@}*/

const TInt	KMdaAudioInputStreamBufferSize=1024;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataMdaAudioInputStream* CT_MMDataMdaAudioInputStream::NewL()
	{
	CT_MMDataMdaAudioInputStream*	ret=new (ELeave) CT_MMDataMdaAudioInputStream();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataMdaAudioInputStream::CT_MMDataMdaAudioInputStream()
:	iInputStream(NULL)
,	iBuffer(NULL)
,	iBufferPtr(NULL)
,	iBufferSize(KMdaAudioInputStreamBufferSize)
,	iStreamOpened(EFalse)
,	iFileOpened(EFalse)
,	iIsAllDataRead(EFalse)
,	iAsyncErrorIndex(0)
	{
	}

void CT_MMDataMdaAudioInputStream::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	User::LeaveIfError(iFs.Connect());
	}

CT_MMDataMdaAudioInputStream::~CT_MMDataMdaAudioInputStream()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

TAny* CT_MMDataMdaAudioInputStream::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iInputStream;
	}

void CT_MMDataMdaAudioInputStream::DestroyData()
	{
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	iFs.Close();
	delete iBufferPtr;
	iBufferPtr=NULL;
	delete iBuffer;
	iBuffer=NULL;
	delete iInputStream;
	iInputStream=NULL;
	iAsyncErrorIndex = 0;
	}

TBool CT_MMDataMdaAudioInputStream::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdBitRateL )
		{
		DoCmdBitRateL(aSection);
		}
	else if ( aCommand==KCmdCustomInterface )
		{
		DoCmdCustomInterface();
		}
	else if ( aCommand==KCmdDataType )
		{
		DoCmdDataTypeL(aSection);
		}
	else if ( aCommand==KCmdGain )
		{
		DoCmdGain(aSection);
		}
	else if ( aCommand==KCmdGetBalanceL )
		{
		DoCmdGetBalanceL(aSection);
		}
	else if ( aCommand==KCmdGetBytes )
		{
		DoCmdGetBytes();
		}
	else if ( aCommand==KCmdGetSupportedBitRatesL )
		{
		DoCmdGetSupportedBitRatesL();
		}
	else if ( aCommand==KCmdMaxGain )
		{
		DoCmdMaxGain();
		}
	else if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdOpen )
		{
		DoCmdOpen(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdPosition )
		{
		DoCmdPosition();
		}
	else if ( aCommand==KCmdReadL )
		{
		DoCmdReadL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSetAudioPropertiesL )
		{
		DoCmdSetAudioPropertiesL(aSection);
		}
	else if ( aCommand==KCmdSetBalanceL )
		{
		DoCmdSetBalanceL(aSection);
		}
	else if ( aCommand==KCmdSetBitRateL )
		{
		DoCmdSetBitRateL(aSection);
		}
	else if ( aCommand==KCmdSetDataTypeL )
		{
		DoCmdSetDataTypeL(aSection);
		}
	else if ( aCommand==KCmdSetGain )
		{
		DoCmdSetGain(aSection);
		}
	else if ( aCommand==KCmdSetPriority )
		{
		DoCmdSetPriority(aSection);
		}
	else if ( aCommand==KCmdStop )
		{
		DoCmdStop();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_MMDataMdaAudioInputStream::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_MMDataMdaAudioInputStream::DoCmdBitRateL(const TDesC& aSection)
	{

	TInt	bitRate=0;
	TRAPD(err, bitRate=iInputStream->BitRateL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("BitRateL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("BitRateL=%d"), bitRate);
		TInt	expectedBitRate=0;
		if(GetIntFromConfig(aSection, KBitRate(), expectedBitRate))
			{
			if(bitRate != expectedBitRate)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("bitRate(%d) != expectedBitRate(%d)"), bitRate, expectedBitRate);
				}
			}
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdCustomInterface()
	{
	TAny*	any=iInputStream->CustomInterface(KNullUid);
	if ( any!=NULL )
		{
		ERR_PRINTF2(_L("Non-NULL return value %0x"), any);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdDataTypeL(const TDesC& aSection)
	{
	TFourCC	actualVal=iInputStream->DataType();
	INFO_PRINTF2(_L("DataType=%d"), actualVal.FourCC());

	TFourCC	expectedVal;
	TBool	hasFourCC = CT_MMUtil::ReadFourCC(*this, aSection, expectedVal);
	if ( hasFourCC )
		{
		if( expectedVal != actualVal )
			{
			ERR_PRINTF3(_L("dataType(%d) != expectedDataType(%d)"), actualVal.FourCC(), expectedVal.FourCC());
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdGain(const TDesC& aSection)
	{
	TInt	gain=iInputStream->Gain();
	INFO_PRINTF2(_L("Gain=%d"), gain);

	TInt	expectedGain=0;
	if(GetIntFromConfig(aSection, KGain(), expectedGain))
		{
		TInt	maxGain=iInputStream->MaxGain();
		if(expectedGain>maxGain)
			{
			expectedGain=maxGain;
			}
		if(expectedGain == -1)
			{
			// Expect to get the maximum gain
			if(gain != maxGain)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("gain(%d) != maxGain(%d)"), gain, maxGain);
				}
			}
		else if(gain != expectedGain)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("gain(%d) != expectedGain(%d)"), gain, expectedGain);
			}
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdGetBalanceL(const TDesC& aSection)
	{
	TInt	balance=0;
	TRAPD(err, balance=iInputStream->GetBalanceL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetBalanceL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("GetBalanceL=%d"), balance);
		TInt	expectedBalance=0;
		if(GetIntFromConfig(aSection, KBalance(), expectedBalance))
			{
			if(balance != expectedBalance)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("balance(%d) != expectedBalance(%d)"), balance, expectedBalance);
				}
			}
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdGetBytes()
	{
	TInt	bytes=iInputStream->GetBytes();
	INFO_PRINTF2(_L("GetBytes=%d"), bytes);
	}

void CT_MMDataMdaAudioInputStream::DoCmdGetSupportedBitRatesL()
	{
	RArray<TInt>	rates;
	CleanupClosePushL(rates);
	TRAPD(err, iInputStream->GetSupportedBitRatesL(rates));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetSupportedBitRatesL Error %d"), err);
		SetError(err);
		}
	for ( TInt index=0; index<rates.Count(); ++index )
		{
		INFO_PRINTF3(_L("rates[%d]=%d"), index, rates[index]);
		}
	CleanupStack::PopAndDestroy(&rates);
	}

void CT_MMDataMdaAudioInputStream::DoCmdMaxGain()
	{
	TInt	maxGain=iInputStream->MaxGain();
	INFO_PRINTF2(_L("MaxGain=%d"), maxGain);
	}

void CT_MMDataMdaAudioInputStream::DoCmdNewL(const TDesC& aSection)
	{
	delete iInputStream;
	iInputStream=NULL;

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	priority;
	if ( CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		TRAPD(err, iInputStream=CMdaAudioInputStream::NewL(*this, priority, priorityPreference));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this, priority, priorityPreference) Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		TRAPD(err, iInputStream=CMdaAudioInputStream::NewL(*this));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this) Error %d"), err);
			SetError(err);
			}
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdOpen(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	//	Open the stream
	TMdaAudioDataSettings	settings;
	iAsyncErrorIndex = aAsyncErrorIndex;
	if ( CT_MMUtil::ReadAudioDataSettings(*this, settings, aSection) )
		{
		iInputStream->Open(&settings);
		}
	else
		{
		iInputStream->Open(NULL);
		}
	IncOutstanding();
	}

void CT_MMDataMdaAudioInputStream::DoCmdPosition()
	{
	TTimeIntervalMicroSeconds	position=iInputStream->Position();
	INFO_PRINTF2(_L("Position=%d"), position.Int64());
	}

void CT_MMDataMdaAudioInputStream::DoCmdReadL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	GetIntFromConfig(aSection, KBufferSize(), iBufferSize);
	delete iBuffer;
	iBuffer=NULL;
	iBuffer=HBufC8::NewL(iBufferSize);
	iAsyncErrorIndex = aAsyncErrorIndex;

	TPtrC	outputFile;
	if ( GetStringFromConfig(aSection, KOutputFile(), outputFile) )
		{
		iFs.MkDirAll(outputFile);
		iFs.Delete(outputFile);
		TInt	err=iFile.Create(iFs, outputFile, EFileWrite);
		if ( err==KErrNone )
			{
			INFO_PRINTF1(_L("File created"));
			iFileOpened=ETrue;
			IncOutstanding();
			ReadStreamAudioDataL();
			}
		else
			{
			ERR_PRINTF3(_L("Cannot open outputfile %S. Error %d"), &outputFile, err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("No %S"), &KOutputFile());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdSetAudioPropertiesL(const TDesC& aSection)
	{
	TPtrC	paramStr;

	TMdaAudioDataSettings::TAudioCaps	sampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
	TBool unknownSampleRate = EFalse;
	if( GetStringFromConfig(aSection, KAudioCapsP1(), paramStr))
		{
		if( !CT_MMUtil::ConvertToAudioCaps(paramStr, sampleRate) )
			{
			TInt otherSampleRate = 0;
			if ( GetIntFromConfig(aSection, KAudioCapsP1(), otherSampleRate) )
				{
				sampleRate = (TMdaAudioDataSettings::TAudioCaps) otherSampleRate;
				unknownSampleRate = ETrue;
				}
			else
				{
				ERR_PRINTF2(_L("SetAudioPropertiesL cannot parse %S"), &KAudioCapsP1());
				SetBlockResult(EFail);
				}
			}
		}

	TMdaAudioDataSettings::TAudioCaps channels = TMdaAudioDataSettings::EChannelsMono;
	TBool unknownChannels = EFalse;
	if( GetStringFromConfig(aSection, KAudioCapsP2(), paramStr))
		{
		if( !CT_MMUtil::ConvertToAudioCaps(paramStr, channels) )
			{
			TInt otherChannels = 0;
			if ( GetIntFromConfig(aSection, KAudioCapsP2(), otherChannels) )
				{
				channels = (TMdaAudioDataSettings::TAudioCaps) otherChannels;
				unknownChannels = ETrue;
				}
			else
				{
				ERR_PRINTF2(_L("SetAudioPropertiesL cannot parse %S"), &KAudioCapsP2());
				SetBlockResult(EFail);
				}
			}
		}

	TRAPD(err, iInputStream->SetAudioPropertiesL((TInt)sampleRate, (TInt)channels));
	if (( err==KErrNotSupported ) && ( !unknownSampleRate ) && ( !unknownChannels ))
		{
		WARN_PRINTF1(_L("SetAudioPropertiesL not supported"));
		}
	else if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetAudioPropertiesL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdSetBalanceL(const TDesC& aSection)
	{
	TInt	balance=KMMFBalanceCenter;
	GetIntFromConfig(aSection, KBalance(), balance);
	TRAPD(err, iInputStream->SetBalanceL(balance));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetBalanceL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdSetBitRateL(const TDesC& aSection)
	{
	TInt	bitRate=48000 * 16;
	GetIntFromConfig(aSection, KBitRate(), bitRate);
	TRAPD(err, iInputStream->SetBitRateL(bitRate));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetBitRateL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdSetDataTypeL(const TDesC& aSection)
	{
	TFourCC	val;
	if ( CT_MMUtil::ReadFourCC(*this, aSection, val) )
		{
		TRAPD(err, iInputStream->SetDataTypeL(val));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("SetDataTypeL Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1(_L("No data type"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioInputStream::DoCmdSetGain(const TDesC& aSection)
	{
	TInt	gain=0;
	GetIntFromConfig(aSection, KGain(), gain);
	iInputStream->SetGain(gain);
	}

void CT_MMDataMdaAudioInputStream::DoCmdSetPriority(const TDesC& aSection)
	{
	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityMin;
		}
	iInputStream->SetPriority(priority, priorityPreference);
	}

void CT_MMDataMdaAudioInputStream::DoCmdStop()
	{
	iInputStream->Stop();
	}

/**
aAsyncErrorIndex callback called when CMdaAudioInputStream::Open completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if opened ok
@pre N/A
@post N/A
*/
void CT_MMDataMdaAudioInputStream::MaiscOpenComplete(TInt aError)
	{
	if ( aError==KErrNone )
		{
		iStreamOpened=ETrue;
		INFO_PRINTF1(_L("Open OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Open Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		iStreamOpened=EFalse;
		}
	DecOutstanding();
	}

/**
aAsyncErrorIndex callback called when CMdaAudioInputStream::Stop completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if stopped ok
@pre N/A
@post N/A
*/
void CT_MMDataMdaAudioInputStream::MaiscRecordComplete(TInt aError)
	{
	if ( aError==KErrNone )
		{
		INFO_PRINTF1(_L("Record OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Record Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	DecOutstanding();
	}

/**
aAsyncErrorIndex callback called when CMdaAudioInputStream::ReadL completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if read ok
@pre N/A
@post N/A
*/
void CT_MMDataMdaAudioInputStream::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{
	if ( aError==KErrNone )
		{
		INFO_PRINTF1(_L("Copied OK"));
		if ( aBuffer.Length() > 0)
			{
			iFile.Write(aBuffer);
			TRAPD(err, ReadStreamAudioDataL());
			if ( err!=KErrNone )
				{
				ERR_PRINTF2(_L("ReadStreamAudioDataL Error %d"), err);
				SetAsyncError(iAsyncErrorIndex, err);
				DecOutstanding();
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("Buffer Copied Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		DecOutstanding();
		}
	}

/**
Utility helper function ReadStreamAudioDataL - reads audio data from a stream
@internalAll
@return void
@param None
@pre N/A
@post contents of input stream written to iBuffer
*/
void CT_MMDataMdaAudioInputStream::ReadStreamAudioDataL()
	{
	*iBuffer=KNullDesC8;
	delete iBufferPtr;
	iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());
	iInputStream->ReadL(*iBufferPtr);
	}
