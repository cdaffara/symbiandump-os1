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
// This file implements the base methods for the CATBase class.  It deals primarily
// expect string handling and event notification.  
// used by the current etel regression test harness.
// 
//

/**
 @file
 @note	Only the constructors, destructors, panic function,and event handling code are 
*/

#include "Te_LoopBackATBASE.H"
#include "Te_LoopBackATIO.H"
#include "Te_LoopBackSLOGGER.H"

_LIT(KCsyName,"LOOPBACK");
_LIT(KPortName,"LOOPBACK::1");

const TBps KBaudRate=EBps115200;
const TDataBits KDataBits=EData8;
const TStopBits KStopBits=EStop1;
const TUint KHandshake=0;

//
// Panic Function
//
GLDEF_C void HayesPanic(THayesPanic aPanicNumber)
/**
 * This global method implements a test harness specific panic, identified as the Hayes
 * TSY.  The panic reason is passed as a parameter.
 *
 * @param		aPanicNumber, one of the reasons specified in the THayesPanic enumeration.
 * @return		None
 */
	{
	_LIT(KPanicHayesTSY,"Hayes TSY");
	User::Panic(KPanicHayesTSY,aPanicNumber);
	}

//
// CATParamListEntry
// A pointer to a meaningful result in the received buffer from the modem
//
CATParamListEntry::CATParamListEntry(const TPtrC8& aPtr)
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	iResultPtr.Set(aPtr);
	}

CATParamListEntry::~CATParamListEntry()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{}

void CATParamListEntry::Deque()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	iLink.Deque();
	}

//
// The Command Base Class
//
CATBase::CATBase()
/**
 * This constructor determines the offset of the iLink member of the a CATParamListEntry
 * and stores this offset in the iRxResults member of the CATBase object.
 *
 * @return	None
 */
	{
	iRxResults.SetOffset(_FOFF(CATParamListEntry,iLink));
	}

CATBase::~CATBase()
//
//	Assumes CATIO pointer is still valid
//
/**
 * This destructor first checks to see if the iResults list is empty.  If it is not, it
 * clears the list and deletes the entry.
 */
	{
	if(!iRxResults.IsEmpty())
		{
		CATParamListEntry* entry;
		TDblQueIter<CATParamListEntry> iter(iRxResults);
		while (entry = iter++,entry!=NULL)
			{
			entry->Deque();
			delete entry;
			}
		}
	}

void CATBase::ConstructL()
/**
 * This is the second phase constructor for the CATBase class.  It configures the port
 * specified by the combination of KCsyName and KPortName.  Configuration data includes
 * baud rate, data bits, stop bits, and handshake.
 *
 * @leave	This method leaves if there is an error configuring the port or potentially
 *			from the NewL method which is called.
 */
	{
	TFileName csyName(KCsyName);
	TName portName(KPortName);
	iIo=CATIO::NewL(csyName,portName);
	TCommConfigV01 commConfig;
	commConfig.iRate=KBaudRate;
	commConfig.iDataBits=KDataBits;
	commConfig.iStopBits=KStopBits;
	commConfig.iHandshake=KHandshake;

	TCommConfig commConfigWrapper(commConfig);
	TInt r=iIo->ConfigurePort(commConfigWrapper);
	User::LeaveIfError(r);
	}

void CATBase::GenericEventSignal(TEventSource aEventSource, TInt aStatus)
//
//	If an IO error has occurred, complete this ATBase-object with the error. 
//
/**
 * This method is the general event signal routine.  If an IO error occurred, 
 * the Complete method is called with the errored status.  If there is no 
 * error (aStatus is KErrNone) the EventSignal routine is called.
 *
 * @param	aEventSource a TEventSource member which indicates Read, Write,
 *			or Timeout completion.
 * @param	aStatus status from the IO operation.  If this status is NOT
 *			KErrNone, the Complete method will be called instead of the
 *			EventSignal method.
 */
	{		
	if (aStatus!=KErrNone)
		{
		LOGTEXT2(_L8("CATBase EventSignal received error status %d"),aStatus);
		Complete(aStatus);
		}
	else
		EventSignal(aEventSource);
	}

void CATBase::AddStdExpectStrings()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	if (!iOKExpectString)
		iOKExpectString=iIo->AddExpectString(this,KOKString);
	if (!iErrorExpectString)
		iErrorExpectString=iIo->AddExpectString(this,KErrorString);
	}

