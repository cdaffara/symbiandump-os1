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
// This file contains the scripting engine which processes a given script on demand.  It
// uses services from the ATBase Class and the CATio Class.  All commands from the enumeration
// TScriptCommands must be represented in the switch statement in the method ProcessScriptLine.
// Any command not represented is ignored.
// Scripting Engine Code
// 
//

/**
 @file
*/

#include "Te_LoopBackScriptEng.h"
#include "Te_LoopBackrestartsignal.h"

#define EForever (1==1)

const TInt KStandardTxTimeout=5000;	// Five seconds

// A completion signal that can be passed through the error channel without ambiguity
// I.e. any non-zero positive number
const TInt KScriptCompletionSignal=99;

CATScriptEng::CATScriptEng()
/**
 * The standard constructor for this class for scripts without a variable delay.
 */
	{}

CATScriptEng::CATScriptEng(TInt aVarDelay) : iDelay(aVarDelay)
/**
 * The constructor for this class for scripts with a variable delay.
 */
	{}

CATScriptEng::~CATScriptEng()
/**
 * The standard destructor for this class.
 */
	{
	delete iRestartSignal;
	}

void CATScriptEng::ConstructL()
/**
 * The second stage contructor for the CATScriptEngClass.  It simply calls the second stage
 * constructor for the CATBase class.
 *
 * @return	None
 * @note	This ConstructL routine does not appear to be referenced.
 */
	{
	CATBase::ConstructL();
	iRestartSignal=CRestartSignal::NewL(this);
	}

void CATScriptEng::StartRestart(TRequestStatus** aStatus)
	{
	iRestartSignal->Start(aStatus);
	}

TInt CATScriptEng::StartScript(const TScript* aScript)
/**
 * This method starts a specified script.  The script is passed in as a parameter.
 * The CATScriptEng::StartScript method first invokes the Start method of the CATIO class.
 * It saves the parameter in private member data of the CATScriptEng class.
 * It then invokes the method ProcessMultipleScriptLines to begin processing
 * the referenced script.
 *
 * @param	aScript - a constant script pointer specified by the invoked test.
 *
 * @return	KErrNone
 */
	{
	iIo->Start(this);
	iStartScriptPnt=iScriptPnt=aScript;
	ProcessMultipleScriptLines();
	return KErrNone;
	}

void CATScriptEng::EventSignal(TEventSource aEventSource)
/**
 * This method is called when the completion event is signaled.  The appropriate
 * action is taken based on the event source.  This method handles the events defined in
 * the TCompletionEvent enumeration.  These events include a transmit event, a receive event,
 * a timeout event and a failure if timeout event.  If the script has specified to fail if
 * a timeout occurs (EFailIfTimeOut completion event), this routine will call the Complete
 * method specified by the test (normally the active scheduler is stopped by this method)
 * with the error KErrTimedOut.  This event method is used for active object read completions,
 * write completions, and timeouts via the CATBase::GenericEventSignal method.
 *
 * The iCompletionEvent member of the current script line (pointed to by iScriptPnt) is used
 * to test the incoming event.  Unless the EKeepExpectStrings flag is specified in the event,
 * the expect strings used by the scripting engine are removed.  This is the normal mode of 
 * operation.  The event ERxEvent is associated with the EReadCompletion event source
 * (the aEventSource parameter), the event ETxEvent is associated with the EWriteCompletion
 * event source, and the ETimeout event is associated with the ETimeOutCompletion event source.
 * In the case of the Read and TimeOut Completions, there is an attempt to cancel outstanding
 * commands.
 *
 * If an invalid completion event is specified or an invalid event source is passed into this
 * function, it will panic.
 *
 * @param	aEventSource, an event source from the TEventSource enumeration.
 *
 * @return	None
 */
	{
	if(aEventSource==ERestartSignal)
		{
		iIo->Cancel();
		iIo->RemoveExpectStrings(this);
		iJmpChatString=NULL;
		StartScript(iStartScriptPnt);
		return;
		}

	TCompletionEvent ev=(TCompletionEvent)iScriptPnt->iCompletionEvent;

	if((ev&EFailIfTimeOut)&&(aEventSource==ETimeOutCompletion))
		{
		Complete(KErrTimedOut);
		return;
		}

//
// Check if a conditional jump may be required
//
	if((ev&ERxEvent)&&(iJmpChatString))
		{
		LOGTEXT(_S8("TsyLb:\tJmp Chat String defined, but not necessarily found yet"));
		if(iIo->FoundChatString()==iJmpChatString)
			{
			LOGTEXT(_S8("TsyLb:\tFound Jmp Chat String, Jumping..."));
			iScriptPnt=iStartScriptPnt+iJmpAbsolute-1;		// Subtract one because it will be incremented below
			}
		}

//
// Should the Expect Strings be kept
//
	if(!(ev&EKeepExpectStrings))
		{
		iIo->RemoveExpectStrings(this);
		iJmpChatString=NULL;
		}

	if((ev&ERxEvent)&&(aEventSource==EReadCompletion))
		{
		iScriptPnt++;
		ProcessMultipleScriptLines();
		return;
		}

	if(	((ev&ETxEvent)&&(aEventSource==EWriteCompletion)) ||
		((ev&ETimeOutEvent)&&(aEventSource==ETimeOutCompletion)) )
		{
		iIo->WriteAndTimerCancel(this);
		iScriptPnt++;
		ProcessMultipleScriptLines();
		return;
		}

	HayesPanic(EATCommand_IllegalCompletionWriteNotExpected);
	}

