// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Serial IO Functions
// 
//

#include "ATIO.H"
#include "mSLOGGER.H"
#include "ATERROR.H"

//
// CCompletionEntry
//
CCompletionEntry* CCompletionEntry::NewL(CCommChatString* aCs,	CATBase* aAtCommand)
	{
	return new(ELeave) CCompletionEntry(aCs,aAtCommand);
	}

CCompletionEntry::CCompletionEntry(CCommChatString* aCs, CATBase* aAtCommand) : iCs(aCs), iAtCommand(aAtCommand)
	{}

CCompletionEntry::~CCompletionEntry()
	{}

//
// CATIo
//
CATIO* CATIO::NewL(TFileName& aCsy, TName& aPort,TPortAccess& aPortAccess)
	{
	CATIO* atIo=new(ELeave) CATIO(aPortAccess);
	CleanupStack::PushL(atIo);
	atIo->ConstructL(aCsy,aPort);
	CleanupStack::Pop();
	LOGTEXT2(_L8("CATIO::NewL returns 0x%x"),atIo);
	return atIo;
	}

CATIO::CATIO(TPortAccess& aPortAccess)
	: iPortAccess(aPortAccess)
	{
	iExpectList.SetOffset(_FOFF(CCompletionEntry,iLink));
	}

void CATIO::ConstructL(TFileName& aCsy, TName& aPort)
	{
	iCsy=aCsy;
	CommConstructL(KCommReadPriority,KCommWritePriority);
	iChat = new (ELeave) CCommChatter(this, KChatterPriority);
	iChat->CreateL(KChatBufferSize);
	
	{		// Braces scope the lifetime of tmpCsy and tmpPort
	TBuf8<KMaxFileName> tmpCsy;
	TBuf8<KMaxName> tmpPort;
	tmpCsy.Copy(aCsy);		// Convert parameter to 8 bit string
	tmpPort.Copy(aPort);	// Convert parameter to 8 bit string
	LOGTEXTREL3(_L8("CATIO::ConstructL Attempting open csy:%S port:%S"),&tmpCsy,&tmpPort);
	}
	TInt ret = CommOpen(aCsy, aPort, ECommShared);
	if(ret == KErrAccessDenied || ret == KErrInUse)
		iPortAccess=EPortAccessDenied;
	else
		(void)User::LeaveIfError(ret);
	LOGTEXTREL(_L8("CATIO::ConstructL Open successful"));

	iChatStringFound= new(ELeave) CArrayFixFlat<CCommChatString*>(5);
	}

CATIO::~CATIO()
	{
	delete iChatStringFound;
	delete iChat;
	}

TInt CATIO::ConfigurePort(TCommConfig aConfiguration)
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		TInt ret;
		TCommConfig cbuf;
		TCommConfigV01 &cfg=cbuf();
		iCommPort.Config(cbuf);							// Get the Configuration
	
		TCommConfigV01 &newCfg=aConfiguration();
		cfg.iRate=newCfg.iRate;
		cfg.iDataBits=newCfg.iDataBits;
		cfg.iStopBits=newCfg.iStopBits;
		cfg.iParity=newCfg.iParity;
		cfg.iHandshake=newCfg.iHandshake;
		ret = iCommPort.SetConfig(cbuf);				// Set the Configuration
		if(ret!=KErrNone)
			{
			LOGTEXT2(_L8("CATIO:\tError %d configuring port"),ret);
			return ret;
			}
		}
	return KErrNone;
	}

TInt CATIO::GetPortShutdownTimeout()
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		TCommConfig2 cbuf;
		TCommConfigV02& cfg=cbuf();
		iCommPort.Config(cbuf);				// Get the Configuration

		return (cfg.iTxShutdownTimeout);
		}
	else
		return (0);
	}

