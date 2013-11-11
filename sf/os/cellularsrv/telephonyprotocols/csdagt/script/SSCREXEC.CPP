// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// NetDial Script Executor
// 
//

/**
 @file Sscrexec.cpp 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SSCREXECTraces.h"
#endif

#include "SSCREXEC.H"
#include "SIO.H"
#include "ND_SCR.H"

_LIT(KPPPProtocolName,"PPP.");

const TInt KNumErrorVariables=9;
//const TInt KMaxReadFromPctLength=256;
//const TInt KMaxReadFromPctLength8=2*KMaxReadFromPctLength;

const SErrorVariable ErrorVariables[KNumErrorVariables]=
	{
	{KErrExitScriptTimeOutString,KErrExitScriptTimeOut},
	{KErrExitLoginFailString,KErrExitLoginFail},
	{KErrExitNoModemString,KErrExitNoModem},
	{KErrExitModemErrorString,KErrExitModemError},
	{KErrExitNoAnswerString,KErrEtelNoAnswer},
	{KErrExitNoCarrierString,KErrEtelNoCarrier},
	{KErrExitNoDialToneString,KErrEtelNoDialTone},
	{KErrExitBusyString,KErrEtelBusyDetected},
	{KErrExitModemInitErrorString,KErrEtelInitialisationFailure},
	};

// CScriptExecutor definitons 
CScriptExecutor* CScriptExecutor::NewL(CNetDialScript* aScript,const TDesC& aCommChannel, TInt aScriptLength)

/**
2 phased constructor for CScriptExecutor, first phase.

@param aScript a pointer to used script.
@param aCommPort a reference to COMM port.
@param aScriptLength is lenght for used script.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CScriptExecutor object.
*/
	{
	CScriptExecutor* p=new(ELeave) CScriptExecutor(aScript);
	CleanupStack::PushL(p);
	p->ConstructL(aCommChannel, aScriptLength);
	CleanupStack::Pop();
	return p;
	}

CScriptExecutor::CScriptExecutor(CNetDialScript* aScript) 
	: CActive(EPriorityStandard), iScript(aScript), iInitializationFlag(EFalse),iServerRetransmitLoginPromptStatus(EFirstWaitCommand)
/**
Constructor for CScriptExecutor, used in the first phase of construction.

@param aScript a pointer to used script.
*/
	{}

void CScriptExecutor::ConstructL(const TDesC& aCommChannel, TInt aScriptLength)
/**
Instantiate member variables.

@param aCommPort a reference to COMM port.
@param aScriptLength is script lenght.
*/
	{
	CActiveScheduler::Add(this);

	iScriptIO=CScriptIO::NewL(this, aCommChannel);

// create script reader if script length is greater than zero	
	if (aScriptLength>0)
		{
		iScriptReader=CScriptReader::NewL(aScriptLength);
		TScriptStatus scriptStatus=iScriptReader->ScriptStatus();
		iVarMan=CScriptVarMan::NewL();
		iCharConv=CScriptCharacterConverter::NewL();
		iLabelMan=CScriptLabelMan::NewL();
// Construct Command Managers
		iWaitCommand=CWaitCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv,iScriptIO,iLabelMan, this);
		iCommands[0]=iWaitCommand;
		iCommands[1]=CSendCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv,iScriptIO);
		iReadCommand=CReadPCTCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv,this);
		iCommands[2]=iReadCommand;
		iLoopCommand=CLoopCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv);
		iCommands[3]=iLoopCommand;
		iCommands[4]=CExitCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv);
		iGotoCommand=CGotoCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv,iLabelMan);
		iCommands[5]=iGotoCommand;
		iCommands[6]=CSetCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv);
		iCommands[7]=CDTRCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv,iScriptIO);
		iCommands[8]=CCharMapCommand::NewL(scriptStatus,iScriptReader,iVarMan,iCharConv);
