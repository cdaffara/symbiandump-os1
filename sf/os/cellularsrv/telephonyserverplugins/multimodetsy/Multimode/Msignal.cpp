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
// Queries the ME signal quality. Retrieves signal level and BitErrorRate (ber)
// 
//

#include <et_phone.h>
#include "Msignal.h"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "Matstd.h"

CATGetSignal* CATGetSignal::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATGetSignal* signal = new(ELeave) CATGetSignal(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(signal);
	signal->ConstructL();
	CleanupStack::Pop();
	return signal;
	}

CATGetSignal::CATGetSignal(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals)
{}

void CATGetSignal::ConstructL()
	{
	CATCommands::ConstructL();
	}

CATGetSignal::~CATGetSignal()
	{
	iIo->RemoveExpectStrings(this);
	}


void CATGetSignal::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams) 
	{
	LOGTEXT(_L8("MMTsy:\tCATGetSignal:\tStarting ME signal quality query."));
	iReqHandle = aTsyReqHandle;

	//
	// Validate the arguments
	TTsySignalInfo* signal = static_cast<TTsySignalInfo*>(aParams);
	__ASSERT_DEBUG(signal!=NULL,Panic(EATGetSignalNullParameter));
	__ASSERT_DEBUG(signal->iBar!=NULL,Panic(EATGetSignalNullParameter));
	__ASSERT_DEBUG(signal->iSignalStrength!=NULL,Panic(EATGetSignalNullParameter));

	//
	// Store the arguments
	iSignalInfo.iSignalStrength = signal->iSignalStrength;
	iSignalInfo.iBar = signal->iBar;

	iTxBuffer.Format(KGetSignalQualityCommand);
	iIo->Write(this, iTxBuffer);
	iIo->SetTimeOut(this, 5000);
	iState=EATSignalWaitForWriteComplete;
	}


void CATGetSignal::EventSignal(TEventSource aSource)
	{
	if (aSource==ETimeOutCompletion)
		{
		LOGTEXT(_L8("MMTsy:\tCATGetSignal:\tTimeout Error during signal quality query"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
		case EATSignalWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATSignalReadComplete;
			}
		break;

		case EATSignalReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{	
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret)
				{
				Complete(ret,aSource);
				return;
				}
			TRAP(ret,ParseSignalResponseL());
			Complete(ret,aSource);
			}
		break;

		default:
		break;
		}//switch
	}//EventSignal


void CATGetSignal::Stop(TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));
	LOGTEXT2(_L8("CATGetSignal::Stop - Cancelling from state %d"), iState);
	
	if (iState==EATSignalWaitForWriteComplete)
		{
		Complete(KErrCancel, EReadCompletion);
		}
	// else do nothing because we have already sent the AT command
	// or have not actually started doing anything!
	}


void CATGetSignal::Complete(TInt aError, TEventSource aSource)
	{
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;

	CATCommands::Complete(aError, aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);
	
	LOGTEXT2(_L8("MMTsy:CATGetSignal:\tCATGetSignal completed with error code : %d"), aError);
	iState = EATNotInProgress;
	}


void CATGetSignal::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = EATNotInProgress;
		}
	}

void CATGetSignal::ParseSignalResponseL()
	{
	ParseBufferLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	TBool first = ETrue;
	TUint icounter = 0;
	TInt ret;

	while (entry = iter++, entry != NULL)
		{
		if (first)
			{
			first = EFalse;
			while (entry->iResultPtr != KCSQResponseString)
				{
				entry->Deque();
				delete entry;
				entry = iter++;
				if (entry == NULL)
					{
					CleanupStack::PopAndDestroy();
					User::Leave(KErrNotFound);
					}
				} //while
			LOGTEXT(_L8("MMTsy:\tCATGetSignal:\tFound +CSQ string!"));
			}
		else
			{
			TLex8 aLex((entry->iResultPtr).Ptr());
			if (icounter++ == 0)
				{	
				TInt32 signalStrength; 
				ret = aLex.Val(signalStrength);
				if (ret == KErrNone)
					{
					//Calculate the number of signal bars 
					TInt8 numberOfBars;
					numberOfBars = NumberOfSignalBars(signalStrength);

					// Convert the the 0 to 31 range(in aSignalStrength) which the AT 
					// command returns to the corresponding dBm values(-113dBm to -51dBm).
					signalStrength = (-113 + (2 * signalStrength));
					
					if((iSignalInfo.iBar))					// Ensure we do not cause an access violation
						*(iSignalInfo.iBar) = numberOfBars;		

					if((iSignalInfo.iSignalStrength))	// Ensure we do not cause an access violation
						*(iSignalInfo.iSignalStrength) = signalStrength;
					}
				}
			}//else
		entry->Deque();
		delete entry;
		}//while

	CleanupStack::PopAndDestroy();
	}//ParsesignalResponseL


TInt8 CATGetSignal::NumberOfSignalBars(TInt32 aSignalStrength)

/** * Number of signalbars *
 * This method returns the number of signal bars that the phone should show on the display. 
 * @param aSignalStrength points to the signalstrength given in the range 0 to 31. 0 to 31 is returned by the phone(ETSI standard) and represents -113dB to -51dB.
 * @return number of bars. Can take the values (-1) to 5 (-1 represents KErrNotFound)
 */
	{
	TInt8 bars; 

	if(aSignalStrength == KSignalStrengthMinus113dBm)
		{
		bars = 0;
		}
	else if (aSignalStrength <= KSignalStrengthMinus101dBm)
		{
		bars = 1;
		}
	else if (aSignalStrength <= KSignalStrengthMinus89dBm)
		{
		bars = 2;
		}
	else if (aSignalStrength <= KSignalStrengthMinus77dBm)
		{
		bars = 3;
		}
	else if (aSignalStrength <= KSignalStrengthMinus53dBm)
		{
		bars = 4;
		}

	else if(aSignalStrength == KSignalStrengthMinus51dBm)
		{
		bars = 5;
		}
	else
		{
		bars=KErrNotFound; 
		}

	return bars;

	} //End of NumberOfSignalBars

