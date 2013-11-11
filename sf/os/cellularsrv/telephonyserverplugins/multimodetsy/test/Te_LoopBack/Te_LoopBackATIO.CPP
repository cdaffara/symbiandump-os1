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
// This file defines classes to handle serial port input/output and implement 
// the "expect string" handling.  These "expect strings" are strings 
// that are expected to be received on the serial line with the 
// relevant callbacks.  This is a portion of code that has been reused
// from the GSM TSY.  In this component, it is a bit like taking a 
// sledgehammer to crack a nut, but it does do the job. 
// 
//

/**
 @file
 @note  There are mulitple classes implemented in this file.
 @note  These classes are CCompletionEntry and CATIO
*/

#include "Te_LoopBackATIO.H"
#include "Te_LoopBackATBASE.H"
#include "Te_LoopBackSLOGGER.H"

CCompletionEntry* CCompletionEntry::NewL(CCommChatString* aCs,	CATBase* aAtCommand)
/**
 * This method creates an instance of CCompletionEntry.
 *
 * @param aCS: pointer to expected string response from the GSM TSY.
 * @param aAtCommand: pointer to the script engine processing the line of script.
 * @leave Leaves if out-of-memory.
 * @return pointer to the instance of "CCompletionEntry".
 */
	{
	return new(ELeave) CCompletionEntry(aCs,aAtCommand);
	}

CCompletionEntry::CCompletionEntry(CCommChatString* aCs, CATBase* aAtCommand) : iCs(aCs), iAtCommand(aAtCommand)
/**
 * This method is the constructor for CCompletionEntry.
 *
 * @param aCS: pointer to expected string response from the GSM TSY.
 * @param aAtCommand: pointer to the script engine processing the line of script.
 * @note Initializes private data "iCs" and "iAtCommand" to received parameters.
 */
	{}

CCompletionEntry::~CCompletionEntry()
/**
 * This method is the Destructor for CCompletionEntry.
 */
	{}

//
// CATIo
//
CATIO* CATIO::NewL(const TDesC& aCsy, const TDesC& aPort)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CATIO.
 *
 * @param aCsy: reference to a DLL name of the CSY.
 * @param aPort: reference to the port name.
 * @leave Leaves if out-of-memory.
 * @return pointer to the instance of "CATIO".
 */
	{
	CATIO* atIo=new(ELeave) CATIO();
	CleanupStack::PushL(atIo);
	atIo->ConstructL(aCsy,aPort);
	CleanupStack::Pop();
	return atIo;
	}

CATIO::CATIO()
/**
 * This method is the constructor for CATIO.  Private data is initialized and 
 * a new value for the byte offset into the class for member data iLink is retrieved.
 */
	{
	__DECLARE_NAME(_S("CATIO"));
	iExpectList.SetOffset(_FOFF(CCompletionEntry,iLink));
	iReadPending=EFalse;
	iWritePending=EFalse;
	iWaitTimerPending=EFalse;
	}

void CATIO::ConstructL(const TDesC& aCsy, const TDesC& aPort)
/**
 * This method is used to implement the 2 Phase Constructor for CATIO.
 * This method sets up new iCommReader and iCommWriter data for CATIO.
 * It also sets up a new iChat for and creates the buffer for iChat.
 * The Comm port indicated by the DLL and port name is then opened with 
 * as a shared resource.
 *
 * @param   aCsy:  reference to a DLL name of the CSY.
 * @param   aPort: reference to the port name.
 * @leave  Leaves if comm port can not be opened or if out-of-memory when creating other objects.
 * @return	pointer to the instance of "CATIO".
 */
	{
	CommConstructL(KCommReadPriority,KCommWritePriority);
	iChat = new (ELeave) CCommChatter(this, KChatterPriority);
	iChat->CreateL(KChatBufferSize);
 	User::LeaveIfError(CommOpen(aCsy, aPort, ECommShared));
	iChatStringFound= new(ELeave) CArrayFixFlat<CCommChatString*>(5);
	}

CATIO::~CATIO()
/**
 * This method is used to delete the instantion of CATIO.  This method 
 * initiates the removal of the intantiations of iChatStringFound and iChat 
 * that were created for this CATIO. 
 *
 * @param   None.
 * @return	None.
 * @note	None.
 */
	{
	delete iChatStringFound;
	delete iChat;
	}

