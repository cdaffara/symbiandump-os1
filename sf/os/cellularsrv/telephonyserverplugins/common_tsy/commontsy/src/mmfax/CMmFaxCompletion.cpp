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



// INCLUDE FILES
#include "cmmfaxcompletion.h"
#include "cmmcalltsy.h"

// ======== MEMBER FUNCTIONS ========

CMmFaxCompletion::CMmFaxCompletion() : CBase(), iTsyReqHandle ( 0 ),
	iTelObject( NULL )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_CTOR_1, "TSY: CMmFaxCompletion::CMmFaxCompletion" );
    }

CMmFaxCompletion::~CMmFaxCompletion()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_DTOR_1, "TSY: CMmFaxCompletion::~CMmFaxCompletion");
    if ( iTsyReqHandle )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_DTOR_2, "TSY: CMmFaxCompletion::~CMmFaxCompletion: request %d not completed!", iTsyReqHandle);
        // Completion should not be done here, because in some cases
        // operations are completed automatically by ETel!
        // CompleteOperation(KErrCancel);
        }
    }

    
// ---------------------------------------------------------------------------
// CMmFaxCompletion::CompleteOperation
// Completes ongoing read/write operation.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::CompleteOperation(
        TInt aError )     // error Value         
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_COMPLETEOPERATION_1, "TSY: CMmFaxCompletion::CompleteOperation iTsyReqHandle: %d", iTsyReqHandle );
    if ( 0 < iTsyReqHandle )
        {
        iTelObject->ReqCompleted( iTsyReqHandle, aError );
        iTsyReqHandle = 0;
        }
    }
    
// ---------------------------------------------------------------------------
// CMmFaxCompletion::Configure
// Fax configuration. Set parameters for Fax server completion.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::Configure(
        const TTsyReqHandle aTsyReqHandle,  
        CTelObject* aTelObject )                  
    {
    if ( 0 < iTsyReqHandle )
        {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_CONFIGURE_1, "TSY: CMmFaxCompletion::Configure overwriting previous request %d with %d",iTsyReqHandle, aTsyReqHandle);
        }
    
    iTsyReqHandle = aTsyReqHandle; 
    iTelObject = aTelObject;
    }

// ---------------------------------------------------------------------------
// CMmFaxCompletion::GetCadenceAndTimeOfLastRing
// Returns a cadence and time of the last ring.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::GetCadenceAndTimeOfLastRing(
        TTimeIntervalMicroSeconds& /*aCadence*/,    // cadence time
        TTime& /*aTime*/ )                          // time     
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_GETCADENCEANDTIMEOFLASTRING_1, "TSY: CMmFaxCompletion::GetCadenceAndTimeOfLastRing");
    // Checking the usage of aCadence and aTime in the calling function
    // (fax server) shows that we don't need to change the values.
    // REINTERPRET_CAST(CTsyPhone*,iMmCall->Owner()->Owner())->
    // GetCadenceAndTimeOfLastRing(aCadence,aTime);
    }

// ---------------------------------------------------------------------------
// CMmFaxCompletion::RxConnectComplete
// Informs that receiving connection is complete.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::RxConnectComplete(
        TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_RXCONNECTCOMPLETE_1, "TSY: CMmFaxCompletion::RXConnectComplete ErrorValue: %d", aError);
    CMmCallTsy* mmCall = REINTERPRET_CAST(CMmCallTsy*, iTelObject);

    // This is a good place to fill missing remote party phone number.
    if ( KErrNone== aError )
        {
        FillAnswerback( mmCall );
        }

    if ( 0 < iTsyReqHandle )
        {


        if ( RMobileCall::EMobileTerminated== mmCall->CallDirection()  )
            {
            // answered to incoming call
            mmCall->CompleteAnswerIncomingCall( aError );
            }
        else
            {
            // dialled to some fax machine to get a fax..
            mmCall->CompleteDial( aError);
            }
        iTsyReqHandle = 0;
        }
    }
    
