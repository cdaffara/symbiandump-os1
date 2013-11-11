/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
#include <e32cmn.h>
#include "suplpsygenericinfouser.h"
#include "suplpsylogging.h"
#include "suplpsypanic.h"
#include "suplpsy.hrh"  
#include "lbsnetinternalapi.h"
#include "lbsnetinternalmsgtypes.h"
#include "lbsnrhngmsgs.h"
#include "LbsPsyNgMsgs.h"
#include "lbscommoninternaldatatypes.h"

#include <lbssuplpsyadaptation.h>

const TUid KSuplPsyUid = {KSuplPsyImplUid}; //KSuplPsyImplUid defined in "suplpsy.hrh"
// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::NewL
// -----------------------------------------------------------------------------
//
CLbsSuplPsyAdaptation * CLbsSuplPsyAdaptation::NewL(MLbsSuplPsyAdaptationObserver& aObserver)
    {
	CLbsSuplPsyAdaptation* self = new( ELeave ) CLbsSuplPsyAdaptation(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::~CLbsSuplPsyAdaptation
// -----------------------------------------------------------------------------
//
CLbsSuplPsyAdaptation::~CLbsSuplPsyAdaptation()
    {
    Cancel();
    iPsyChannel.Close();
    delete iPositionInfo;
    iPositionInfo = NULL;
    delete iSentMsg;
    iSentMsg = NULL;
    }

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::CLbsSuplPsyAdaptation 
// -----------------------------------------------------------------------------
//
CLbsSuplPsyAdaptation::CLbsSuplPsyAdaptation (MLbsSuplPsyAdaptationObserver& aObserver):
        CActive( EPriorityStandard ),
        iObserver( aObserver ),
        iLocRequest( EFalse)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::ConstructL
// -----------------------------------------------------------------------------
//
void CLbsSuplPsyAdaptation::ConstructL()
    {
    iPsyChannel.OpenL(RLbsNetChannel::EChannelSUPLPSY2NG, *this);
    }

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::RequestLocationUpdate
// -----------------------------------------------------------------------------
//
void CLbsSuplPsyAdaptation::RequestLocationUpdate(TUint aRequestId, TBool aNewClient, TUid aProtocolModule)
    {
    TRACESTRING( "CLbsSuplPsyAdaptation::RequestLocation start" )
    //Check if location request has been made already
    if ( IsActive() )
        {
        return;
        }
        
    iRequestid = aRequestId;

    TRACESTRING( "Creating request message... " )
                
    delete iSentMsg;
    iSentMsg = NULL;
    TRAPD(err,iSentMsg = CreateTLbsCellLocationRequestMsgL(aRequestId, aNewClient, aProtocolModule); )
    if(err != KErrNone)
    	{
    	iLocRequest = EFalse;
    	iStatus = KRequestPending;
      TRACESTRING( "CLbsSuplPsyAdaptation::RequestLocation CreateTLbsCellLocationRequestMsgL failed" )
      SetActive();
      TRequestStatus* status = &iStatus;
      User::RequestComplete( status, err );
      return;
      }
    iLocRequest = ETrue;
    iPsyChannel.SendMessage(*iSentMsg, iStatus);
    SetActive();
    TRACESTRING( "CLbsSuplPsyAdaptation::RequestLocation end" )        
    }
// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::RunL
// -----------------------------------------------------------------------------
//
void CLbsSuplPsyAdaptation::RunL()
    {
    TRACESTRING2( "CLbsSuplPsyAdaptation::RunL status=%d", iStatus.Int() )
    //Call OnSessionComplete only if it not location request.
    //Location request gets completed in ProcessNetChannelMessage() method            
    if (!iLocRequest)
        iObserver.OnSessionComplete(iRequestid, iStatus.Int());	
    TRACESTRING( "CLbsSuplPsyAdaptation::RunL end" )
}
	
// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::CancelLocationRequest
// -----------------------------------------------------------------------------
//
void CLbsSuplPsyAdaptation::CancelLocationRequest(TUint aRequestId, TInt aReason)
    {
    if (iLocRequest)
        {            
        if ( IsActive() )
            iPsyChannel.CancelSendMessageNotification();                
        delete iSentMsg;
        iSentMsg = NULL;
        iSentMsg = new  TLbsCellLocationCancelMsg(TLbsNetSessionIdInt(KSuplPsyUid, aRequestId), aReason);
        TRequestStatus status;
        iPsyChannel.SendMessage(*iSentMsg, status);
        iLocRequest = EFalse;
        //SetActive();
        User::WaitForRequest(status);
        }
    }

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::DoCancel
// -----------------------------------------------------------------------------
//
void CLbsSuplPsyAdaptation::DoCancel()
    {
    iPsyChannel.CancelSendMessageNotification(); 
    }

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::RunError
// -----------------------------------------------------------------------------
//
TInt CLbsSuplPsyAdaptation::RunError( TInt /* aError */ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::CreateTLbsCellLocationRequestMsgL
// -----------------------------------------------------------------------------
//
TLbsNetInternalMsgBase* CLbsSuplPsyAdaptation::CreateTLbsCellLocationRequestMsgL(TUint aRequestId, TBool aNewClient, TUid aProtocolModule)
    {
	TLbsNetPosRequestOptionsInt options;
	options.SetNewClientConnected(aNewClient);
	
	TLbsCellLocationRequestMsg* locReqMsg = new (ELeave) TLbsCellLocationRequestMsg(TLbsNetSessionIdInt(KSuplPsyUid, aRequestId), aProtocolModule, options);
	return locReqMsg;
    }        

// -----------------------------------------------------------------------------
// CLbsSuplPsyAdaptation::ProcessNetChannelMessage
// -----------------------------------------------------------------------------
//
void CLbsSuplPsyAdaptation::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
    {
    if (RLbsNetChannel::EChannelSUPLPSY2NG != aChannelId)
        {            
        iObserver.OnSessionComplete(iRequestid, KErrGeneral); 
        }
    else
        {            
    	switch (aMessage.Type())
		    {
            case TLbsNetInternalMsgBase::ECellLocationResponse:    
                {                    
                const TLbsCellLocationResponseMsg& msg = static_cast<const TLbsCellLocationResponseMsg&>(aMessage);
                TInt reason = msg.Reason();
                if (reason >= KErrNone)            
                    {
                    TRACESTRING( "Extracting position ... " )
                    const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(msg.PositionInfo());
                    TRAPD(err,iPositionInfo = HPositionGenericInfo::NewL(posInfo.PositionClassSize()); )
                    if(err != KErrNone)
                    	{
                    	iObserver.OnSessionComplete(iRequestid, err); 
                    	return;
                    	}
                    TPosition pos;
                    posInfo.GetPosition( pos );
                    //Mem::Copy(&iPositionInfo, &posInfo, posInfo.PositionClassSize());
					(static_cast<TPositionInfo*>(iPositionInfo))->SetPosition(pos);
                    //iPositionInfo->SetPosition(pos);
                    }            
                iObserver.OnLocationUpdate(iRequestid,*iPositionInfo,reason);
                break;
                }
            default: 
                {
                TRACESTRING( "Invalid message received " )
                iObserver.OnSessionComplete(iRequestid, KErrGeneral); 
                break;
                }
            }
        }
    }