TInt CATIO::ConfigurePort(TCommConfig aConfiguration)
/**
 * This method is used to update the comm port configuration with data received 
 * from the input parameter.
 *
 * @param   aConfiguration: reference to port configuration data.
 * @return	error value for attempt to configure port.
 */
	{
	TInt ret;
	TCommConfig cbuf;
	TCommConfigV01 &cfg=cbuf();
	iCommPort.Config(cbuf);				// Get the Configuration

	// overwrite port configuration with input configuration
	TCommConfigV01 &newCfg=aConfiguration();
	cfg.iRate=newCfg.iRate;
	cfg.iDataBits=newCfg.iDataBits;
	cfg.iStopBits=newCfg.iStopBits;
	cfg.iParity=newCfg.iParity;
	cfg.iHandshake=newCfg.iHandshake;
	ret = iCommPort.SetConfig(cbuf);		// Set the Configuration
	if(ret!=KErrNone)
		{
		LOGTEXT2(_L8("CATIO:\tError %d configuring port"),ret);
		return ret;
		}
	return KErrNone;
	}

TInt CATIO::Start(CATBase* aCompletionClass)
/**
 * This method is used to make sure the comm port is ready and a read is outstanding on 
 * the comm port.  This is done by making sure the comm port is initialized for both read 
 * and write capabilities.  After the comm port is initialized for read and write, a read
 * is posted to the comm port to have a read outstanding for CATIO.
 *
 * @param   aCompletionClass: pointer to a CATBase class indicating port and data.
 * @return	error value for attempt to read port.
 */
	{
	iControllingClass=aCompletionClass;
	CommReadReady();
	LOGTEXT(_S8("CATIO::Start() - ReadReady Completed"));
	CommWriteReady();
	LOGTEXT(_S8("CATIO::Start() - WriteReady Completed"));
	return Read();
	}

void CATIO::GetExcessData(TDes8& aBuffer)
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	TPtrC8 des(iRxBuf.Ptr()+iRxBufOffset, iRxBuf.Length()-iRxBufOffset);
	aBuffer.Copy(des);
	}

void CATIO::MarkRxBuffer(TInputBufferMark& aBufferMarker)
/**
 * Set markers for last character received from the GSM TSY.
 *
 * @param   aBufferMark: reference to markers in buffer for last character received from comm port.
 * @return	None.
 */
	{
	iChat->GetChatBufferMarker(aBufferMarker);
	}

TPtrC8 CATIO::GetRxBufferLC(TInputBufferMark& aBufferMarker)
/**
 * This function is currently not used by the Etel regression test harness.
 *
 * @note: This function is designed to return a descriptor for the buffer between the
 *        marker passed as a parameter and the current append marker.
 */
	{
	return iChat->GetChatBufferLC(aBufferMarker);
	}

void CATIO::CommReadComplete(TInt aStatus)
/**
 * This method is invoked by the RunL of the active object to complete a read.
 * When read complete is for KErrCommsLineFail, then a check will be made to make sure 
 * there have been 2 KErrCommsLineFail's in succession before signaling the line error.
 * When no errors or 1st KErrCommsLineFail, call ProcessReadCharsL to process the 
 * characters read from the comm port.  If an error occurs processing the comm port
 * read buffer characters, the ProcessReadCharsL method will leave and will be trapped
 * in this function and the call is canceled.
 *
 * @param   aStatus: status from server indicating why read completed.
 * @return	None.
 */
	{
	LOGTEXT(_S8("CATIO Read Completion"));
	__ASSERT_ALWAYS(iReadPending,HayesPanic(EATCommand_IllegalCompletionReadNotExpected));
//	if(!iReadPending)
  //
	//	{
	//	SignalCommandsWithError(KErrIllegalReadComplete);
	//	return;
	//	}
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
/**
 * This method is invoked when an error is detected.  All current strings with the
 * error are removed and iControllingClass->GenericEventSignal method is called to
 * signal EReadCompletion.
 *
 * @param   aStatus: status indicating error of the read.
 * @return	None.
 */
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
			if(removeSimilar->iAtCommand==ce->iAtCommand)
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
	if(iControllingClass!=NULL)
		iControllingClass->GenericEventSignal(EReadCompletion,aStatus);
	}