// ---------------------------------------------------------------------------
// CMmFaxCompletion::RxFaxDataComplete
// Informs that receiving data is complete.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::RxFaxDataComplete(
        TInt aError ) // in: an error value
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_RXFAXDATACOMPLETE_1, "TSY: CMmFaxCompletion::RxFaxDataComplete ErrorValue: %d", aError);
    if ( 0 < iTsyReqHandle )
        {
        iTelObject->ReqCompleted( iTsyReqHandle, aError );
        iTsyReqHandle = 0;
        }
    }

// ---------------------------------------------------------------------------
// CMmFaxCompletion::RxPostPageComplete
// Informs that Post Page is complete.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::RxPostPageComplete(
        TInt aError )
    {

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_RXPOSTPAGECOMPLETE_1, "TSY: CMmFaxCompletion::RxPostPageComplete ErrorValue: %d", aError );
    if ( 0 < iTsyReqHandle)
        {
        iTelObject->ReqCompleted( iTsyReqHandle, aError );
        iTsyReqHandle = 0;
        }
    }

// ---------------------------------------------------------------------------
// CMmFaxCompletion::RxPostPageComplete
// Fax transmit connection is completed.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::TxConnectComplete(
        TInt aError )	
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_TXCONNECTCOMPLETE_1, "TSY: CMmFaxCompletion::TXConnectComplete ErrorValue: %d", aError);

    CMmCallTsy* mmCall = REINTERPRET_CAST(CMmCallTsy*,iTelObject);

    // this is a good place to fill missing caller id
    if ( KErrNone == aError )
        {
        FillAnswerback( mmCall );
        }

    if ( 0 < iTsyReqHandle )
        {
        mmCall->CompleteDial( aError );
        iTsyReqHandle = 0;
        }
    }

// ---------------------------------------------------------------------------
// CMmFaxCompletion::TxFaxDataComplete
// Transmission fax data is complete.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::TxFaxDataComplete(
        TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_TXFAXDATACOMPLETE_1, "TSY: CMmFaxCompletion::TxFaxDataComplete ErrorValue: %d", aError);

    if ( 0 < iTsyReqHandle )
        {
        iTelObject->ReqCompleted( iTsyReqHandle, aError );
        iTsyReqHandle = 0;
        }
    }

// ---------------------------------------------------------------------------
// CMmFaxCompletion::TxPostPageComplete
// Transmission post page is complete.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::TxPostPageComplete(
        TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_TXPOSTPAGECOMPLETE_1, "TSY: CMmFaxCompletion::TxPostPageComplete ErrorValue: %d", aError );

    if ( 0 < iTsyReqHandle )
        {
        iTelObject->ReqCompleted( iTsyReqHandle, aError );
        iTsyReqHandle = 0;
        }
    }

// ---------------------------------------------------------------------------
// CMmFaxCompletion::FillAnswerback
// Checks progress information chunk and fills iAnswerBack field.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxCompletion::FillAnswerback(
        CMmCallTsy* aMmCall ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_FILLANSWERBACK_1, "TSY: CMmFaxCompletion::FillAnswerback");

    if ( aMmCall )
        {
        // if progress chunk already exists, this will return pointer to existing chunk.
        RFax::TProgress* faxProgress = aMmCall->CreateFaxProgressChunk();
        if ( faxProgress )
            {

            if ( 0 == faxProgress->iAnswerback.Length() )
                { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXCOMPLETION_FILLANSWERBACK_2, "TSY: CMmFaxCompletion::FillAnswerback Getting missing remote number from call info");
                // Copy remote party phone number to Progress chunk's 
                // answerback field. Answerback will hold only leftmost 20 
                // numbers if the number is longer than that.
                RMobileCall::TMobileCallInfoV1 info;
                RMobileCall::TMobileCallInfoV1Pckg infoPkg(info);
                aMmCall->FillMobileCallInfo(&infoPkg);
                faxProgress->iAnswerback.Copy(
                  info.iRemoteParty.iRemoteNumber.iTelNumber.Left( 20 ) );
                }
            }
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

//  End of File 
