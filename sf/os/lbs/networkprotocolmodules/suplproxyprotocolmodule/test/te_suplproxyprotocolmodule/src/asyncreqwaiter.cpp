// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file asyncreqwaiter.cpp
*/

#include "asyncreqwaiter.h"
#include "epos_comasuplinforequestlist.h"

#include "epos_eomasuplposerrors.h"
#include "epos_comasuplposition.h"
#include "epos_comasuplreqasstdata.h"
#include "epos_comasuplpospayload.h"
#include "epos_tomasuplallowedcapabilities.h"
#include "epos_comasuplsetcapabilities.h"
#include "epos_comasuplvelocity.h"

CAsyncReqWaiter::CAsyncReqWaiter(COMASuplPosSessionBase* aSessionPtr) :
            CActive(EPriorityStandard)
    {
    iPosSessionPtr=aSessionPtr;
    CActiveScheduler::Add(this);
    }

CAsyncReqWaiter* CAsyncReqWaiter::NewL(COMASuplPosSessionBase* aSessionPtr)
    {
    CAsyncReqWaiter* asyncWaiter = new (ELeave) CAsyncReqWaiter(aSessionPtr);
    CleanupStack::PushL(asyncWaiter);
    asyncWaiter->ConstructL();
    CleanupStack::Pop(asyncWaiter);
    return asyncWaiter;
    }

void CAsyncReqWaiter::ConstructL()
    {
    
    }

CAsyncReqWaiter::~CAsyncReqWaiter()
    {
    Cancel();
    if (iSuplInfoRequest)
        {
        delete iSuplInfoRequest;
        }
    if (iSuplPosition)
        {
        delete iSuplPosition;
        }
    }

void CAsyncReqWaiter::StartTest(TestCase aTestCaseId)
    {
    iTestCaseId = aTestCaseId;
    iResponseExp = EFalse;
    iStatus = KRequestPending;
    TRequestStatus* stat = &iStatus;
    User::RequestComplete(stat,KErrNone);
    SetActive();
    }