TInt CATIO::SetPortShutdownTimeout(TInt aTimeout)
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		TCommConfig2 cbuf;
		TCommConfigV02 &cfg=cbuf();
		iCommPort.Config(cbuf);				// Get the Configuration
	
		cfg.iTxShutdownTimeout = aTimeout;
		return (iCommPort.SetConfig(cbuf));	// Set the Configuration
		}
	else
		return KErrAccessDenied;
	}

void CATIO::Start(CATBase* aCompletionClass)
	{
	CommReadReady();
	StartWrite(aCompletionClass);
	}

void CATIO::StartWrite(CATBase* aCompletionClass)
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		iWriteCommand=aCompletionClass;
		CommWriteReady();
		iWritePending=ETrue;
		}
	}

TPtrC8 CATIO::Buffer() const
	{
	return iChat->Buffer();
	}

TInt CATIO::BufferFindF(const TDesC8& aDes) const
	{
	return iChat->Buffer().FindF(aDes);
	}

TPtrC8 CATIO::CurrentLine() const
	{
	return iChat->CurrentLine();
	}

void CATIO::ClearBuffer()
	{
	if(!iPreserveBuffer)
		iChat->ClearBuffer();
	}

void CATIO::ClearCurrentLine()
	{
	iChat->ClearCurrentLine();
	}

void CATIO::CommReadComplete(TInt aStatus)
	{
	LOGTEXT(_L8("Read Completion"));
	__ASSERT_ALWAYS(iReadPending,Panic(EATCommand_IllegalCompletionReadNotExpected));
	if (aStatus==KErrCommsLineFail)
		{
		if (iSecondChanceForCommsError++!=1)	
			aStatus=KErrNone;		// only signal error if get 2 KErrCommsLineFail's in succession
		else
			iSecondChanceForCommsError=0;
		}
	if (aStatus!=KErrNone)
		{
		SignalCommandsWithError(aStatus);
		return;
		}
	iReadPending = EFalse;
	TRAPD(ret,ProcessReadCharsL());
	if(ret!=KErrNone)
		{
		Cancel();	// This error cannot be related to a command - so they'll all be cleaned up.
		iChat->DeleteAllAndStop();
		}
	}

void CATIO::SignalCommandsWithError(TInt aStatus)
//
//	Complete all current AT commands with the error and call the error handler
//
	{	
	LOGTEXT2(_L8("Received an error of %d"),aStatus);
	Cancel();
	CCompletionEntry* ce;
	TDblQueIter<CCompletionEntry> iter(iExpectList);
	while (ce = iter, ce!=NULL)
		{
		ce->iAtCommand->GenericEventSignal(EReadCompletion,aStatus);
		iter.SetToLast();
		CCompletionEntry* removeSimilar;
		while (removeSimilar=iter--, removeSimilar!=ce)
			{
			if(removeSimilar && removeSimilar->iAtCommand==ce->iAtCommand)
				{
				iChat->RemoveString(removeSimilar->iCs);
				delete removeSimilar->iCs;
				removeSimilar->iLink.Deque();
				delete removeSimilar;
				}
			}
		iChat->RemoveString(ce->iCs);
		delete ce->iCs;
		ce->iLink.Deque();
		delete ce;
		iter.SetToFirst();
		}
	// Reset TSY status, complete notifications and re-add some expect strings
	iErrorHandler->CleanUp(aStatus);
	}