void CATBase::ValidateExpectStringL()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	if(iIo->FoundChatString()==iErrorExpectString)
		{
		LOGTEXT(_S8("CATBase Modem returned ERROR in response to command"));
		User::Leave(KErrGeneral);
		}
	if(iIo->FoundChatString()==iOKExpectString)
		return;
	LOGTEXT(_S8("CATBase Modem returned unexpected response to command"));
	User::Leave(KErrUnknown);
	}

void CATBase::RemoveStdExpectStrings()
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	LOGTEXT(_S8("RemoveStdExpectStrings()"));
	iIo->RemoveExpectString(iOKExpectString);
	iOKExpectString=NULL;
	iIo->RemoveExpectString(iErrorExpectString);
	iErrorExpectString=NULL;
	}

void CATBase::ParseBufferLC()
//
//  Parses buffer
//
/**
 * This function is currently not used by the Etel regression test harness.
 */	{
	LOGTEXT(_S8("CATBase Parse the Buffer List"));
	iBuffer.Set(iIo->GetRxBufferLC(iBufferMarker));
	TInt pos=iBuffer.FindF(KOKString);
	if(pos==KErrNotFound)
		{
		LOGTEXT(_S8("CATBase Error - Cannot find OK'"));
		User::Leave(pos);
		}
	//	Place everything before the OK into buffer
	iBuffer.Set(iBuffer.Left(pos));

	TLex8 yyLex(iBuffer);
	TChar peek;
	//	Look for '=' sign and move seeker cursor to the right of it if it exists
	pos=iBuffer.Find(_L8("="));
	if (pos!=KErrNotFound)
		{
		yyLex.Inc(pos+1);
		}
	//	Move cursor past any spaces or open brackets
	yyLex.SkipSpace();
	peek=yyLex.Peek();
	if ((TUint)peek=='(' || (TUint)peek=='[' || (TUint)peek=='{')
		yyLex.Inc();

	yyLex.SkipSpace();
	peek = yyLex.Peek();
	do
		{
		// Search for the next character that is not a comma, and mark it. Keep looking at 
		// subsequent characters until it is a space,comma,closing bracket or end of string.
		// Store the string (between marked character and current character) as an item in
		// an array of CATParamListEntry object pointers.
		if (peek!=',')
			{
			yyLex.Mark();
			do
				{
				yyLex.Inc();
				peek=yyLex.Peek();
				}
			while (peek!=',' && !peek.IsSpace() && peek!=')'&& peek!=']'&& peek!='}' && !yyLex.Eos());
			CATParamListEntry* aParamListEntry = new (ELeave) CATParamListEntry(yyLex.MarkedToken());
			iRxResults.AddLast(*aParamListEntry);
			}
		// Move cursor to the next non-space character, and end the loop if it is a closing
		// bracket or the end of the buffer.
		yyLex.Inc();
		yyLex.SkipSpace();
		peek = yyLex.Peek();
		}
	while (!yyLex.Eos() && peek!=')'&& peek!=']'&& peek!='}');
	}

void CATBase::StandardWriteCompletionHandler(TEventSource aSource,TInt aTimeOut)
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	__ASSERT_ALWAYS(aSource==EWriteCompletion,HayesPanic(EATCommand_IllegalCompletionWriteExpected));
	iIo->SetTimeOut(this,aTimeOut * KOneSecondPause);
	AddStdExpectStrings();
	}

void CATBase::Write(const TPtrC8& aCommand,TInt aTimeOut)
/**
 * This function is currently not used by the Etel regression test harness.
 */
	{
	iTxBuffer.Format(_L8("%S\r"),&aCommand);
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,aTimeOut * KOneSecondPause);
	}

void CATBase::Write(const TPtrC8& aCommand,TInt aTimeOut,TInt aValue)
/**
 * This function is currently not used by the Etel regression test harness.
 */	{
	iTxBuffer.Format(_L8("%S%d\r"),&aCommand,aValue);
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,aTimeOut * KOneSecondPause);
	}

void CATBase::WriteExpectingResults(const TPtrC8& aCommand,TInt aTimeOut)
/**
 * This function is currently not used by the Etel regression test harness.
 */	{						   
	iIo->MarkRxBuffer(iBufferMarker);
	Write(aCommand,aTimeOut);
	}
