// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//



//INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdtmftsyTraces.h"
#endif

#include "cmmdtmftsy.h"
#include <etelmmcs.h> 

// include internal Tsy classes
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmcalltsy.h"
#include "cmmcalllist.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmDtmfTsy::CMmDtmfTsy()
    {
    }

void CMmDtmfTsy::ConstructL()
    {
    iSendDTMFTonesCancelFlag = EFalse;
    iDtmfType = EDtmfTypeUnknown;

    //register dtmf tsy in the message manager
    iMmPhone->MessageManager()->RegisterTsyObject(
        CMmMessageManagerBase::EDtmfTsy, this );
    }

CMmDtmfTsy* CMmDtmfTsy::NewL(
    CMmPhoneTsy* aPhoneTsy ) // Pointer to PhoneTsy
    {
    CMmDtmfTsy* self = new ( ELeave ) CMmDtmfTsy;
    
    // this cannot leave:
    self->iMmPhone = aPhoneTsy;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
CMmDtmfTsy::~CMmDtmfTsy()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_DTOR_1, "TSY: CMmDtmfTsy::~CMmDtmfTsy");
    if (iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject(this);        
        }
    iMmPhone = NULL;
    iDTMFStringToContinue.Zero();
    iRetDtmfCaps = NULL;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::DoExtFuncL
// DTMF-specific functionality of CMmPhoneTsy::DoExtFuncL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::DoExtFuncL( 
	const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc, 
    const TDataPackage& aPackage )  
                                  
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_DOEXTFUNCL_1, "TSY: CMmDtmfTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);
    TInt ret = KErrNone;
    TAny* dataPtr = aPackage.Ptr1();
    TBool cont ( EFalse );

    switch ( aIpc )
        {
        // supported DTMF features

        // Get DTMF Caps
        case EMobilePhoneGetDTMFCaps:
        	ret = GetDTMFCaps( aTsyReqHandle, 
            reinterpret_cast<TUint32*>( dataPtr ) );
            break;	
        // Notify Change of DTMF Capabilities
        case EMobilePhoneNotifyDTMFCapsChange:
        	ret = NotifyDTMFCapsChange( 
            reinterpret_cast<TUint32*>( dataPtr ) );
            break;	
        // Send DTMF Tones 
        case EMobilePhoneSendDTMFTones:
            ret = SendDTMFTonesL( aTsyReqHandle, aPackage.Des1u() );
            break;
        // Start DTMF Tone
        case EMobilePhoneStartDTMFTone:
            ret = StartDTMFToneL( aTsyReqHandle,
            reinterpret_cast<TChar*>( dataPtr ) );
            break;	
        // Stop DTMF Tone
        case EMobilePhoneStopDTMFTone:
            ret = StopDTMFToneL( aTsyReqHandle );
            break;	
        // Notify Stop in DTMF String
        case EMobilePhoneNotifyStopInDTMFString:
            ret = NotifyStopInDTMFString();
            break;	
        // Continue or Stop DTMF String Sending
        case EMobilePhoneContinueDTMFStringSending:
            cont = * ( reinterpret_cast<TBool*>( dataPtr ) );
            ret = ContinueDTMFStringSendingL( aTsyReqHandle, cont );
            break;
        case EMobilePhoneNotifyDTMFEvent:
            ret = NotifyDTMFEvent( reinterpret_cast<TInt*>( dataPtr ) );
	    	break;	
		// not supported DTMF features
		case EMobilePhoneReadDTMFTones:
            ret = KErrNotSupported;
            break;	
        // error case
        default:
            // this method should only be called for DTMF cases
            ret = KErrArgument;
            break;	
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_DOEXTFUNCL_2, "TSY: CMmDtmfTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::NotifyDtmfEvent
// Request for Notification of DTMF event received from client. 
// ---------------------------------------------------------------------------
TInt CMmDtmfTsy::NotifyDTMFEvent( TInt* aInfo )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_NOTIFYDTMFEVENT_1, "TSY: CMmDtmfTsy::NotifyDTMFEvent");

	iDtmfInfo = aInfo; // save pointer to client data

	// save handle type
	iMmPhone->iReqHandleType = CMmPhoneTsy::EMultimodePhoneNotifyDtmfEvent;
  
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CMmDtmfTsy::CompleteNotifyDtmfEvent
// Notifies client that a DTMF event has occurred. 
// ---------------------------------------------------------------------------
//
void CMmDtmfTsy::CompleteNotifyDTMFEvent( TInt aInfo )
     {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_COMPLETENOTIFYDTMFEVENT_1, "TSY: CMmCustomTsy::CompleteNotifyDtmfEvent");
    // reset request handle. Returns the deleted req handle.
	TTsyReqHandle reqHandle =	
		iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
    					CMmPhoneTsy::EMultimodePhoneNotifyDtmfEvent );

    // check if request was called
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        *iDtmfInfo = aInfo;
        iDtmfInfo=NULL;
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }
    }
	