void CATScriptEng::ProcessMultipleScriptLines()
//
// Process multiple script lines
//
/**
 * This method is used to drive the processing of the script.  A loop is entered which calls
 * another method to process a single line.  The member data iCurrentLineScript is set to
 * the current line of the script.  If an error is returned from the ProcessScriptLine method,
 * the loop is exited without moving the current line in the script.  If the current line in
 * the script specifies a non-zero completion event, the loop is exited without moving the
 * current line.  In the event that a non-zero completion event is specified, the event handling
 * code will move the current pointer and then call this method again.  A zero completion event
 * means that the command is completed when it has been issued.  There is no need to wait for
 * any event before continuing.  An example of this type of event is the EComplete command.  When
 * a zero completion event was specified, the iScriptPnt is incremented and the next line is
 * immediately processed.
 * Most transmits and receives require an event to complete the command (i.e., a read or write
 * completion).  This is noted by a non-zero completion event.  In this case, the script pointer
 * is not moved (until the command is completed).
 *
 * @return	None
 */
	{
	for(;;)
		{
		iCurrentScriptLine=*iScriptPnt;
		LOGTEXT(_S8("------------CurrentScriptLine---------------")) ;
		if(ProcessScriptLine()!=KErrNone)
			break;
		if(iCurrentScriptLine.iCompletionEvent!=0)
			break;
		iScriptPnt++;
		}
	}

TInt CATScriptEng::ProcessScriptLine()
//
// Process a single line of the script
//