// Add Error Variables
		__ASSERT_DEBUG((sizeof(ErrorVariables)/sizeof(SErrorVariable))==KNumErrorVariables,NetDialPanic(ENumOfVariablesIncorrect));
		TInt i;
		for (i=0; i<KNumErrorVariables; i++)
			iVarMan->AddVariableL(TPtrC(ErrorVariables[i].iString),ErrorVariables[i].iValue);
		}
	else
		{
		iScriptReader=NULL;
		iVarMan=NULL;
		iCharConv=NULL;
		iLabelMan=NULL;
		iWaitCommand=NULL;
		iReadCommand=NULL;
		iGotoCommand=NULL;
		iLoopCommand=NULL;
		TInt i;
		for (i=0; i<KNumCommands; i++)
			iCommands[i]=NULL;
		}

// PCT stuff
	iReadFound=EFalse;
	iReadReq=0;
	iReadFromPctPending=EFalse;
	iDataToWrite.SetLength(0);
	}
	
CScriptExecutor::~CScriptExecutor()
/**
Destructor.
Cancels active requests and deletes members.
*/
	{
	Cancel();
	delete iDummySearchArray;
	delete iScriptIO;
	delete iScriptReader;
	delete iVarMan;
	delete iCharConv;
	delete iLabelMan;
	for(TInt i=0;i<KNumCommands	;i++)
		delete iCommands[i];
	}

void CScriptExecutor::SetLoginParams(const TDesC& aLoginName,const TDesC& aLoginPass)
/**
Sets login parameters.

@param aLoginName a reference to login name.
@param aLoginPass a reference to login password.
*/
	{
	iLoginName.Copy(aLoginName);
	iLoginPass.Copy(aLoginPass);
	}

void CScriptExecutor::SetScript(const TDesC& aScript)
/**
Sets script.

@param aScript a reference to login name.
*/
	{
	__ASSERT_DEBUG(iLastCommand==NULL, NetDialPanic(ELastCommandNotNull));
	__ASSERT_DEBUG(iScriptReader!=NULL, NetDialPanic(ENullScriptReader));

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTEXECUTOR_SETSCRIPT_1,"Script:\tBeginning");
	iScriptReader->SetScript(aScript);
	iLoopCommand->Loop(EFalse);
	}

void CScriptExecutor::ScanScriptL()
/**
Scans script for READ command.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTEXECUTOR_SCANSCRIPTL_1,"Script:\tScanning");

	__ASSERT_DEBUG(iScriptReader!=NULL, NetDialPanic(ENullScriptReader));

	iReadFound=EFalse;
	TInt ret=KErrNone;
	iScriptReader->SetLoggingOff();
	while (ret!=KErrScriptCompleted)
		{
		TRAP(ret,(iReadFound=iReadCommand->CheckReadL()));
		if (iReadFound || ret!=KErrNone)
			break;
		ret=iScriptReader->GetNextLine();
		}

	if (iReadFound)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTEXECUTOR_SCANSCRIPTL_2,"Script:\tFound Read");
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTEXECUTOR_SCANSCRIPTL_3,"Script:\tDid Not Find Read");
		}

	User::LeaveIfError(iScriptReader->Reset());
	}

void CScriptExecutor::RunScript()
/**
Runs script.
*/
	{
	iScriptIO->Start();
	iStatus=KRequestPending;
	SetActive();
	}

void CScriptExecutor::ProcessScript()
/**
Processes script line by line
*/
	{
	TRAPD(ret,ProcessNextLinesL());
	if(ret!=KErrNone)
		{
		CompletedScript(ret);
		}
	}