// ---------------------------------------------------------------------------
// CMmDtmfTsy::NotifyDTMFEventCancel
// Cancels a notification request. 
// ---------------------------------------------------------------------------
TInt CMmDtmfTsy::NotifyDTMFEventCancel( const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_NOTIFYDTMFEVENTCANCEL_1, "TSY: CMmDtmfTsy::NotifyDtmfEventCancel");

	iDtmfInfo = NULL;
	TTsyReqHandle reqHandle =	
	iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
					CMmPhoneTsy::EMultimodePhoneNotifyDtmfEvent );
	iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );

	return KErrNone;
    }
	
	
// ---------------------------------------------------------------------------
// CMmDtmfTsy::CancelService
// Cancels DTMF requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::CancelService(   
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle ) 
    {
    TInt ret ( KErrNotSupported );
    TInt trapError = KErrNone;
    switch ( aIpc )
        {
        case EMobilePhoneNotifyDTMFCapsChange:
            ret = NotifyDTMFCapsChangeCancel( aTsyReqHandle );
            break;	
        case EMobilePhoneNotifyStopInDTMFString:
            ret = NotifyStopInDTMFStringCancel( aTsyReqHandle );
            break;	
        case EMobilePhoneSendDTMFTones:
            // Note: it is silently ignored if SendDTMFTonesCancelL
            // leaves here (it's just a cancellation)
            TRAP( trapError, 
                ret = SendDTMFTonesCancelL();
                );
            if ( KErrNone != trapError )
                {
                return trapError;
                }
            break;
		case EMobilePhoneNotifyDTMFEvent:
			ret = NotifyDTMFEventCancel(aTsyReqHandle);
			break;
        // error case
        default:
            // this method should only be called for DTMF cases
            ret = KErrArgument;
            break;	
        }

    return ret;
    }
#ifndef USING_CTSY_DISPATCHER
// ---------------------------------------------------------------------------
// CMmDtmfTsy::Complete
// Completes the request due timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDtmfTsy::Complete(
    TInt aReqHandleType, 
    TInt ) 
    {
    switch( aReqHandleType )
        {
        // there is only one DTMF case with a defined timeout
        case CMmPhoneTsy::EMultimodePhoneSendDTMFTones:
            CompleteSendDTMFTones( KErrTimedOut );
            break;	
        default:
            // ignore
            break;
        }
    }