void CATIO::ProcessReadCharsL()
/**
 * This method processes the characters read from the comm port. It uses 
 * these characters to lookup any string from the script to complete.
 * If the string to complete is not found, this method will leave.
 * If the string to complete is found, a read complete is signaled and 
 * another read is queued.  
 * 
 * @leave Leaves if string to complete is not found in the expected list.
 */
	{
	LOGTEXT3(_L8("Rx:\t(%d) %S"),iRxBuf.Length(),&iRxBuf);
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
// Find the string to complete
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
					LOGTEXT(_S8("CATIO Internal Error - Chat String signalled, but not found"));
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

CCommChatString* CATIO::FoundChatString()
/**
 * This returns a pointer to the string from the script to complete.
 * 
 * @param   None.
 * @return	pointer to CComChatString containing string to complete..
 */
	{
	return iCurrentFoundChatString;
	}

CCommChatString* CATIO::AddExpectString(CATBase* aATBase, const TDesC8& aString)
/**
 * This method is used to instantiate a CCompletionEntry object with a
 * CCommChatString object containing the expected string to complete the
 * string from the GSM TSY.  This CCompletionEntry is then added to 
 * the end of the expected list of strings from the GSM TSY.
 *
 * @param aATBase: pointer to the script engine processing the line of script.
 * @param aString: reference to the expected string for aATBase to be returned from the GSM TSY.
 * @return pointer to the chat string used in the instatiated CCompletionEntry.
 */
	{
	CCommChatString* cs=iChat->AddString(aString);
	CCompletionEntry* completionEntry=NULL;
	TRAP_IGNORE(completionEntry=CCompletionEntry::NewL(cs,aATBase));		// TRAP but ignore error
	if(completionEntry)
		iExpectList.AddLast(*completionEntry);
	return cs;
	}

void CATIO::RemoveExpectString(CCommChatString* aExpectString)
/**
 * This method uses the CCommChatString input parameter to remove a single 
 * CCompletionEntry from the list of expected completion strings.
 *
 * @param aExpectString: reference to the string in the list of expected string returned from the GSM TSY.
 */
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
/**
 * This method uses the CATBase input parameter to remove all of the 
 * CCompletionEntry(s) that have the same CATBase-derived callback class
 * from the list of expected completion strings.
 *
 * @param aTBase: reference to the string in the list of expected string returned from the GSM TSY.
 */
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
/**
 * This method is used to indicate a write to the comm port has completed.
 * The timer is stopped and if a write is NOT pending, then PANIC.
 * Set the write pending flag to EFalse and signal the write completion event 
 * with the status from the input parameter.
 * 
 * @param aStatus: status indicating reason for write complete.
 */
	{
	LOGTEXT(_S8("CATIO Comm Write Completion"));
	iChat->StopTimer();
	__ASSERT_ALWAYS(iWritePending,HayesPanic(EATCommand_IllegalCompletionWriteNotExpected));
	iWritePending=EFalse;
	iWriteCommand->GenericEventSignal(EWriteCompletion,aStatus);
	}

void CATIO::ChatStringMatchL(CCommChatString* aCs)
/**
 * This method is used to append the input string onto a list of found strings.
 * 
 * @param aCs: pointer to the chat string matched.
 * @leave Leaves if can not place string on end of found string list.
 */
	{
	LOGTEXT(_S8("CATIO Found Match."));
	iStringFound=ETrue;
	iChatStringFound->AppendL(aCs);
	}

void CATIO::ChatTimeout()
/**
 * This method is used to indicate a chat time-out has completed.
 * If a timer is NOT pending, then PANIC.  When the timer is pending, 
 * set the timer pending flag to EFalse and signal the timeout completion event.
 */
	{
	LOGTEXT(_S8("CATIO Chat Time-out Completion"));
	if(iWaitTimerPending)
		{
		iWaitTimerPending=EFalse;
		iTimeOutCommand->GenericEventSignal(ETimeOutCompletion,KErrNone);
		}
	else
		HayesPanic(EATCommand_IllegalWaitCompletion);
	}

void CATIO::SetTimeOut(CATBase* aCompletionClass, TUint aTimePeriodMillisec)
/*
 * This method starts the timer with the command in the input aCompletionClass
 * for the input specified period.  The indicater for timer pending is set to ETrue.
 *
 * @param aCompletionClass: pointer to object containing the time-out command.
 * @param aTimePeriodMillisec: time-out period in milliseconds.
 */
	{
	iTimeOutCommand=aCompletionClass;
	iChat->StartTimer(aTimePeriodMillisec*1000);
	iWaitTimerPending=ETrue;
	}

TInt CATIO::Read()
/*
 * This method attempts to read from the comm port only if there is currently 
 * not a read pending.  If a read is pending, this method simply returns without
 * attempting a read.  If no read is pending, this routing attempts to read as 
 * much data as is ready up to the maximum length specified in the descriptor, iRxBuf.
 * The data is read into the iRxBuf.
 */
 	{
	if(iReadPending==EFalse)
		{
 		iReadPending=ETrue;
		iStringFound=EFalse;
		CommReadOneOrMore(iRxBuf);
		iRxBufOffset = 0;
		LOGTEXT(_S8("CATIO Queued a Read"));
		}
	return KErrNone;
	}

TInt CATIO::Write(CATBase* aCompletionClass, const TDesC8& aString)
/*
 * This method attempts to write the input data into comm port using the 
 * write command in the input aCompletionClass.
 *
 * @param aCompletionClass: pointer to object containing function to call when the write completes.
 * @param aString, a descriptor reference containing the data to be written to the port.
 */
	{
	iWriteCommand=aCompletionClass;
	iWritePending=ETrue;
	CommWrite(aString);
	LOGTEXT2(_L8("Tx:\t%S"),&aString);
	LOGTEXTREL2(_L8("Tx:\t%S"),&aString);
	LOGTEXT(_S8("CATIO Queued a Transmission"));
	return KErrNone;
	}

TBool CATIO::RWPending()
/*
 * This method returns to the caller a boolean indicating True when 
 * either a Write is Pending  OR  a ReadPending is pending.
 *
 * @return Boolean indicating either a Write is Pending OR a Read is Pending.
 */
	{
	return (iWritePending)||(iReadPending);
	}

TBool CATIO::ReadPending()
/*
 * This method returns to the caller a boolean indicating True when 
 * a ReadPending is pending.
 *
 * @return Boolean indicating if a Read is Pending.
 */
	{
	return iReadPending;
	}

void CATIO::Disconnect()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	TCommConfig cbuf;
	TCommConfigV01 &cfg=cbuf();
	iCommPort.Config(cbuf);
	cfg.iHandshake = KConfigFreeRTS	| KConfigFreeDTR;
	iCommPort.SetConfig(cbuf);
	iCommPort.SetSignalsToSpace(KSignalRTS | KSignalDTR);
	CommClose();
	}