void CScriptExecutor::ProcessNextLinesL()
/**
If read/write not pending clear previous command.  Toggle skip mode if necessary then 
either parse label or jump to beginning of loop or parse command as necessary.
*/
	{
	__ASSERT_DEBUG(iScriptReader!=NULL, NetDialPanic(ENullScriptReader));

	while(!(iScriptIO->RWPending())&& !(iReadFromPctPending))
		{
		CleanupLastCommand();
		iGotoCommand->ServiceSkipReqs();
		if(iGotoCommand->ParseLabelL())
			continue;
		if(iLoopCommand->CheckLoopL())
			continue;
		
		TInt i;
		for(i=0;i<KNumCommands;i++)
			{
			if(iCommands[i]->ParseL())
				{
				iLastCommand=iCommands[i];
				break;
				}
			}
		if(iLastCommand==NULL)
			User::Leave(KErrIllegalCommand);
		}
	}

void CScriptExecutor::CompletedWrite(TInt aStatus)
/**
Stops script if aStatus is an error, otherwise continue to process.

@param aStatus is status passed from caller.
*/
	{
	if((aStatus==KErrTimedOut)&&(iInitializationFlag))
		CompletedScript(KErrExitNoModem);
	else if((aStatus==KErrNone)||(aStatus==KErrTimedOut))
		{
		if (iDataToWrite.Length()>0 && !iScriptIO->WritePending())
			{
			iScriptIO->Write(iDataToWrite);
			iDataToWrite.SetLength(0);
			}
		else
			{
			iInitializationFlag=EFalse;
			if(iServerRetransmitLoginPromptStatus == ESendingCR)
				{
				// The previous WAIT for a login prompt timed out and we sent 
				// a 0x0d to encourage the server to resend the login
				// So wait for the login prompt again
				iServerRetransmitLoginPromptStatus = ESecondWaitCommand;
				TRAPD(ret, iDummySearchArray=new(ELeave) CLabelSearchArray(1));
				if(ret != KErrNone)
					{
					iServerRetransmitLoginPromptStatus = EDeactivated;
					ProcessScript();
					}
				else iScriptIO->Read(iDummySearchArray, iRetransmitLoginTimeOut);
				}
			else ProcessScript();
			}
		}
	else	
		CompletedScript(aStatus);
	}

void CScriptExecutor::CompletedReadL(TInt aStatus,TInt aIndex)
/**
Stops script if there is an error, or otherwise continue to process or jump to 
label as ncessary.  Ignore comms line fail errors.

@param aStatus is passed from caller.
@param aIndex is index for label.
*/
	{
	__ASSERT_DEBUG(iScriptReader!=NULL, NetDialPanic(ENullScriptReader));

	if(iServerRetransmitLoginPromptStatus == ESecondWaitCommand)
		{
		// We must have sent a 0x0d to try and force the server to retransmit the 
		// login prompt
		// Delete the dummy search array and stop the RetransmitLogin mechanism since
		// irrespective of the outcome of the read, we're not going to send any more 0x0d
		delete iDummySearchArray;
		iDummySearchArray=NULL;
		iServerRetransmitLoginPromptStatus=EDeactivated;
		}

	if(iInitializationFlag)
		{
		iInitializationFlag=EFalse;
		ProcessScript();
		}
	else
		{
		switch(aStatus)
			{
		case KErrNone:
				{
				iServerRetransmitLoginPromptStatus=EDeactivated;
				TPtrC label=iWaitCommand->LabelFromIndexL(aIndex);
				iGotoCommand->Goto(label);
				}
			ProcessScript();
			break;
		case KErrTimedOut:
			// If the very first WAIT script command fails it's possible that the
			// server sent the login prompt before MM.TSY had finished with
			// the serial port
			// If so, we can send a 0x0d (Carriage Return) to wake it up 
			// and re-transmit the login prompt
			if(iServerRetransmitLoginPromptStatus == EFirstWaitCommand)
				{
				// Transmit a 0x0d to the server
				iServerRetransmitLoginPromptStatus=ESendingCR;
				TBuf8<1> CarriageReturn;
				CarriageReturn.SetLength(1);
				CarriageReturn[0]=0x0d;
				TPtrC8 CR(CarriageReturn);
				iScriptIO->Write(CR);
				break;
				}
			iServerRetransmitLoginPromptStatus=EDeactivated;
			ProcessScript();
			break;
		case KErrCommsLineFail:
			iServerRetransmitLoginPromptStatus=EDeactivated;
			ProcessScript();
			break;
		default:
			iServerRetransmitLoginPromptStatus=EDeactivated;
			CompletedScript(aStatus);
			break;
			}
		}
	}