#endif // USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmDtmfTsy::GetDtmfType
// Returns value of internal attribute iDtmfType.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmDtmfTsy::TDtmfType CMmDtmfTsy::GetDtmfType()
    {
    return iDtmfType;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::GetDTMFCaps
// This function returns the DTMF capabilities of the phone in 
// the aCaps parameter. The DTMF capabilities can change 
// dynamically for example when the call becomes active or is 
// closed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::GetDTMFCaps(
    const TTsyReqHandle aTsyReqHandle, // Tsy request handle
    TUint32* aCaps ) // on return, contains the capabilities 
    {
    //Set initial value for capabilities
    *aCaps = 0;
    //Get the number of active Calls in list of calls
    TInt count = ActiveCalls();
    //Search the Calls in call list and check if at least one call is active
    if(count > 0)
        {
        *aCaps = RMobilePhone::KCapsSendDTMFString |
            RMobilePhone::KCapsSendDTMFSingleTone;
        }

    // Complete the request
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::NotifyDTMFCapsChange
// This function is used to request notifications about changes 
// in DTMF capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::NotifyDTMFCapsChange(
    TUint32* aCaps ) 
    {
    iRetDtmfCaps = aCaps;
    iMmPhone->iReqHandleType = CMmPhoneTsy::EMultimodePhoneNotifyDTMFCaps;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::NotifyDTMFCapsChangeCancel
// Cancels DTMF capabilities notification request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::NotifyDTMFCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    iRetDtmfCaps = NULL;
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyDTMFCaps );
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::CompleteNotifyDTMFCaps
// Notifies client about changes in DTMF capabilities if the 
// client has requested DTMF change notifications.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDtmfTsy::CompleteNotifyDTMFCaps()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle 	
        = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyDTMFCaps );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {        
        GetDTMFCaps( reqHandle, iRetDtmfCaps );
        iRetDtmfCaps = NULL;
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }

    }
#ifndef USING_CTSY_DISPATCHER
// ---------------------------------------------------------------------------
// CMmDtmfTsy::SendDTMFTonesL
// Sends burst DTMF tones. The DTMF tones to be sent are given
// in the parameter and the DTMF string can contain one or more DTMF
// characters.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::SendDTMFTonesL(   
    const TTsyReqHandle aTsyReqHandle, // Request handle
    const TDesC* aTones ) // DTMF tone string to be sent
    {
    if( aTones->Length() == 0 )
        {
        return KErrArgument;
        }
    if( aTones->Length() > KDtmfStringMaxLength)
        {
        return KErrArgument;
        }
    //check if handle is not in use already
    TTsyReqHandle sendDTMFTonesHandle = iMmPhone->iTsyReqHandleStore
        ->GetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneSendDTMFTones );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == sendDTMFTonesHandle )
        {
        // packed parameter for DOS call: tones
        // Check if there is an active call
        if(ActiveCalls() == 0)
            {
            return KErrEtelCallNotActive;
            }
        CMmDataPackage dataPackage;
        dataPackage.PackData( const_cast<TDesC*>( aTones ) );
        TInt ret = iMmPhone->MessageManager()->HandleRequestL(
            EMobilePhoneSendDTMFTones, 
            &dataPackage
            );
        // Save the type of the DTMF
        iDtmfType = EDtmfTypeString;

        if ( KErrNone != ret )
            {
            // DTMF string sending has failed
            iMmPhone->ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            iMmPhone->iReqHandleType =
                CMmPhoneTsy::EMultimodePhoneSendDTMFTones;
            }
        
        return KErrNone;
        }
    else
        {
        return KErrServerBusy;
        }

    }
#endif // #ifndef USING_CTSY_DISPATCHER
// ---------------------------------------------------------------------------
// CMmDtmfTsy::CompleteSendDTMFTones
// This function should be called when DTMF string has been sent to
// the network. The function completes SendDTMFTones request to the 
// client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDtmfTsy::CompleteSendDTMFTones( 
     TInt aResult ) // error code
    {
    // Get req handle. 
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSendDTMFTones );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iDTMFStringToContinue.Zero();
        iMmPhone->ReqCompleted( reqHandle, aResult );
        }

    }

#ifndef USING_CTSY_DISPATCHER
// ---------------------------------------------------------------------------
// CMmDtmfTsy::SendDTMFTonesCancelL
// Cancels DTMF tone sending.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::SendDTMFTonesCancelL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_SENDDTMFTONESCANCELL_1, "TSY: CMmDtmfTsy::SendDTMFTonesCancel");

    TTsyReqHandle sendDTMFTonesHandle =
        iMmPhone->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSendDTMFTones );
    
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != sendDTMFTonesHandle )
        {
        // no packed parameter for DOS call
        CMmDataPackage dummyPackage;
        TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneSendDTMFTonesCancel, 
            &dummyPackage
            );
        if ( KErrNone == ret )
            {
            // Set the DTMFTonesCancelFlag
            iSendDTMFTonesCancelFlag = ETrue;
            }
        }
    
    return KErrNone;
    }