void CAsyncReqWaiter::RunL()
    {
    if (iResponseExp)
        {
        //Response received from the SPPM 
        //Stop the test (by stopping the scheduler)
        CActiveScheduler::Stop();
        }
    else
        {
        //Request to be sent to the SPPM.
        iResponseExp = ETrue;
        switch (iTestCaseId)
            {
            case ECancelGetSuplInfo:
                {
                iSuplInfoRequest = COMASuplInfoRequestList::NewL();
                
                //Note: ownership of these objects held in iSuplInfoRequest
                COMASuplSETCapabilities* setCaps = COMASuplSETCapabilities::NewL();
                setCaps->SetType(COMASuplInfoRequest::EOMASuplSETCapabilities);
                iSuplInfoRequest->Append(setCaps);

                COMASuplReqAsstData* assisData = COMASuplReqAsstData::NewL();
                assisData->SetReqAsstData(EFalse, EFalse, EFalse, EFalse, EFalse,
                        ETrue/*aRefTime*/, EFalse, EFalse);
                iSuplInfoRequest->Append(assisData);

                //a delay is added while requesting this reference position from the
                //test refloc plugin so that the GetSuplInfoL() req will be oustanding when the cancel
                //reaches the SPPM.
                COMASuplPosition* pos = COMASuplPosition::NewL();
                pos->SetType(COMASuplInfoRequest::EOMASuplPosition);
                iSuplInfoRequest->Append(pos);
                
                COMASuplVelocity* velocity = COMASuplVelocity::NewL();
                velocity->SetType(COMASuplInfoRequest::EOMASuplVelocity);
                iSuplInfoRequest->Append(velocity);
                
                //Send GetSuplInfoL and then cancel it immediately
                __ASSERT_DEBUG(iPosSessionPtr,User::Invariant());
                iPosSessionPtr->GetSuplInfoL(iSuplInfoRequest,iStatus);
                SetActive();
                
                iPosSessionPtr->CancelSuplInfoRequest();

                break;
                }
            case EGetPosition:
                {
                iSuplPosition = COMASuplPosition::NewL();

                //Send the GetPositionL() request
                __ASSERT_DEBUG(iPosSessionPtr,User::Invariant());
                iPosSessionPtr->GetPositionL(iStatus, iSuplPosition);
                SetActive();
                
                break;
                }
            case EGetSuplInfo:
                {
                iSuplInfoRequest = COMASuplInfoRequestList::NewL();

                //Note: ownership of these objects held in iSuplInfoRequest
                COMASuplSETCapabilities* setCaps = COMASuplSETCapabilities::NewL();
                setCaps->SetType(COMASuplInfoRequest::EOMASuplSETCapabilities);
                iSuplInfoRequest->Append(setCaps);

                COMASuplReqAsstData* assisData = COMASuplReqAsstData::NewL();
                assisData->SetReqAsstData(EFalse, EFalse, EFalse, EFalse, EFalse,
                        ETrue/*aRefTime*/, EFalse, EFalse);
                iSuplInfoRequest->Append(assisData);

                //a delay is added while requesting this reference position from the
                //test refloc plugin 
                COMASuplPosition* pos = COMASuplPosition::NewL();
                pos->SetType(COMASuplInfoRequest::EOMASuplPosition);
                iSuplInfoRequest->Append(pos);
                
                COMASuplVelocity* velocity = COMASuplVelocity::NewL();
                velocity->SetType(COMASuplInfoRequest::EOMASuplVelocity);
                iSuplInfoRequest->Append(velocity);
                
                //Send GetSuplInfoL() req to the SPPM  
                __ASSERT_DEBUG(iPosSessionPtr,User::Invariant());
                iPosSessionPtr->GetSuplInfoL(iSuplInfoRequest,iStatus);
                SetActive();
                
                break;
                }
            case ECancelGetPosition:
                {
                iSuplPosition = COMASuplPosition::NewL();
                //Send GetPositionL() request and cancel it immediately
                __ASSERT_DEBUG(iPosSessionPtr,User::Invariant());
                iPosSessionPtr->GetPositionL(iStatus, iSuplPosition);
                SetActive();
                
                iPosSessionPtr->CancelGetPosition();
                break;
                }
            case EGetSuplInfoSessionEnd:
                {
                if (iSuplInfoRequest)
                    {
                    delete iSuplInfoRequest;
                    iSuplInfoRequest = NULL;
                    }
                iSuplInfoRequest = COMASuplInfoRequestList::NewL();

                //Note: ownership of these objects held in iSuplInfoRequest
                COMASuplSETCapabilities* setCaps = COMASuplSETCapabilities::NewL();
                setCaps->SetType(COMASuplInfoRequest::EOMASuplSETCapabilities);
                iSuplInfoRequest->Append(setCaps);

                COMASuplReqAsstData* assisData = COMASuplReqAsstData::NewL();
                assisData->SetReqAsstData(EFalse, EFalse, EFalse, EFalse, EFalse,
                        ETrue/*aRefTime*/, EFalse, EFalse);
                iSuplInfoRequest->Append(assisData);

                //a delay is added while requesting this reference position from the
                //test refloc plugin so that the GetSuplInfoL() req will be oustanding when SessionEnd
                //reaches the SPPM.
                COMASuplPosition* pos = COMASuplPosition::NewL();
                pos->SetType(COMASuplInfoRequest::EOMASuplPosition);
                iSuplInfoRequest->Append(pos);
                
                COMASuplVelocity* velocity = COMASuplVelocity::NewL();
                velocity->SetType(COMASuplInfoRequest::EOMASuplVelocity);
                iSuplInfoRequest->Append(velocity);
                
                //Send GetSuplInfoL and then send SessionEnd immediately
                __ASSERT_DEBUG(iPosSessionPtr,User::Invariant());
                iPosSessionPtr->GetSuplInfoL(iSuplInfoRequest,iStatus);
                SetActive();
                iPosSessionPtr->SessionEnd();

                break;
                }
            case ESuplInfoPosProtocolVersion:
                {
                if (iSuplInfoRequest)
                    {
                    delete iSuplInfoRequest;
                    iSuplInfoRequest = NULL;
                    }
                //Create a new GetSuplInfoL() request
                iSuplInfoRequest = COMASuplInfoRequestList::NewL();
                iSETCapabilities = COMASuplSETCapabilities::NewL();
                iSETCapabilities->SetType(COMASuplInfoRequest::EOMASuplSETCapabilities);
                iSuplInfoRequest->Append(iSETCapabilities);//iSuplInfoRequest takes ownership
                iPosSessionPtr->GetSuplInfoL(iSuplInfoRequest,iStatus);
                SetActive();
                break;
                }
            case ERefLocPluginPositionError:
                {
                iSuplInfoRequest = COMASuplInfoRequestList::NewL();

                //Note: ownership of these objects held in iSuplInfoRequest
                COMASuplSETCapabilities* setCaps = COMASuplSETCapabilities::NewL();
                setCaps->SetType(COMASuplInfoRequest::EOMASuplSETCapabilities);
                iSuplInfoRequest->Append(setCaps);

                COMASuplReqAsstData* assisData = COMASuplReqAsstData::NewL();
                assisData->SetReqAsstData(EFalse, EFalse, EFalse, EFalse, EFalse,
                        ETrue/*aRefTime*/, EFalse, EFalse);
                iSuplInfoRequest->Append(assisData);

                //a delay is added while requesting this reference position from the
                //test refloc plugin
                COMASuplPosition* pos = COMASuplPosition::NewL();
                pos->SetType(COMASuplInfoRequest::EOMASuplPosition);
                iSuplInfoRequest->Append(pos);
                
                COMASuplVelocity* velocity = COMASuplVelocity::NewL();
                velocity->SetType(COMASuplInfoRequest::EOMASuplVelocity);
                iSuplInfoRequest->Append(velocity);
                
                //Send GetSuplInfoL() req to the SPPM  
                __ASSERT_DEBUG(iPosSessionPtr,User::Invariant());
                iPosSessionPtr->GetSuplInfoL(iSuplInfoRequest,iStatus);
                SetActive();
                break;
                }
            default:
                {
				iStatus = KErrArgument;
                CActiveScheduler::Stop();
                }
            }
        }
    }


void CAsyncReqWaiter::DoCancel()
    {

    }

TInt CAsyncReqWaiter::RunError(TInt aError)
    {
    return aError;
    }
