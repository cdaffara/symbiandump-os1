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

#include "T_RBusDevCommData.h"
#include "T_TCommCapsV01Data.h"
#include "T_TCommConfigV01Data.h"

/*@{*/
_LIT(KStrNull,								" ");

_LIT(KCmdConstructor,						"new");
_LIT(KCmdDestructor,						"~");
_LIT(KCmdBreak,								"Break");
_LIT(KCmdBreakCancel,						"BreakCancel");
_LIT(KCmdCaps,								"Caps");
_LIT(KCmdConfig,							"Config");
_LIT(KCmdMinTurnaroundTime,					"MinTurnaroundTime");
_LIT(KCmdNotifyReceiveDataAvailable,		"NotifyReceiveDataAvailable");
_LIT(KCmdNotifyReceiveDataAvailableCancel,	"NotifyReceiveDataAvailableCancel");
_LIT(KCmdNotifySignalChange,				"NotifySignalChange");
_LIT(KCmdNotifySignalChangeCancel,			"NotifySignalChangeCancel");
_LIT(KCmdOpen,								"Open");
_LIT(KCmdQueryReceiveBuffer,				"QueryReceiveBuffer");
_LIT(KCmdRead,								"Read");
_LIT(KCmdReadCancel,						"ReadCancel");
_LIT(KCmdReadOneOrMore,						"ReadOneOrMore");
_LIT(KCmdReceiveBufferLength,				"ReceiveBufferLength");
_LIT(KCmdResetBuffers,						"ResetBuffers");
_LIT(KCmdSetConfig,							"SetConfig");
_LIT(KCmdSetMinTurnaroundTime,				"SetMinTurnaroundTime");
_LIT(KCmdSetReceiveBufferLength,			"SetReceiveBufferLength");
_LIT(KCmdSetSignals,						"SetSignals");
_LIT(KCmdSignals,							"Signals");
_LIT(KCmdVersionRequired,					"VersionRequired");
_LIT(KCmdWrite,								"Write");
_LIT(KCmdWriteCancel,						"WriteCancel");

_LIT(KFldBufferLength,						"bufferlength");
_LIT(KFldCaps,								"caps");
_LIT(KFldClearMask,							"clearmask");
_LIT(KFldConfig,							"config");
_LIT(KFldData,								"data");
_LIT(KFldExpected,							"expected");
_LIT(KFldExpectedFile,						"expected_file");
_LIT(KFldFile,								"file");
_LIT(KFldIsEmpty,							"is_empty");
_LIT(KFldMask,								"mask");
_LIT(KFldPortPDD,							"portpdd");
_LIT(KFldPortLDD,							"portldd");
_LIT(KFldRepeat,							"repeat");
_LIT(KFldSave,								"save");
_LIT(KFldSetMask,							"setmask");
_LIT(KFldTime,								"time");
_LIT(KFldTimeOut,							"timeout");
_LIT(KFldUnit,								"unit");
_LIT(KFldUseLength,							"uselength");
_LIT(KFldValue,								"value");
_LIT(KFldVersionName,						"version_name");
_LIT(KFldVersionBuild,						"version_build");
_LIT(KFldVersionMajor,						"version_major");
_LIT(KFldVersionMinor,						"version_minor");

_LIT(KLogError,								"Error=%d");
_LIT(KLogErrorBufferLength,					"Buffer Length %d must be greater than 0");
_LIT(KLogErrorExpectedValue,				"Expected Value does not match actual");
_LIT(KLogErrorExpectedSignal,				"Expected Signal %d does not match actual %d");
_LIT(KLogMissingParameter,					"Missing parameter '%S'");
_LIT(KLogNullCapsData,						"Caps Data - '%S' is NULL");
_LIT(KLogNullConfigData,					"Config Data - '%S' is NULL");
_LIT(KLogLoadPhysicalDeviceError,			"Load Physical Device '%S' error=%d");
_LIT(KLogLoadLogicalDeviceError,			"Load Logical Device '%S' error=%d");
/*@}*/