void CScriptExecutor::CompletedScript(TInt aStatus)
/**
If aStatus is not an error, check that not still looking for a label.
Convert internal error to public error and finish.

@param aStatus is passed from caller.
*/
	{
	// Safety check - this method may be called from multiple locations,
	// possibly resulting in more than one call for the same event.
	if (iStatus==KRequestPending)
		{
		__ASSERT_DEBUG(iScriptReader!=NULL, NetDialPanic(ENullScriptReader));

		if(aStatus==KErrScriptCompleted)
			aStatus=KErrNone;
	
		TInt error=aStatus;

		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTEXECUTOR_COMPLETEDSCRIPT_1,"Script:\tScript Completed With Error %d",error);

		ConvertScriptError(error);
		PctCancelAndClose();

		CleanupLastCommand();
		iLoopCommand->Loop(EFalse);
	
		iScriptIO->Cancel();

		TRequestStatus* statusPtr=&iStatus;
		User::RequestComplete(statusPtr,error);
		}
	}

void CScriptExecutor::DoCancel()
/**
Cancel read/write, and callbacks and cleanup last command
*/
	{
	if(iServerRetransmitLoginPromptStatus == ESecondWaitCommand)
		{
		delete iDummySearchArray;
		iDummySearchArray=NULL;
		iServerRetransmitLoginPromptStatus=EDeactivated;
		}

	CleanupLastCommand();
	iScriptIO->Cancel();
	PctCancelAndClose();
	
	TRequestStatus* statusPtr=&iStatus;
	User::RequestComplete(statusPtr,KErrCancel);
	iScript->ScriptOperationComplete(KErrCancel);
	}

void CScriptExecutor::Disconnect()
/**
Disconect.
*/
	{
	iScriptIO->Disconnect();
	}

void CScriptExecutor::ReConfigureAndCancelCommPort(TRequestStatus& aStatus)
/**
Call ReConfigureAndCancelPort() from script IO handler.
*/
	{
	iScriptIO->ReConfigureAndCancelPort(aStatus);
	}


void CScriptExecutor::CloseScript()
/**
Close script.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTEXECUTOR_CLOSESCRIPT_1, "Script:\tClosing");
	if (iScriptReader!=NULL)
		iScriptReader->Reset();		// ignore error
	}

void CScriptExecutor::Close()
/**
Cleanup last command, close script file, and delete all variables and labels from lists.
*/
	{
	CleanupLastCommand();

	if (iLoopCommand!=NULL)
		iLoopCommand->Loop(EFalse);

	PctCancelAndClose();
	CloseScript();

	if (iVarMan!=NULL)
		iVarMan->DeleteAll();
	if (iLabelMan!=NULL)
		iLabelMan->DeleteAll();
	}

void CScriptExecutor::SetRetransmittedLoginTimeout(TReal& aRetransmitLoginTimeOut)
/**
Remember the timeout used on the wait statement that is currently executing

@param aRetransmitLoginTimeOut is passed from CWaitCommand.
*/
	{
	iRetransmitLoginTimeOut = aRetransmitLoginTimeOut;
	if(iServerRetransmitLoginPromptStatus == EFirstWaitCommand)
		{
		// This is the first time we are going to wait for anything
		// If we wait too long, the TCP/IP stack will time out so better not
		// wait more than say 30 seconds the first time around
		if(aRetransmitLoginTimeOut > 30)
			aRetransmitLoginTimeOut = 30;
		}
	}

