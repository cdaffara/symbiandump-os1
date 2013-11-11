/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Handles client-side telephony audio routing requests.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <e32std.h>

#include "TelephonyAudioRouting.h"
#include "TelephonyAudioRoutingProxy.h"
#include "TelephonyAudioRoutingClientServer.h"
#include "TelephonyAudioRoutingSession.h"
#include "MTelephonyAudioRoutingObserver.h"


    
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CTelephonyAudioRoutingProxy::CTelephonyAudioRoutingProxy(
	MTelephonyAudioRoutingObserver& aObserver)
	: 	iObserver(aObserver),
		iAvailableOutputs(0, NULL, NULL)
    {
   
    }
    
    
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingProxy::ConstructL()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::ConstructL"),this);
	User::LeaveIfError(iSession.ConnectL(*this, iObserver));	

	// Make sure session is set up with default values from server for its local variables:
	iSession.GetDefaultValuesL();
	
	CTelephonyAudioRouting::TAudioOutput audioOutput = Output();

	// Get stored values for audio output and array and store values in local variables:
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::ConstructL, current iAudioOutput retrieved from server: %d"), audioOutput);	
	
	TArray<CTelephonyAudioRouting::TAudioOutput> availableOutputArray = AvailableOutputs();
	TInt count = availableOutputArray.Count();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t ConstructL: Count = %d "), count);
	for(TInt i = 0; i < count; i++)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t ConstructL: availableOutputs[i] = %d "),availableOutputArray[i]);
	}	

	iSession.NotifyIfOutputChanged();
	iSession.NotifyIfAvailableOutputsChanged();

	}

// Two-phased constructor.
EXPORT_C CTelephonyAudioRoutingProxy* CTelephonyAudioRoutingProxy::NewL(MTelephonyAudioRoutingObserver& aObserver)
    {
	CTelephonyAudioRoutingProxy* self = new(ELeave) CTelephonyAudioRoutingProxy(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }


// Destructor
CTelephonyAudioRoutingProxy::~CTelephonyAudioRoutingProxy()
    { 
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::~CTelephonyAudioRoutingProxy[%x]"),this);    
    iSession.Close();
    }



// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::AvailableOutputs
// Provides caller with an array of available audio outputs
// ---------------------------------------------------------
//
const TArray<CTelephonyAudioRouting::TAudioOutput>& CTelephonyAudioRoutingProxy::AvailableOutputs()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::AvailableOutputs"),this);
	
	TRAP_IGNORE(iSession.GetAvailableOutputsL());  // Session will get values from Server and store in its iAvailableOutputs
		
	TInt count = (iSession.AvailableOutputs()).Count();	
	
	iAvailableOutputs = (iSession.AvailableOutputs()).Array();
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::AvailableOutputs: Count = %d "), count);
	for(TInt i = 0; i < count; i++)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::AvailableOutputs: availableOutputs[i] = %d "),iAvailableOutputs[i]);
	}			
	return iAvailableOutputs;
	}
	
// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::Output
// Provides caller with the current audio output setting.
// (other items were commented in a header).
// ---------------------------------------------------------
//
CTelephonyAudioRouting::TAudioOutput CTelephonyAudioRoutingProxy::Output()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::Output"),this);
	
	TAudioOutput audioOutput = (iSession.CurrentAudioOutputPkg())();

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::Output: iSession's Audio Output = %d"),audioOutput);		
	return audioOutput;
	}

// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::SetOutputL
// Allows caller to set audio output to a particular value.
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CTelephonyAudioRoutingProxy::SetOutputL(TAudioOutput aOutput)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::SetOutput"),this);	
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::SetOutput with: %d"), aOutput);	

		TBool validOutput = ETrue;
		// Verify request is valid: (not necessary for ENone or ENotActive)
		if ((aOutput != CTelephonyAudioRouting::ENotActive) && (aOutput != CTelephonyAudioRouting::ENone))
		{
			validOutput = ValidateOutputRequest(aOutput);
		}
			
		if (validOutput)
		{
    		iSession.SetOutput(aOutput, iShowNote); 
    		// Reset iShowNote once it's sent
    		iShowNote = EFalse;   	
		}
		else
		{
			TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::SetOutput: Error: Requested output not in array"));
			// reset session's iOkToGetShowNoteMode flag, since it errored
			iSession.ResetShowNoteMode();			
			User::Leave(KErrHardwareNotAvailable);		
		}
	}
	
// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::PreviousOutput
// Provides caller with current value of previous audio output value.
// (other items were commented in a header).
// ---------------------------------------------------------
//

CTelephonyAudioRouting::TAudioOutput CTelephonyAudioRoutingProxy::PreviousOutput()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::PreviousOutput"), this);
	TAudioOutput prevOutput = (iSession.PreviousAudioOutputPkg())();

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::PreviousOutput: iSession's Previous Audio Output = %d"),prevOutput);		
	return prevOutput;	
	}	
	
// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::ValidateOutputRequest
// Checks that output requested is in list of available outputs.
// (other items were commented in a header).
// ---------------------------------------------------------
//

TBool CTelephonyAudioRoutingProxy::ValidateOutputRequest(TAudioOutput aOutput)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::ValidateOutputRequest"),this);		
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::ValidateOutputRequest for: %d"),aOutput);
	
	TArray<CTelephonyAudioRouting::TAudioOutput> availableOutputs = (iSession.AvailableOutputs()).Array();
	
	TBool found = EFalse;	
	TInt count = availableOutputs.Count();
	for(TInt i = 0; i < count; i++)
		{
			if (aOutput == availableOutputs[i])
			{
				found = ETrue;
				break;
			}
		}
		return found;		
	}	
	
	
// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::ShowNoteMode
// Provides caller with the current ShowNoteMode setting in server.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CTelephonyAudioRoutingProxy::ShowNoteMode()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::ShowNoteMode"),this);
	
	TPckgBuf<TBool> showNoteModePkg = iSession.ShowNoteMode();
	TBool showNoteMode = showNoteModePkg();
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::ShowNoteMode: = %d"), showNoteMode);		
	return showNoteMode;
	}

	
// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::SetShowNote
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//

TInt CTelephonyAudioRoutingProxy::SetShowNote( TBool aMode )
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::SetShowNote"),this);		
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::SetShowNote With: %d"),aMode);	

	// Note: Will set aMode on a GetShowNote call after SetOutputComplete called
	iShowNote = aMode; 	
	return KErrNone;
	}
// ---------------------------------------------------------
// CTelephonyAudioRoutingProxy::GetShowNote
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//

TInt CTelephonyAudioRoutingProxy::GetShowNote( TBool& aMode )
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy[%x]::GetShowNote"),this);

	// Check with session if Ok to retrieve iShowNoteMode from server:
	TBool okToGetShowNoteMode = iSession.OkToGetShowNoteMode();
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetShowNote: Session: Ok to get show note = %d"),okToGetShowNoteMode);	
	
	if (okToGetShowNoteMode)
	{
		aMode = ShowNoteMode();	
	}
	else
	{
		aMode = EFalse;
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::GetShowNote: (Not OK to Get Value from Server) Returning ShowNoteMode = %d"), aMode);			
	}
		
	// reset session's iOkToGetShowNoteMode flag once it has been retrieved
	iSession.ResetShowNoteMode();

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingProxy::GetShowNote: Returning ShowNoteMode = %d"), aMode);		

	return KErrNone;
	}
// End of File