_LIT(KStrKSignalCTS,						"KSignalCTS");
_LIT(KStrKSignalDSR,						"KSignalDSR");
_LIT(KStrKSignalDCD,						"KSignalDCD");
_LIT(KStrKSignalRNG,						"KSignalRNG");
_LIT(KStrKSignalRTS,						"KSignalRTS");
_LIT(KStrKSignalDTR,						"KSignalDTR");
_LIT(KStrKSignalBreak,						"KSignalBreak");
const CDataWrapperBase::TEnumEntryTable	CT_RBusDevCommData::iEnumTableSignals[] =
	{
//	Enum as a descriptor					Enum
	KStrKSignalCTS,							KSignalCTS,
	KStrKSignalDSR,							KSignalDSR,
	KStrKSignalDCD,							KSignalDCD,
	KStrKSignalRNG,							KSignalRNG,
	KStrKSignalRTS,							KSignalRTS,
	KStrKSignalDTR,							KSignalDTR,
	KStrKSignalBreak,						KSignalBreak,
	KStrNull,								-1
	};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_RBusDevCommData* CT_RBusDevCommData::NewL()
	{
	CT_RBusDevCommData*	ret=new (ELeave) CT_RBusDevCommData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_RBusDevCommData::CT_RBusDevCommData()
:	CT_RBusLogicalChannelData()
,	iBusDevComm(NULL)
,	iBufferRead(NULL)
,	iBufferReadPtr(NULL, 0)
,	iBufferReadOneOrMorePtr(NULL, 0)
,	iReadOneOrMoreLeft(0)
,	iHasReadExpected(EFalse)
,	iBufferLength(0)
,	iMinTurnaroundTime(0)
,	iReceiveBufferLength(0)
,	iSignals(0)
,	iNotifySignals(0)
,	iHasNotifySignalsExpected(EFalse)
,	iNotifySignalsExpected(0)
,	iCallbackBreak(NULL)
,	iCallbackNotifyReceiveDataAvailable(NULL)
,	iCallbackNotifySignalChange(NULL)
,	iCallbackRead(NULL)
,	iCallbackReadOneOrMore(NULL)
,	iCallbackWrite(NULL)
	{
	}

CT_RBusDevCommData::~CT_RBusDevCommData()
/**
 * Public destructor
 */
	{
	DestroyData();

	delete iBufferRead;
	iBufferRead=NULL;

	delete iCallbackWrite;
	iCallbackWrite=NULL;

	delete iCallbackReadOneOrMore;
	iCallbackReadOneOrMore=NULL;

	delete iCallbackRead;
	iCallbackRead=NULL;

	delete iCallbackNotifySignalChange;
	iCallbackNotifySignalChange=NULL;

	delete iCallbackNotifyReceiveDataAvailable;
	iCallbackNotifyReceiveDataAvailable=NULL;

	delete iCallbackBreak;
	iCallbackBreak=NULL;
	}

void CT_RBusDevCommData::ConstructL()
	{
	iCallbackBreak						=CActiveCallbackBase::NewL(*this);
	iCallbackNotifyReceiveDataAvailable	=CActiveCallbackBase::NewL(*this);
	iCallbackNotifySignalChange			=CActiveCallbackBase::NewL(*this);
	iCallbackRead						=CT_ActiveCallbackIO::NewL(*this);
	iCallbackReadOneOrMore				=CT_ActiveCallbackIO::NewL(*this);
	iCallbackWrite						=CT_ActiveCallbackIO::NewL(*this);
	}

void CT_RBusDevCommData::DestroyData()
	{
	delete iBusDevComm;
	iBusDevComm=NULL;
	}

TAny* CT_RBusDevCommData::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iBusDevComm;
	}

RHandleBase* CT_RBusDevCommData::GetHandleBase()
	{
	return iBusDevComm;
	}

RBusLogicalChannel* CT_RBusDevCommData::GetBusLogicalChannel()
	{
	return iBusDevComm;
	}

void CT_RBusDevCommData::PrepareReadExpectedBufferLengthL(CT_ActiveCallbackIO& aCallback, const TDesC& aSection)
	{
	TInt	repeat=1;
	GetIntFromConfig(aSection, KFldRepeat, repeat);

	TPtrC	readExpected;
	iHasReadExpected=GetStringFromConfig(aSection, KFldExpected, readExpected);
	if ( iHasReadExpected )
		{
		aCallback.PrepareFromStringL(repeat, readExpected);
		iBufferLength=aCallback.BufferLength();
		}
	else
		{
		iHasReadExpected=GetStringFromConfig(aSection, KFldExpectedFile, readExpected);
		if ( iHasReadExpected )
			{
			aCallback.PrepareFromFileL(repeat, readExpected);
			}
		}

	if ( iHasReadExpected )
		{
		iBufferLength=aCallback.BufferLength();
		}

	GetIntFromConfig(aSection, KFldBufferLength, iBufferLength);
	if ( iBufferLength <= 0 )
		{
		ERR_PRINTF2(KLogErrorBufferLength, iBufferLength);
		SetBlockResult(EFail);
		}
	}