TInt CScriptExecutor::WritePct(const TDesC8& aBuffer)
/**
CScriptIO calls to write incoming data to PCT. Convert the 8 bit data to
default character set and pass up to the dialog server.

@param aBuffer a reference to buffer to be written.
@return error code from WritePct() or leave code if ConvertFromDefaultL() leaves.
*/
	{
	__ASSERT_DEBUG(aBuffer.Length()<=KRxBufferSize, User::Invariant());
	TBuf<KRxBufferSize> convertedBuf;
	TRAPD(ret, iCharConv->ConvertFromDefaultL(aBuffer,convertedBuf));
	if (ret == KErrNone)
		{
		return iScript->WritePct(convertedBuf);
		}
	else
		{
		return ret;
		}
	}

void CScriptExecutor::SetReadPctBuffer()
/**
This is for the test dialog server only because we can't type in the value.
*/
	{
	iReadBuffer.SetLength(0);
	iReadReq++;
	if (iReadReq==1)
		iReadBuffer.Copy(iLoginName.Left(Min(iLoginName.Length(),3)));
	else if (iReadReq==2)
		{
		if (iLoginName.Length()>3)
			iReadBuffer.Copy(iLoginName.Right(iLoginName.Length()-3));
		iReadBuffer.Append('.');
		}
	else if (iReadReq==3)
		iReadBuffer.Copy(iLoginPass.Left(Min(iLoginPass.Length(),3)));
	else if (iReadReq==4)
		{
		if (iLoginPass.Length()>3)
			iReadBuffer.Copy(iLoginPass.Right(iLoginPass.Length()-3));
		iReadBuffer.Append('.');
		}
	else if (iReadReq==5)
		iReadBuffer.Copy(KPPPProtocolName);
	else
		iReadBuffer.Zero();
	}	

void CScriptExecutor::ReadPct()
/**
Read PCT.
*/
	{
	SetReadPctBuffer();
	iScript->ReadPct(iReadBuffer);
	iScriptIO->ReadEcho();
	iReadFromPctPending=ETrue;
	}

void CScriptExecutor::ReadPctComplete(TInt aStatus)
/**
Data recived from PCT to be written to port. For unicode - need to know the
character set it should be sent in and convert to this from  UNICODE (or whatever 
PCT uses).  Pass to the Script IO as 8 bit.

@param aStatus passed from caller.
*/
	{
	__ASSERT_ALWAYS(iReadFromPctPending, NetDialPanic(EIllegalReadPctComplete));

	if (aStatus==KErrCancel)	// means that NetDial has called Cancel() or ClosePct() while read request outstanding
		{
		iReadFromPctPending=EFalse;
		return;
		}
	
	if (aStatus!=KErrNone)		// something other than KErrCancel
		{
		iScriptIO->ReadEchoCancel();
		iReadFromPctPending=EFalse;
		CompletedScript(aStatus);
		return;
		}
		
	HBufC8* buf=NULL;
	TRAPD(ret,buf=iCharConv->ConvertL(iReadBuffer,iReadCommand->CharSet()));
	if(ret!=KErrNone)
		{
		iScriptIO->ReadEchoCancel();
		iReadFromPctPending=EFalse;
		CompletedScript(ret);
		return;
		}
	TPtr8 eightBitBuf(buf->Des());
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTEXECUTOR_READPCTCOMPLETE_1, "Script:\tRead %s from PCT",eightBitBuf);
	TInt end=iReadBuffer.Locate(KCarriageReturn);
	TInt spaceInBuffer=iDataToWrite.MaxLength()-iDataToWrite.Length();
	
	if (end<0)
		iDataToWrite.Append(eightBitBuf.Left(Min(spaceInBuffer,eightBitBuf.Length())));
	else
		iDataToWrite.Append(eightBitBuf.Left(Min(spaceInBuffer,end+1)));

	if (!iScriptIO->WritePending())
		{
		iScriptIO->Write(iDataToWrite);
		iDataToWrite.SetLength(0);	// clear buffer so we know it's been sent
		}	

	if (end<0)
		{
		SetReadPctBuffer();
		iScript->ReadPct(iReadBuffer);
		}
	else
		{
		iScriptIO->ReadEchoCancel();
		iReadFromPctPending=EFalse;
		}

	delete buf;
	}