void CATIO::Cancel()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	LOGTEXT(_S8("CATIO CATIO:\tCancel called"));
	CommCancel();
	iReadPending = EFalse;
	iWritePending = EFalse;
	iChat->StopTimer();
	}

void CATIO::WriteAndTimerCancel(CATBase* aATBase)
/**
 * This method is used by the caller to either cancel a write command or stop a timer.
 * The action to be taken is determined by the command type received as a parameter.
 *
 * @param aATBase: pointer to the command type to be processed.
 */
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

void CATIO::WriteAndTimerCancelAll()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	CommWriteCancel();
	iChat->StopTimer();
	}

void CATIO::DropDtr()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	LOGTEXT(_S8("CATIO Dropping DTR"));
	iCommPort.SetSignals(0,KSignalDTR);
	}

void CATIO::RaiseDTR()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	LOGTEXT(_S8("CATIO Raising DTR"));
	iCommPort.SetSignals(KSignalDTR,0);
	}

void CATIO::ResetReadAndWriteBuffers()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	iCommPort.ResetBuffers();
	}

TInt CATIO::GetSizeOfRxBuffer()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	return iCommPort.QueryReceiveBuffer();
	}

TUint CATIO::Signals()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	return iCommPort.Signals();
	}

void CATIO::DropRTS()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	LOGTEXT(_S8("CATIO Dropping RTS"));
	iCommPort.SetSignals(0,KSignalRTS);
	}

void CATIO::RaiseRTS()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	LOGTEXT(_S8("CATIO Raising RTS"));
	iCommPort.SetSignals(KSignalRTS,0);
	}

void CATIO::SignalMark (TUint aSignal)
/**
 * This method is used by the caller on the emulator side to assert signals.
 *
 * @param aSignal: integer indicating the signal to assert.
 */
	{
	LOGTEXT2(_L8("CATIO CATIO::SignalSet(%x)"),aSignal); 
	iCommPort.SetSignalsToMark(aSignal);
	}

void CATIO::SignalSpace (TUint aSignal)
/**
 * This method is used by the caller on the emulator side to deassert signals.
 *
 * @param aSignal: integer indicating the signal to deassert.
 */
	{
	LOGTEXT2(_L8("CATIO CATIO::SignalClear(%x)"),aSignal);
	iCommPort.SetSignalsToSpace (aSignal);
	}
