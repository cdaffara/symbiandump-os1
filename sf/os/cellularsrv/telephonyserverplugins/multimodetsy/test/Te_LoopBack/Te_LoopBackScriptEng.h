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
// This file contains the classes and enumerations necessary to drive the scripting engine.  The
// allowed scripting commands, events, and actual script line structure are defined in this file.
// Note that the actual scripts are NOT defined in this file.
// See ScriptLanguage.htm for more information.
// Scripting Engine to execute command scripts
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SCRIPTENG_H__
#define __SCRIPTENG_H__

#include "Te_LoopBackSLOGGER.H"
#include "Te_LoopBackATBASE.H"
#include "Te_LoopBackATIO.H"

// Valid commands for the ETel Regression Test scripting "Language"
enum TScriptCommands
	{
	ETxString,				//< Transmit ASCII String; CR/LF automatically appended
	ETxStringWithoutCrLf,	//< Transmit ASCII String; No CR or LF.
	ETxStringOK,			//< Transmit the string "OK" including CR/LF.
	ERxString,				//< Expect String which the script engine will wait for
	EIfRxStringJmp,			//< A Conditional Branch, if the specified string is received, it branches.
	EIfRxStringRelativeJmp,	//< A Conditional Branch, if the specified string is received, it branches.
	EWait,					//< Delay specified time before continuing script; care must be used because of timing considerations
							//< NOTE: failures can occur if an EWait command is used before ERxEvent events.
							//< NOTE: safest place to use an EWait command is between ETxEvent events.
	EExecuteSpecial,		//< Asks script engine to call an algorithm specific to test scenario
	EComplete,				//< Informs script engine the script is complete
	ESignalMark,			//< Sets signal(s)
	ESignalSpace			//< Clears signal(s)
	};

// The completion event codes here drive when the script engine can proceed to the next line
// of the script.  Note that these values are bit flags and thus can be combined.  For example,
// the combination of ETxEvent | EFailIfTimeOut means that if a tx event complete is received go
// to the next step in the script.  If a time-out occurs, fail the script.
enum TCompletionEvent
	{
	ETxEvent=		0x1,	//< Progress to next script element if tx complete event is received
	ERxEvent=		0x2,	//< Progress to next script element if rx complete event is received
	ETimeOutEvent=	0x4,	//< Progress to next script element if time-out event occurs
	EFailIfTimeOut=	0x8,	//< If a time-out event occurs, fail the script
	EKeepExpectStrings =0x10	//< Allow expect string to be persistent even after it has been received
	};

// The TScript structure represents one line of a script.  Note that the iParam member has
// specific meaning depending on the command.
struct TScript
	{
	TInt iCommand;			//< Command
	TText8* iText;			//< Text related to Command
	TInt iParam;			//< Parameter related to Command, often a time-out.
							//< NOTE: for an EWait, an iParam value of -1 is only applicable to specific test scripts. 
							//< This indicates the timeout is specified by the delay parameter entered by the operator 
							//< or a test specific default value.
	TInt iCompletionEvent;	//< Flags to indicate the conditions for moving onto the
							//< next state.
	};

/**
 * The CATScriptEng base derives from the CATBase.  In turn, the emulator side of the specific
 * tests all derive from CATScriptEng.  This class drives the scripting engine for the Modem
 * emulator.  It is used to implement the ETel Regression Test Harness "Language".
 */
class CRestartSignal;
class CATScriptEng : public CATBase
	{
public:
	// Called in StartScriptL in the Responder Thread.  Implemented by each specific test case.
	virtual TInt Start()=0;

	// Called in ProcessScriptLine in the Responder Thread when an EExecuteSpecial 
	// command is in the script.  Implemented by each specific test case. 

	virtual void SpecificAlgorithmL(TInt aParam)=0;
	// Called in the Responder Thread for various reasons.  It is called when the script 
	// completes either by running to completion, timeout (if timeouts
	// force a failure), or because of another error.

	virtual void Complete(TInt aError)=0;
	void StartRestart(TRequestStatus** aStatus);
	void Restart();
	TInt iReturnValue;
protected:
	CATScriptEng();
	CATScriptEng(TInt aVarDelay);
	~CATScriptEng();
	void ConstructL();
	TInt StartScript(const TScript* aScript);
	// virtual function inherited from CATBase
	virtual void EventSignal(TEventSource aEventSource); 

private:
	void ProcessMultipleScriptLines();
	TInt ProcessScriptLine();

	const TScript* iStartScriptPnt;		//< Pointer to the beginning of the script in memory.
	const TScript* iScriptPnt;			//< Pointer into the current script being executed used
										//< to increment through script.
	TScript iCurrentScriptLine;			//< A copy of the current script line be processed.
										//< Used by line processing code.
	TInt     iDelay;					//< Variable delay value to be used as timeout by script 
										//< processing engine when iParam value is -1 in a TScript 
										//< script whose iCommand is an EWait.
// Variables to handle conditional Jump
	CCommChatString* iJmpChatString;	//< Used to compare incoming string vs. expected for 
										//< conditional jump
	TUint iJmpAbsolute;					//< Used to store the target absolute script line number
										//< for conditional jumping
	CRestartSignal* iRestartSignal;		//< Pointer to the Restart Signal class.
	};

#endif