TBool CT_RBusDevCommData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
	TInt	err=KErrNone;

	if ( aCommand==KCmdConstructor )
		{
		err=DoCmdConstructor();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdBreak )
		{
		DoCmdBreak(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdBreakCancel )
		{
		DoCmdBreakCancel();
		}
	else if ( aCommand==KCmdCaps )
		{
		DoCmdCapsL(aSection);
		}
	else if ( aCommand==KCmdConfig )
		{
		DoCmdConfigL(aSection);
		}
	else if ( aCommand==KCmdMinTurnaroundTime )
		{
		DoCmdMinTurnaroundTime(aSection);
		}
	else if ( aCommand==KCmdNotifyReceiveDataAvailable )
		{
		DoCmdNotifyReceiveDataAvailable(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdNotifyReceiveDataAvailableCancel )
		{
		DoCmdNotifyReceiveDataAvailableCancel();
		}
	else if ( aCommand==KCmdNotifySignalChange )
		{
		DoCmdNotifySignalChange(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdNotifySignalChangeCancel )
		{
		DoCmdNotifySignalChangeCancel();
		}
	else if ( aCommand==KCmdOpen )
		{
		err=DoCmdOpen(aSection);
		}
	else if ( aCommand==KCmdQueryReceiveBuffer )
		{
		err=DoCmdQueryReceiveBuffer(aSection);
		}
	else if ( aCommand==KCmdRead )
		{
		DoCmdReadL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdReadCancel )
		{
		DoCmdReadCancel();
		}
	else if ( aCommand==KCmdReadOneOrMore )
		{
		DoCmdReadOneOrMoreL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdReceiveBufferLength )
		{
		DoCmdReceiveBufferLength(aSection);
		}
	else if ( aCommand==KCmdResetBuffers )
		{
		DoCmdResetBuffers();
		}
	else if ( aCommand==KCmdSetConfig )
		{
		err=DoCmdSetConfigL(aSection);
		}
	else if ( aCommand==KCmdSetMinTurnaroundTime )
		{
		err=DoCmdSetMinTurnaroundTime(aSection);
		}
	else if ( aCommand==KCmdSetReceiveBufferLength )
		{
		err=DoCmdSetReceiveBufferLength(aSection);
		}
	else if ( aCommand==KCmdSetSignals )
		{
		DoCmdSetSignals(aSection);
		}
	else if ( aCommand==KCmdSignals )
		{
		DoCmdSignals(aSection);
		}
	else if ( aCommand==KCmdVersionRequired )
		{
		DoCmdVersionRequired(aSection);
		}
	else if ( aCommand==KCmdWrite )
		{
		DoCmdWriteL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdWriteCancel )
		{
		DoCmdWriteCancel();
		}
	else
		{
		ret=CT_RBusLogicalChannelData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

TInt CT_RBusDevCommData::DoCmdConstructor()
	{
	DestroyData();
	TRAPD(err, iBusDevComm=new (ELeave) RBusDevComm());
	return err;
	}

void CT_RBusDevCommData::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_RBusDevCommData::DoCmdBreak(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TInt	timeOut=0;
	GetIntFromConfig(aSection, KFldTimeOut, timeOut);

	TInt	time;
	if ( !GetIntFromConfig(aSection, KFldTime, time) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldTime());
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Break"));
		iBusDevComm->Break(iCallbackBreak->iStatus, time);
		iCallbackBreak->Activate(aAsyncErrorIndex, timeOut);
		IncOutstanding();
		}
	}

void CT_RBusDevCommData::DoCmdBreakCancel()
	{
	INFO_PRINTF1(_L("BreakCancel"));
	iBusDevComm->BreakCancel();
	}

void CT_RBusDevCommData::DoCmdCapsL(const TDesC& aSection)
	{
	TPtrC	dataName;
	if ( GetStringFromConfig(aSection, KFldCaps, dataName) )
		{
		CT_TCommCapsV01Data*	wrapper=static_cast<CT_TCommCapsV01Data*>(GetDataWrapperL(dataName));
		TDes8*					data=wrapper->Descriptor();
		if ( data!=NULL )
			{
			INFO_PRINTF1(_L("Caps"));
			iBusDevComm->Caps(*data);
			}
		else
			{
			ERR_PRINTF2(KLogNullCapsData, &dataName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldCaps());
		SetBlockResult(EFail);
		}
	}

void CT_RBusDevCommData::DoCmdConfigL(const TDesC& aSection)
	{
	TPtrC	dataName;
	if ( GetStringFromConfig(aSection, KFldConfig, dataName) )
		{
		CT_TCommConfigV01Data*	wrapper=static_cast<CT_TCommConfigV01Data*>(GetDataWrapperL(dataName));
		TDes8*					data=wrapper->Descriptor();
		if ( data!=NULL )
			{
			INFO_PRINTF1(_L("Config"));
			iBusDevComm->Config(*data);
			}
		else
			{
			ERR_PRINTF2(KLogNullConfigData, &dataName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldConfig());
		SetBlockResult(EFail);
		}
	}

void CT_RBusDevCommData::DoCmdMinTurnaroundTime(const TDesC& aSection)
	{
	TUint	actual=iBusDevComm->MinTurnaroundTime();
	INFO_PRINTF2(_L("MinTurnaroundTime : %d"), actual);

	TUint	expected;
	if ( GetUintFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TBool	save=EFalse;
	GetBoolFromConfig(aSection, KFldSave(), save);
	if ( save )
		{
		iMinTurnaroundTime=actual;
		}
	}

void CT_RBusDevCommData::DoCmdNotifyReceiveDataAvailable(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TInt	timeOut=0;
	GetIntFromConfig(aSection, KFldTimeOut, timeOut);

	INFO_PRINTF1(_L("NotifyReceiveDataAvailable"));
	iBusDevComm->NotifyReceiveDataAvailable(iCallbackNotifyReceiveDataAvailable->iStatus);
	iCallbackNotifyReceiveDataAvailable->Activate(aAsyncErrorIndex, timeOut);
	IncOutstanding();
	}

void CT_RBusDevCommData::DoCmdNotifyReceiveDataAvailableCancel()
	{
	INFO_PRINTF1(_L("NotifyReceiveDataAvailableCancel"));
	iBusDevComm->NotifyReceiveDataAvailableCancel();
	}

void CT_RBusDevCommData::DoCmdNotifySignalChange(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TInt	timeOut=0;
	GetIntFromConfig(aSection, KFldTimeOut, timeOut);

	iHasNotifySignalsExpected=GetOrFromConfig(aSection, KFldExpected(), iEnumTableSignals, iNotifySignalsExpected);

	TUint	mask;
	if ( GetOrFromConfig(aSection, KFldMask(), iEnumTableSignals, mask) )
		{
		INFO_PRINTF1(_L("NotifySignalChange with mask"));
		iBusDevComm->NotifySignalChange(iCallbackNotifySignalChange->iStatus, iNotifySignals, mask);
		}
	else
		{
		INFO_PRINTF1(_L("NotifySignalChange without mask"));
		iBusDevComm->NotifySignalChange(iCallbackNotifySignalChange->iStatus, iNotifySignals);
		}
	iCallbackNotifySignalChange->Activate(aAsyncErrorIndex, timeOut);
	IncOutstanding();
	}

void CT_RBusDevCommData::DoCmdNotifySignalChangeCancel()
	{
	INFO_PRINTF1(_L("NotifySignalChangeCancel"));
	iBusDevComm->NotifySignalChangeCancel();
	}

TInt CT_RBusDevCommData::DoCmdOpen(const TDesC& aSection)
	{
	TBool	dataOk=ETrue;

	TPtrC	pdd;
	if ( GetStringFromConfig(aSection, KFldPortPDD, pdd) && pdd.Length()!=0 )
		{
		TInt	errLoad=User::LoadPhysicalDevice(pdd);
		if ( (errLoad!=KErrNone) && (errLoad!=KErrAlreadyExists) )
			{
			dataOk=EFalse;
			ERR_PRINTF3(KLogLoadPhysicalDeviceError, &pdd, errLoad);
			SetBlockResult(EFail);
			}
		}

	TPtrC	ldd;
	if ( GetStringFromConfig(aSection, KFldPortLDD, ldd) && ldd.Length()!=0 )
		{
		TInt	errLoad=User::LoadLogicalDevice(ldd);
		if ( (errLoad!=KErrNone) && (errLoad!=KErrAlreadyExists) )
			{
			dataOk=EFalse;
			ERR_PRINTF3(KLogLoadLogicalDeviceError, &ldd, errLoad);
			SetBlockResult(EFail);
			}
		}

	TInt	unit;
	if ( !GetIntFromConfig(aSection, KFldUnit, unit) )
		{
		dataOk=EFalse;
		ERR_PRINTF2(KLogMissingParameter, &KFldUnit());
		SetBlockResult(EFail);
		}

	TInt	err=KErrNone;
	if ( dataOk )
		{
		INFO_PRINTF1(_L("Open"));
		err=iBusDevComm->Open(unit);
		}

	return err;
	}

TInt CT_RBusDevCommData::DoCmdQueryReceiveBuffer(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("QueryReceiveBuffer"));
	TInt	actual=iBusDevComm->QueryReceiveBuffer();
	TInt	ret=KErrNone;
	if ( actual<0 )
		{
		ret=actual;
		}
	else
		{
		TInt	expected;
		if ( GetIntFromConfig(aSection, KFldExpected(), expected) )
			{
			if ( expected!=actual )
				{
				ERR_PRINTF1(KLogErrorExpectedValue);
				SetBlockResult(EFail);
				}
			}

		TBool	isEmpty;
		if ( GetBoolFromConfig(aSection, KFldIsEmpty(), isEmpty) )
			{
			if ( isEmpty )
				{
				if ( actual!=0 )
					{
					ERR_PRINTF1(KLogErrorExpectedValue);
					SetBlockResult(EFail);
					}
				}
			else
				{
				if ( actual==0 )
					{
					ERR_PRINTF1(KLogErrorExpectedValue);
					SetBlockResult(EFail);
					}
				}
			}

		TBool	save=EFalse;
		GetBoolFromConfig(aSection, KFldSave(), save);
		if ( save )
			{
			iBufferLength=actual;
			}
		}

	return ret;
	}

void CT_RBusDevCommData::DoCmdReadL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TInt	timeOut=0;
	GetIntFromConfig(aSection, KFldTimeOut, timeOut);

	TBool	useLength=EFalse;
	GetBoolFromConfig(aSection, KFldUseLength(), useLength);

	PrepareReadExpectedBufferLengthL(*iCallbackRead, aSection);
	if ( iBufferLength>0 )
		{
		delete iBufferRead;
		iBufferRead=HBufC8::New(iBufferLength);

		iBufferReadPtr.Set(iBufferRead->Des());
		if ( useLength )
			{
			INFO_PRINTF2(_L("Read with length %d"), iBufferLength);
			iBusDevComm->Read(iCallbackRead->iStatus, iBufferReadPtr, iBufferLength);
			}
		else
			{
			INFO_PRINTF1(_L("Read without length"));
			iBusDevComm->Read(iCallbackRead->iStatus, iBufferReadPtr);
			}
		iCallbackRead->Activate(aAsyncErrorIndex, timeOut);
		IncOutstanding();
		}
	}

void CT_RBusDevCommData::DoCmdReadCancel()
	{
	INFO_PRINTF1(_L("ReadCancel"));
	iBusDevComm->ReadCancel();
	}

void CT_RBusDevCommData::DoCmdReadOneOrMoreL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TInt	timeOut=0;
	GetIntFromConfig(aSection, KFldTimeOut, timeOut);

	PrepareReadExpectedBufferLengthL(*iCallbackReadOneOrMore, aSection);
	if ( iBufferLength>0 )
		{
		delete iBufferRead;
		iBufferRead=HBufC8::New(iBufferLength);

		iBufferReadPtr.Set(iBufferRead->Des());
		iBufferReadPtr.SetLength(iBufferLength);
		iBufferReadOneOrMorePtr.Set(iBufferRead->Des());
		iBufferReadOneOrMorePtr.SetLength(iBufferLength);
		iReadOneOrMoreLeft=iBufferLength;
		INFO_PRINTF2(_L("iBufferLength=%d"), iBufferLength);
		INFO_PRINTF2(_L("iBufferRead->MaxLength=%d"), iBufferRead->Des().MaxLength());
		INFO_PRINTF2(_L("iBufferReadPtr.Length=%d"), iBufferReadPtr.Length());
		INFO_PRINTF2(_L("iBufferReadPtr.MaxLength=%d"), iBufferReadPtr.MaxLength());
		INFO_PRINTF1(_L("ReadOneOrMore"));
		iBusDevComm->ReadOneOrMore(iCallbackReadOneOrMore->iStatus, iBufferReadPtr);
		iCallbackReadOneOrMore->Activate(aAsyncErrorIndex, timeOut);
		IncOutstanding();
		}
	}

void CT_RBusDevCommData::DoCmdReceiveBufferLength(const TDesC& aSection)
	{
	TInt	actual=iBusDevComm->ReceiveBufferLength();
	INFO_PRINTF2(_L("ReceiveBufferLength : %d"), actual);

	TInt	expected;
	if ( GetIntFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TBool	save=EFalse;
	GetBoolFromConfig(aSection, KFldSave(), save);
	if ( save )
		{
		iReceiveBufferLength=actual;
		}
	}

void CT_RBusDevCommData::DoCmdResetBuffers()
	{
	INFO_PRINTF1(_L("ResetBuffers"));
	iBusDevComm->ResetBuffers();
	}

TInt CT_RBusDevCommData::DoCmdSetConfigL(const TDesC& aSection)
	{
	TInt	ret=KErrNone;

	TPtrC	dataName;
	if ( GetStringFromConfig(aSection, KFldConfig, dataName) )
		{
		CT_TCommConfigV01Data*	wrapper=static_cast<CT_TCommConfigV01Data*>(GetDataWrapperL(dataName));
		TDes8*					data=wrapper->Descriptor();
		if ( data!=NULL )
			{
			INFO_PRINTF1(_L("SetConfig"));
			ret=iBusDevComm->SetConfig(*data);
			}
		else
			{
			ERR_PRINTF2(KLogNullConfigData, &dataName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldConfig());
		SetBlockResult(EFail);
		}

	return ret;
	}

TInt CT_RBusDevCommData::DoCmdSetMinTurnaroundTime(const TDesC& aSection)
	{
	TUint	value=iMinTurnaroundTime;
	GetUintFromConfig(aSection, KFldValue(), value);

	INFO_PRINTF1(_L("SetMinTurnaroundTime"));
	return iBusDevComm->SetMinTurnaroundTime(value);
	}

TInt CT_RBusDevCommData::DoCmdSetReceiveBufferLength(const TDesC& aSection)
	{
	TInt	value=iReceiveBufferLength;
	GetIntFromConfig(aSection, KFldValue(), value);

	INFO_PRINTF1(_L("SetReceiveBufferLength"));
	return iBusDevComm->SetReceiveBufferLength(value);
	}

void CT_RBusDevCommData::DoCmdSetSignals(const TDesC& aSection)
	{
	TUint	setMask=iSignals;
	GetOrFromConfig(aSection, KFldSetMask(), iEnumTableSignals, setMask);

	TUint	clearMask=~setMask;
	GetOrFromConfig(aSection, KFldClearMask(), iEnumTableSignals, clearMask);

	INFO_PRINTF1(_L("SetSignals"));
	iBusDevComm->SetSignals(setMask, clearMask);
	}

void CT_RBusDevCommData::DoCmdSignals(const TDesC& aSection)
	{
	TUint	actual=iBusDevComm->Signals();
	INFO_PRINTF2(_L("Signals:0x%x"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableSignals, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TBool	save=EFalse;
	GetBoolFromConfig(aSection, KFldSave(), save);
	if ( save )
		{
		iSignals=actual;
		}
	}

void CT_RBusDevCommData::DoCmdVersionRequired(const TDesC& aSection)
	{
	TVersion				version=iBusDevComm->VersionRequired();
	TBuf<KMaxVersionName>	versionName(version.Name());

	INFO_PRINTF2(_L("Version name  : %S"), &versionName);
	INFO_PRINTF2(_L("Version build : %d"), (TInt)version.iBuild);
	INFO_PRINTF2(_L("Version major : %d"), (TInt)version.iMajor);
	INFO_PRINTF2(_L("Version minor : %d"), (TInt)version.iMinor);

	TPtrC	expectedVersionName;
	if( GetStringFromConfig(aSection, KFldVersionName(), expectedVersionName) )
		{
		if( version.Name() != expectedVersionName )
			{
			ERR_PRINTF3(_L("Expected Version Name (%S) != Actual Version Name (%S)"), &expectedVersionName, &versionName);
			SetBlockResult(EFail);	
			}
		}

	TInt	intTemp;
	if( GetIntFromConfig(aSection, KFldVersionBuild(), intTemp) )
		{
		if( version.iBuild != intTemp )
			{
			ERR_PRINTF3(_L("Expected Version Build (%d) != Actual Version Build (%d)"), intTemp, version.iBuild);
			SetBlockResult(EFail);
			}
		}

	if( GetIntFromConfig(aSection, KFldVersionMajor(), intTemp) )
		{
		if( version.iMajor != intTemp )
			{
			ERR_PRINTF3(_L("Expected Version Major (%d) != Actual Version Major (%d)"), intTemp, version.iMajor);
			SetBlockResult(EFail);
			}
		}

	if( GetIntFromConfig(aSection, KFldVersionMinor(), intTemp) )
		{
		if( version.iMinor != intTemp )
			{
			ERR_PRINTF3(_L("Expected Version Minor (%d) != Actual Version Minor (%d)"), intTemp, version.iMinor);
			SetBlockResult(EFail);
			}
		}

	if (version.Name() == _L("") && version.iBuild == 0 && version.iMajor == 0 && version.iMinor == 0)
		{
		ERR_PRINTF1(_L("Some version fields are not set!"));
		SetBlockResult(EFail);
		}
	}

void CT_RBusDevCommData::DoCmdWriteL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	dataOk=ETrue;
	TBool	useLength=EFalse;
	GetBoolFromConfig(aSection, KFldUseLength(), useLength);

	TInt	timeOut=0;
	GetIntFromConfig(aSection, KFldTimeOut, timeOut);

	TInt	repeat=1;
	GetIntFromConfig(aSection, KFldRepeat, repeat);

	TPtrC	data;
	if ( GetStringFromConfig(aSection, KFldData, data) )
		{
		iCallbackWrite->PrepareFromStringL(repeat, data);
		}
	else
		{
		if ( GetStringFromConfig(aSection, KFldFile, data) )
			{
			iCallbackWrite->PrepareFromFileL(repeat, data);
			}
		else
			{
			dataOk=EFalse;
			ERR_PRINTF2(KLogMissingParameter, &KFldData);
			ERR_PRINTF2(KLogMissingParameter, &KFldFile);
			SetBlockResult(EFail);
			}
		}

	if ( dataOk )
		{
		iBufferLength=iCallbackWrite->BufferLength();

		if ( useLength )
			{
			INFO_PRINTF2(_L("Write with length %d"), iBufferLength);
			iBusDevComm->Write(iCallbackWrite->iStatus, iCallbackWrite->Buffer(), iBufferLength);
			}
		else
			{
			INFO_PRINTF1(_L("Write without length"));
			iBusDevComm->Write(iCallbackWrite->iStatus, iCallbackWrite->Buffer());
			}
		iCallbackWrite->Activate(aAsyncErrorIndex, timeOut);
		IncOutstanding();
		}
	}

void CT_RBusDevCommData::DoCmdWriteCancel()
	{
	INFO_PRINTF1(_L("WriteCancel"));
	iBusDevComm->WriteCancel();
	}

void CT_RBusDevCommData::RunL(CActive* aActive, TInt aIndex)
/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MTPActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
	{
	TBool	activeOk=ETrue;
	TInt	err=KErrNone;

	if ( aActive!=NULL )
		{
		err=aActive->iStatus.Int();
		}

	if ( aActive==iCallbackRead )
		{
		iCallbackRead->KillTimer();
		INFO_PRINTF1(_L("RunL: Read()"));

		if ( err==KErrNone )
			{
			TInt	length=iBufferReadPtr.Length();
			HBufC*	bufferRead=HBufC::NewLC(length);
			bufferRead->Des().Copy(iBufferReadPtr);
			INFO_PRINTF2(_L("Received Data Length:%d"), length);
			if ( iHasReadExpected )
				{
				const TDesC8&	expectedBuffer=iCallbackRead->Buffer();
				if ( expectedBuffer.Match(iBufferReadPtr)!=0 )
					{
					for ( TInt index=0; index<length; ++index )
						{
						const TUint8&	actual=(*iBufferRead)[index];
						const TUint8&	expected=expectedBuffer[index];
						if ( actual!=expected )
							{
							ERR_PRINTF4(_L("Index:%d Read:%d Expected:%d"), index, (TInt)actual, (TInt)expected);
							SetBlockResult(EFail);
							}
						}
					}
				}
			CleanupStack::PopAndDestroy(bufferRead);
			}
		}
	else if ( aActive==iCallbackReadOneOrMore )
		{
		INFO_PRINTF1(_L("RunL: ReadOneOrMore()"));

		if ( err==KErrNone )
			{
			TInt	length=iBufferReadPtr.Length();
			if ( iReadOneOrMoreLeft!=length )
				{
				iBufferReadPtr.SetLength(iReadOneOrMoreLeft);
				iReadOneOrMoreLeft-=length;
				INFO_PRINTF2(_L("iReadOneOrMoreLeft:%d"), iReadOneOrMoreLeft);
				iBufferReadPtr.Set(iBufferReadPtr.RightTPtr(iReadOneOrMoreLeft));
				iBufferReadPtr.SetLength(iReadOneOrMoreLeft);
				INFO_PRINTF1(_L("ReadOneOrMore"));
				iBusDevComm->ReadOneOrMore(iCallbackReadOneOrMore->iStatus, iBufferReadPtr);
				iCallbackReadOneOrMore->Activate(aIndex);
				IncOutstanding();
				}
			else
				{
				iCallbackReadOneOrMore->KillTimer();
				HBufC*	bufferRead=HBufC::NewLC(iBufferLength);
				bufferRead->Des().Copy(iBufferReadOneOrMorePtr);
				if ( iHasReadExpected )
					{
					const TDesC8&	expectedBuffer=iCallbackReadOneOrMore->Buffer();
					if ( expectedBuffer.Match(iBufferReadOneOrMorePtr)!=0 )
						{
						for ( TInt index=0; index<iBufferLength; ++index )
							{
							const TUint8&	actual=(*iBufferRead)[index];
							const TUint8&	expected=expectedBuffer[index];
							if ( actual!=expected )
								{
								ERR_PRINTF4(_L("Index:%d Read:%d Expected:%d"), index, (TInt)actual, (TInt)expected);
								}
							}

						ERR_PRINTF1(KLogErrorExpectedValue);
						SetBlockResult(EFail);
						}
					}
				CleanupStack::PopAndDestroy(bufferRead);
				}
			}
		}
	else if ( aActive==iCallbackBreak )
		{
		iCallbackBreak->KillTimer();
		INFO_PRINTF1(_L("RunL: Break()"));
		}
	else if ( aActive==iCallbackNotifySignalChange )
		{
		iCallbackNotifySignalChange->KillTimer();
		INFO_PRINTF1(_L("RunL: NotifySignalChange()"));

		if ( err==KErrNone )
			{
			INFO_PRINTF2(_L("Signals Changed=0x%x"), iNotifySignals);
			if ( iHasNotifySignalsExpected )
				{
				if ( iNotifySignals!=iNotifySignalsExpected )
					{
					ERR_PRINTF3(KLogErrorExpectedSignal, iNotifySignals, iNotifySignalsExpected);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else if ( aActive==iCallbackWrite )
		{
		iCallbackWrite->KillTimer();
		INFO_PRINTF1(_L("RunL: Write()"));
		}
	else if ( aActive==iCallbackNotifyReceiveDataAvailable )
		{
		iCallbackNotifyReceiveDataAvailable->KillTimer();
		INFO_PRINTF1(_L("RunL: NotifyReceiveDataAvailable()"));
		}
	else
		{
		CT_RBusLogicalChannelData::RunL(aActive, aIndex);
		activeOk=EFalse;
		}

	if ( activeOk )
		{
		DecOutstanding();
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(aIndex, err);
		}
	}

void CT_RBusDevCommData::DoCancel(CActive* aActive, TInt aIndex)
/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @internalComponent
 @see - MTPActiveCallback
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
*/
	{
	TBool	activeOk=ETrue;
	SetBlockResult(EFail);

	if ( aActive==iCallbackRead )
		{
		ERR_PRINTF1(_L("ReadCancel Called"));
		iBusDevComm->ReadCancel();
		}
	else if ( aActive==iCallbackReadOneOrMore )
		{
		ERR_PRINTF1(_L("ReadCancel Called"));
		iBusDevComm->ReadCancel();
		}
	else if ( aActive==iCallbackBreak )
		{
		ERR_PRINTF1(_L("BreakCancel Called"));
		iBusDevComm->BreakCancel();
		}
	else if ( aActive==iCallbackNotifySignalChange )
		{
		ERR_PRINTF1(_L("NotifySignalChangeCancel Called"));
		iBusDevComm->NotifySignalChangeCancel();
		}
	else if ( aActive==iCallbackWrite )
		{
		ERR_PRINTF1(_L("WriteCancel Called"));
		iBusDevComm->WriteCancel();
		}
	else if ( aActive==iCallbackNotifyReceiveDataAvailable )
		{
		ERR_PRINTF1(_L("NotifyReceiveDataAvailableCancel Called"));
		iBusDevComm->NotifyReceiveDataAvailableCancel();
		}
	else
		{
		activeOk=EFalse;
		CT_RBusLogicalChannelData::DoCancel(aActive, aIndex);
		}

	if ( activeOk )
		{
		DecOutstanding();
		}
	}