void CScriptExecutor::DestroyPctNotificationReceived(TInt aStatus)
/**
PCT destroyed. Assume KErrNone means destroyed by user / someone else and
otherwise we destroyed it.

@param aStatus is passed from caller.
*/
	{
	if (aStatus==KErrNone)		// user cancelled
		{
		iScriptIO->Cancel();
		iScript->CancelDialogServer();			// cancels any outstanding reads
		CompletedScript(KErrCancel);	// this will close the PCT
		}	
	else
		iScript->ClosePct();			// NetDial is in charge of closing the PCT whatever happens

	}

void CScriptExecutor::PctCancelAndClose()
/**
Cancel any outstanding requests and cancel the active object waiting on the 
destroy notification) and close the pct
*/
	{
	iScript->CancelDialogServer();
	iScript->ClosePct();
	}

void CScriptExecutor::SetVariableL(const TDesC& aName, const TDesC& aContent)
/**
Add variable with name aName and content aContent to list

@param aName a reference to variable name.
@param aContent a reference to content.
*/
	{
	iVarMan->AddVariableL(aName,aContent);
	}

void CScriptExecutor::ConfigureCommPort(TRequestStatus& aStatus, const TCommConfig& aConfiguration)
/**
Configure COMM port.

@param aConfiguration a reference to confiquration
@return error code from ConfigurePort().
*/
	{
	iScriptIO->ConfigurePort(aStatus, aConfiguration);
	}

void CScriptExecutor::DropSignals(TRequestStatus& aStatus)
/**
Drop signals on COMM port.
*/
	{
	iScriptIO->DropSignals(aStatus);
	}

void CScriptExecutor::CancelConfigureCommPort()
/**
Cancel Configure COMM port.
*/
	{
	iScriptIO->CancelConfigurePort();
	}

void CScriptExecutor::DropDTR(TRequestStatus* aStatusPtr)
/**
Drop DTR.
*/
	{
	iScriptIO->DropDTR(aStatusPtr);
	}

void CScriptExecutor::CreateChannel(TRequestStatus& aStatus)
	{
	ASSERT(iScriptIO);
	iScriptIO->CreateChannel(aStatus);
	}

void CScriptExecutor::CancelCreateChannel()
	{
	ASSERT(iScriptIO);
	iScriptIO->CancelCreateChannel();
	}

void CScriptExecutor::ShutdownChannel(TRequestStatus& aStatus)
	{
	ASSERT(iScriptIO);
	iScriptIO->ShutdownChannel(aStatus);
	}

TInt CScriptExecutor::GetExcessData(TDes8& aBuffer)
/**
Get excess data.

@param aBuffer a reference to excess data.
*/
	{
	return iScriptIO->GetExcessData(aBuffer);
	}

void CScriptExecutor::ConvertScriptError(TInt& aError) const
/**
Convert internal errors to KErrExitScriptError.

@param a reference to error code. New value is passed back by using it.
*/
	{
	if(aError<KNetdialInternalErrorBase)	
		aError=KErrExitScriptError;
	}	

void CScriptExecutor::CleanupLastCommand()
/**
Cleanup last command.
*/
	{
	if(iLastCommand!=NULL)
		{
		iLastCommand->Cleanup();
		iLastCommand=NULL;
		}
	}

void CScriptExecutor::RunL()
/**
Complete script operation.
*/
	{
	iScript->ScriptOperationComplete(iStatus.Int());
	}