void CATIO::ProcessReadCharsL()
	{
	LOGTEXT2(_L8("Received %d"),iRxBuf.Length());
	LOGTEXTREL2(_L8("Rx:\t%S"),&iRxBuf);

	TBool hitFlag=EFalse;
	TInt len;

	for (iRxBufOffset=0; iRxBufOffset<iRxBuf.Length(); iRxBufOffset++)
		{
		iChat->AddCharL(iRxBuf[iRxBufOffset]);
// Check for hits and one up-call per hit NOW
		if((len=iChatStringFound->Count())>0)
			{
			for(TInt i=0;i<len;i++)
				{
				hitFlag=ETrue;
// Find the AT Command to complete
				CCompletionEntry* ce;
				TDblQueIter<CCompletionEntry> iter(iExpectList);
				TBool aFoundFlag=EFalse;
				while (ce = iter++, ce!=NULL)
					{
					if(ce->iCs==iChatStringFound->At(i))
						{
						iCurrentFoundChatString=ce->iCs;
						ce->iAtCommand->GenericEventSignal(EReadCompletion,KErrNone);
						aFoundFlag=ETrue;
						break;
						}
					}
				if(!aFoundFlag)
					{
					LOGTEXT(_L8("Internal Error - Chat String signalled, but not found"));
					User::Leave(KErrGeneral);
					}
				}
			iChatStringFound->Delete(0,iChatStringFound->Count());
			}
		}
	if(hitFlag)
		{
		//iReadPending=EFalse;
		Read();							// Queue Another...
		}
	else
		{
		iReadPending = ETrue;
		CommReadOneOrMore(iRxBuf);
		}
	}

void CATIO::SetPreserveBufferFlag(TBool aFlg)
//
// Set Preserve Buffer Flag
//
	{
	iPreserveBuffer=aFlg;
	}

CCommChatString* CATIO::FoundChatString()
	{
	return iCurrentFoundChatString;
	}

CCommChatString* CATIO::AddExpectString(CATBase* aATBase, const TDesC8& aString, TBool aPartLine)
	{
	CCommChatString* cs=NULL;

	TRAPD(err,cs=DoAddExpectStringL(aATBase,aString,aPartLine));
	if(err==KErrNone)
		LOGTEXT2(_L8("CATIO::AddExpectString added %S"),&aString);
	else
		{
		cs=NULL;
		LOGTEXT2(_L8("CATIO::AddExpectString FAILED TO ADD %S"),&aString);
		}
	return cs;
	}

CCommChatString* CATIO::DoAddExpectStringL(CATBase* aATBase, const TDesC8& aString, TBool aPartLine)
	{
	CCommChatString* cs=iChat->AddStringL(aString, aPartLine);		
	CCompletionEntry* completionEntry=CCompletionEntry::NewL(cs,aATBase);
	iExpectList.AddLast(*completionEntry);
	return cs;
	}


void CATIO::RemoveExpectString(CCommChatString* aExpectString)
	{
// Find the AT Command to complete
	CCompletionEntry* ce;
	TDblQueIter<CCompletionEntry> iter(iExpectList);
	while (ce = iter++, ce!=NULL)
		{
		if(ce->iCs==aExpectString)
			{
			iChat->RemoveString(ce->iCs);
			delete ce->iCs;
			ce->iLink.Deque();
			delete ce;
			break;
			}
		}
	}

void CATIO::RemoveExpectStrings(CATBase* aATBase)
	{
// Find the AT Command to complete
	CCompletionEntry* ce;
	TDblQueIter<CCompletionEntry> iter(iExpectList);
	while (ce = iter++, ce!=NULL)
		{
		if(ce->iAtCommand==aATBase)
			{
			iChat->RemoveString(ce->iCs);
			delete ce->iCs;
			ce->iLink.Deque();
			delete ce;
			}
		}
	}

void CATIO::CommWriteComplete(TInt aStatus)
	{
	LOGTEXT(_L8("Comm Write Completion"));
	iChat->StopTimer();
	__ASSERT_ALWAYS(iWritePending,Panic(EATCommand_IllegalCompletionWriteNotExpected));
//	if(!iWritePending)
//		aStatus=KErrIllegalWriteComplete;						// No Write Queued!
	iWritePending=EFalse;
	iWriteCommand->GenericEventSignal(EWriteCompletion,aStatus);
	}

void CATIO::ChatStringMatchL(CCommChatString* aCs)
	{
	iStringFound=ETrue;
	iChatStringFound->AppendL(aCs);
	}