#endif // USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmDtmfTsy::CompleteSendDTMFTonesCancel
// Complete cancel of sending DTMF tones.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDtmfTsy::CompleteSendDTMFTonesCancel( 
    TInt aResult ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_COMPLETESENDDTMFTONESCANCEL_1, "TSY: CMmDtmfTsy::CompleteSendDTMFTonesCancel");

    // Get req handle. 
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->GetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSendDTMFTones );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // If cancel has not succeeded, just discard the message.
        if ( iSendDTMFTonesCancelFlag )
            {
            // Cancel has succeeded. Complete original request and 
            // reset handle
            iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneSendDTMFTones ); 

            iMmPhone->ReqCompleted( reqHandle, KErrCancel );
           	}
        else
            {
            CompleteSendDTMFTones( aResult );
            }
        }
    }
#ifndef USING_CTSY_DISPATCHER
// ---------------------------------------------------------------------------
// CMmDtmfTsy::StartDTMFToneL
// This function starts a single DTMF tone sending. The tone 
// is played until it is stopped using StopDTMFTone method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::StartDTMFToneL(
    const TTsyReqHandle aTsyReqHandle,
    TChar* aTone ) 
    {
    TInt ret = KErrServerBusy;

    // Check if request handle already exists
    TTsyReqHandle startDtmftoneHandle = 
        iMmPhone->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneStartDTMFTone );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != startDtmftoneHandle )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {

        ret = KErrUnknown;
        // packed parameter for DOS call: tone
        CMmDataPackage dataPackage;
        dataPackage.PackData( aTone );
        ret = iMmPhone->MessageManager()->HandleRequestL(
            EMobilePhoneStartDTMFTone, 
            &dataPackage
            );

        if ( KErrNone == ret )
            {
            // Save the type of the DTMF
            iDtmfType = EDtmfTypeDigit;
            }

        iMmPhone->ReqCompleted( aTsyReqHandle, ret );

        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::StopDTMFToneL
// Stops playing of a single DTMF tone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::StopDTMFToneL(    
    const TTsyReqHandle aTsyReqHandle ) 
    {
    TInt ret = KErrServerBusy;

    // Check if request handle already exists
    TTsyReqHandle stopDtmftoneHandle = 
        iMmPhone->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneStopDTMFTone );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != stopDtmftoneHandle )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // no packed parameter for DOS call
        CMmDataPackage dummyPackage;
        ret = iMmPhone->MessageManager()->HandleRequestL(
            EMobilePhoneStopDTMFTone, 
            &dummyPackage
            );

        if ( KErrNone == ret )
            {
            // Save the type of the DTMF
            iDtmfType = EDtmfTypeDigit;
            }

        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        }

    return KErrNone;
    }
#endif //USING_CTSY_DISPATCHER
// ---------------------------------------------------------------------------
// CMmDtmfTsy::NotifyStopInDTMFString
// Notifies the client when the phone has encountered a "w" char
// during the sending of a DTMF string.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::NotifyStopInDTMFString()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_NOTIFYSTOPINDTMFSTRING_1, "TSY: CMmDtmfTsy::NotifyStopInDTMFString");
    //Set TSY request handle type
    iMmPhone->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyStopInDTMFString;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::NotifyStopInDTMFStringCancel
// Cancel NotifyStopInDTMFString request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::NotifyStopInDTMFStringCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyStopInDTMFString );

    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::CompleteNotifyStopInDTMFString