/**
 *
 * This method processes a single line from a script.  It switches on the command type
 * which is set up in the script itself.  Command types are documented in ScriptEng.h.
 * The member data iCurrentScriptLine is a variable of type TScript.  TScript is a
 * structure with several member.  There is a member (iCommand) which is an integer from the
 * enumeration TScriptCommands.  The second member is a pointer to a string (iText).  This is
 * text to be used to execute the command.  The third member is a parameter (iParam) which is
 * command specific.  A common use for iParam is a time-out value.  The fourth member is a
 * completion event (iCompletionEvent) which contains a value or set of flags that indicates
 * when it is okay to move to the next state in the script.  When this is a non-zero value it
 * is a member of the enumeration TCompletionEvent.
 *
 * The script commands cause different actions to occur.  The transmit commands (ETxString,
 * ETxStringWithoutCrLf and ETxStringOK) result in writes into the tsy (as the modem would 
 * normally write to the tsy).  The command ERxString results in the script engine looking for
 * output from the tsy (as the tsy would normally write to the modem).  The command EIfRxStringJmp
 * allows for rudimentary branching.  In this case the script engine will wait for the specified
 * text before branching to the script array element specified in the iParam entry of this 
 * script line.  The EWait command causes a timer to be started for the time frame specified in
 * the command script line via iParam.  The EExecuteSpecial command executes a test specific 
 * algorithm.  The EComplete command is used to notify the script engine that the script is
 * complete.  When the EComplete command is processed, the Complete method is called.  Each test
 * must define the Complete method as it is a pure virtual function in the CATScriptEng
 * class.  Many of the tests simply set a return value and stop the active scheduler in their
 * Complete routines.
 *
 * @return		KErrNone
 * @return		KScriptCompletionSignal in response to the EComplete Command
 * @return		Variable exceptions possible in response to the EExecuteSpecial command.  In
 *				this case, the return value is specified by the test specific algorithm.
 *
 * 
 */
	{
	TInt ret=KErrNone;

	// Log the the logfile the current script line which is being executed
	{
	TInt32 scriptLine=reinterpret_cast<TInt32>(iScriptPnt);
	scriptLine-=reinterpret_cast<TInt32>(iStartScriptPnt);
	scriptLine/=sizeof(TScript);
	LOGTEXT2(_L8("TsyLb:\tCurrentScriptLine=%d"),scriptLine);
	}

	switch(iCurrentScriptLine.iCommand)
		{
		case ESignalMark:
				{
				LOGTEXT(_S8("TsyLb:\tESignalSet command"));
				iIo->SignalMark(iCurrentScriptLine.iParam);
				}
			break;

		case ESignalSpace:
				{
				LOGTEXT(_S8("TsyLb:\tESignalClear command"));
				iIo->SignalSpace(iCurrentScriptLine.iParam);
				}
			break;

		case ETxString:
				{
				LOGTEXT(_S8("TsyLb:\tETxString command"));
				TBuf8<KTxBufferSize> outputString((TText8*)iCurrentScriptLine.iText);
				iTxBuffer.Copy(outputString);
				iTxBuffer.Append('\n');
				iTxBuffer.Append('\r');
				iIo->Write(this, iTxBuffer);
				TInt timeout=KStandardTxTimeout;
				if(iCurrentScriptLine.iParam!=0)
					timeout=iCurrentScriptLine.iParam*1000;
				iIo->SetTimeOut(this, timeout);
				}
			break;

		case ETxStringWithoutCrLf:
				{
				LOGTEXT(_S8("TsyLb:\tETxStringWithoutCrLf command"));
				TBuf8<KTxBufferSize> outputString((TText8*)iCurrentScriptLine.iText);
				iTxBuffer.Copy(outputString);
				iIo->Write(this, iTxBuffer);
				TInt timeout=KStandardTxTimeout;
				if(iCurrentScriptLine.iParam!=0)
					timeout=iCurrentScriptLine.iParam*1000;
				iIo->SetTimeOut(this, timeout);
				}
			break;

		case ETxStringOK:
				{
				LOGTEXT(_S8("TsyLb:\tETxStringOK command"));
				_LIT8(KOKString,"\nOK\r\n");
				iIo->Write(this, KOKString);
				TInt timeout=KStandardTxTimeout;
				if(iCurrentScriptLine.iParam!=0)
					timeout=iCurrentScriptLine.iParam*1000;
				iIo->SetTimeOut(this, timeout);
				}
			break;

		case ERxString:
				{
				TBuf8<KRxBufferSize> expectString((TText8*)iCurrentScriptLine.iText);
				iIo->AddExpectString(this,expectString);
				LOGTEXT2(_L8("TsyLb:\tERxString command: %S"),&expectString);
				iIo->MarkRxBuffer(iBufferMarker);
				}
			break;

		case EIfRxStringJmp:
				{
				LOGTEXT(_S8("TsyLb:\tEIfRxStringJmp command"));
				TBuf8<KRxBufferSize> expectString((TText8*)iCurrentScriptLine.iText);
				iJmpChatString=iIo->AddExpectString(this,expectString);
				iJmpAbsolute=iCurrentScriptLine.iParam;
				iIo->MarkRxBuffer(iBufferMarker);
				}
			break;

		case EIfRxStringRelativeJmp:
				{
				LOGTEXT(_S8("TsyLb:\tEIfRxStringJmp command"));
				TBuf8<KRxBufferSize> expectString((TText8*)iCurrentScriptLine.iText);
				iJmpChatString=iIo->AddExpectString(this,expectString);
				//hack to calculate current position in array.
				//Why developers not use indexes for arrays, did not?
				iJmpAbsolute = (iScriptPnt - iStartScriptPnt)+iCurrentScriptLine.iParam;
				iIo->MarkRxBuffer(iBufferMarker);
				}
			break;

		case EWait:
			LOGTEXT(_S8("TsyLb:\tEWait command"));
			if (-1 == iCurrentScriptLine.iParam)
				iIo->SetTimeOut(this, iDelay*1000);
			else
				iIo->SetTimeOut(this, iCurrentScriptLine.iParam*1000);
			break;

		case EExecuteSpecial:
			{
			LOGTEXT(_S8("TsyLb:\tEExecuteSpecial command"));
			TRAP(ret,SpecificAlgorithmL(iCurrentScriptLine.iParam));
			if(ret!=KErrNone)
				Complete(ret);
			break;
			}
			
		case EComplete:
				{
				LOGTEXT(_S8("TsyLb:\tEComplete command"));
				iIo->RemoveExpectStrings(this);
				iOKExpectString = NULL;
				iErrorExpectString = NULL;
				ret=KScriptCompletionSignal;
				iRestartSignal->Cancel();
				TInt r=KErrNone;
				Complete(r);
				}
			break;

		default:
			break;
		}
	return ret;
	}

void CATScriptEng::Restart()
	{
	EventSignal(ERestartSignal);
	}