void CATIO::ChatTimeout()
	{
	LOGTEXT(_L8("Chat Time-out Completion"));
	if(iWaitTimerPending)
		{
		iWaitTimerPending=EFalse;
		iTimeOutCommand->GenericEventSignal(ETimeOutCompletion,KErrNone);
		}
	else
		Panic(EATCommand_IllegalWaitCompletion);
	}

void CATIO::SetTimeOut(CATBase* aCompletionClass, TUint aTimePeriodMillisec)
	{
	iTimeOutCommand=aCompletionClass;
	iChat->StartTimer(aTimePeriodMillisec*1000);
	iWaitTimerPending=ETrue;
	}

void CATIO::Read()
 	{
	if (iPortAccess==EPortAccessAllowed && iReadPending==FALSE)
		{
 		iReadPending=ETrue;
		iStringFound=EFalse;
		CommReadOneOrMore(iRxBuf);
		iRxBufOffset = 0;
		LOGTEXT(_L8("Queued a Read"));
		}
	}

void CATIO::Write(CATBase* aCompletionClass, const TDesC8& aString)
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		ClearBuffer();	// assume previous responses fully processed
		iWriteCommand=aCompletionClass;
		iWritePending=ETrue;
		CommWrite(aString);
		if (aString.Length() > 350)
			{	
			LOGTEXT2(_L8("CATIO:\taString too big! Length:%d"), aString.Length());
			}	
		else
			{
			LOGTEXTREL2(_L8("Tx:\t%S"),&aString);
			}
		LOGTEXT(_L8("Queued a Transmission"));
		}
	}

TBool CATIO::ReadPending()
	{
	return iReadPending;
	}

void CATIO::Disconnect()
	{
	if (iPortAccess == EPortAccessAllowed)
		{
		TCommConfig cbuf;
		TCommConfigV01 &cfg=cbuf();
		iCommPort.Config(cbuf);
		cfg.iHandshake = KConfigFreeRTS	| KConfigFreeDTR;
		TInt ret=iCommPort.SetConfig(cbuf);
		if (ret == KErrNone)
			iCommPort.SetSignalsToSpace(KSignalRTS | KSignalDTR);
		}

	CommClose();

	iPortAccess = EPortAccessDenied;
	}

void CATIO::Cancel()
	{
	LOGTEXT(_L8("CATIO:\tCancel called"));
	CommCancel();
	iReadPending = EFalse;
	iWritePending = EFalse;
	iChat->StopTimer();
	}

void CATIO::ReadCancel()
	{
	LOGTEXT(_L8("CATIO:\tReadCancel called"));
	CommReadCancel();
	iReadPending = EFalse;
	}

void CATIO::WriteAndTimerCancel(CATBase* aATBase)
	{
	if (aATBase==iWriteCommand)
		{
		CommWriteCancel();
		}
	if (aATBase==iTimeOutCommand)
		{
		iChat->StopTimer();
		}
	}


void CATIO::DropDtr()
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		LOGTEXT(_L8("CATIO::DropDtr Dropping DTR"));
		iCommPort.SetSignals(0,KSignalDTR);
		}
	}


void CATIO::RaiseDTR()
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		LOGTEXT(_L8("CATIO::RaiseDTR Raising DTR"));
		iCommPort.SetSignals(KSignalDTR,0);
		}
	}

void CATIO::ResetReadAndWriteBuffers()
	{
	iCommPort.ResetBuffers();
	}

TInt CATIO::GetSizeOfRxBuffer()
	{
	return iCommPort.QueryReceiveBuffer();
	}

TUint CATIO::Signals()
	{
	return iCommPort.Signals();
	}

void CATIO::DropRTS()
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		LOGTEXT(_L8("Dropping RTS"));
		iCommPort.SetSignals(0,KSignalRTS);
		}
	}

void CATIO::RaiseRTS()
	{
	if (iPortAccess==EPortAccessAllowed)
		{
		LOGTEXT(_L8("Raising RTS"));
		iCommPort.SetSignals(KSignalRTS,0);
		}
	}