// Complete notify 'w' in DTMF string. This means that DTMF
// string sending is stopped until client calls ContinueDTMFStringSendingL.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDtmfTsy::CompleteNotifyStopInDTMFString( 
	CMmDataPackage* aDataPackage )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_COMPLETENOTIFYSTOPINDTMFSTRING_1, "TSY: CMmDtmfTsy::CompleteNotifyStopInDTMFString");
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneNotifyStopInDTMFString );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iDTMFStringToContinue.Zero();    
        //Unpack data
        aDataPackage->UnPackData( iDTMFStringToContinue );        

        // Complete the client request
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmDtmfTsy::ContinueDTMFStringSendingL
// Continues or cancels the stopped DTMF string sending.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::ContinueDTMFStringSendingL(
     const TTsyReqHandle aTsyReqHandle, 
     TBool aContinue )                  
                                           
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_CONTINUEDTMFSTRINGSENDINGL_1, "TSY: CMmDtmfTsy::ContinueDTMFStringSendingL aContinue:%d",aContinue);
    TInt ret( KErrNone );

    TTsyReqHandle sendDTMFTonesHandle = 
        iMmPhone->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSendDTMFTones );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != sendDTMFTonesHandle )
        {
        if (  0 < iDTMFStringToContinue.Length() )
            {
            // Continue or cancel sending depending on the boolean value
            if ( aContinue )
                {

                // Continue DTMF string sending
                CMmDataPackage dataPackage;
                // packed parameter for DOS call: tones to continue
                dataPackage.PackData( &iDTMFStringToContinue );
                ret = iMmPhone->MessageManager()->HandleRequestL(
                    EMobilePhoneContinueDTMFStringSending, 
                    &dataPackage
                    );
                }
            else // Cancel sending
                {
                ret = KErrAbort;
                }
            }
        else if ( 0 == iDTMFStringToContinue.Length() )
            {
            // Last character was a 'w' so do the cleanup and complete requ
            CompleteSendDTMFTones( KErrNone );
            }
        else
            {
            // iDTMFStringToContinue is NULL 
            ret = KErrGeneral;
            }
        }
    else
        {
        // DTMF sending not going on. Complete request
        ret = KErrNotReady;
        }

    //Complete ContinueDTMFStringSending request
    if ( KErrNone == ret || KErrAbort == ret )
        {
        // Complete continue request with KErrNone
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrNone );
        }
    else
        {
        // Complete continue request with error value
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        }  

    //Error occurred or DTMF sending aborted
    if ( KErrNone != ret )
        {
        iDTMFStringToContinue.Zero();
   
        // Complete SendDTMFTones request if user cancelled DTMF sending 
        // or DTMF string sending failed,         
        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown 
            != sendDTMFTonesHandle )
            {
            // Reset SendDTMFTones request handle
            iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneSendDTMFTones );

            // Client has cancelled DTMF sending or sending failed. 
            iMmPhone->ReqCompleted( sendDTMFTonesHandle, ret );
            }
        }

    return KErrNone;
    }


TInt CMmDtmfTsy::ActiveCalls() const
    {
    TInt ret = 0;
    CMmCallList* list = iMmPhone->CallList(); 
    //Get the number of Calls in list of calls
    TInt count = list->GetNumberOfObjects();
    for ( TInt i = 0; i < count; i++ )
        {
        CMmCallTsy* mmCall = list->GetMmCallByIndex( i );
        if ( mmCall )
            {
            // get call status
            RCall::TStatus  callStatus = mmCall->Status();
            // Check if the call state is one that suggests that someone will listen to a DTMF. 
            if (( RCall::EStatusConnected == callStatus ) || ( RCall::EStatusDialling == callStatus )|| ( RCall::EStatusConnecting == callStatus ))
                {
                // get service type of call
                RMobileCall::TMobileCallInfoV1 info;
                RMobileCall::TMobileCallInfoV1Pckg infoPckg(info);
                mmCall->FillMobileCallInfo(&infoPckg);
                RMobilePhone::TMobileService callService = info.iService;
                //If the call is a voice call
                if ( (RMobilePhone::EVoiceService == callService) || (RMobilePhone::EAuxVoiceService ==  callService)) 
                    {
                    ++ret;
                    }
                }
            }
        }
    return ret;

    }
//  End of File  
