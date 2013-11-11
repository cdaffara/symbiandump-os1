// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "trequestorder.h"

CRequestOrder::CRequestOrder(TInt aNumberAsynchronousRequests) :
    iNumberAsynchronousRequests(aNumberAsynchronousRequests)
    {
    }

CRequestOrder::~CRequestOrder()
    {
    delete iOrderArray;
    iLock.Close();
    }
    
void CRequestOrder::ConstructL()
    {
    User::LeaveIfError(iLock.CreateLocal());
    
    iOrderArray = new (ELeave) TOrderArray[iNumberAsynchronousRequests];
    User::LeaveIfNull(iOrderArray);
    
    ResetAll();
    }

CRequestOrder* CRequestOrder::NewL(const TInt aNumberAsynchronousRequests)
    {
    CRequestOrder* requestOrder = new (ELeave) CRequestOrder(aNumberAsynchronousRequests);
    CleanupStack::PushL(requestOrder);
    requestOrder->ConstructL();
    CleanupStack::Pop();
    return requestOrder;
    }


void CRequestOrder::ResetAll() 
    {
    iIndex=0;
    iOrderIndexBefore=0;
    iOrderIndexComposition=0;
    iOrderIndexAfter=0;
    
    for(TInt i=0; i<iNumberAsynchronousRequests;i++)
            {
            iOrderArray[i].iType = EOrderErr;
            iOrderArray[i].iNumber = 0;
            }
    }

TInt CRequestOrder::GetIndex() 
    {
    return iIndex;
    }

void CRequestOrder::SetOrder(TOrderType aOrderType) 
    {
    iLock.Wait();
    iOrderArray[iIndex].iType = aOrderType;
    switch (aOrderType)
        {
        case EOrderBefore:
            {
            iOrderArray[iIndex].iNumber = iOrderIndexBefore++;
            break;
            }
        case EOrderComposition:
            {
            iOrderArray[iIndex].iNumber = iOrderIndexComposition++;
            break;
            }
        case EOrderAfter:
            {
            iOrderArray[iIndex].iNumber = iOrderIndexAfter++;
            break;
            }
        default:
            {
            _LIT(KMsg, "GRAPHICS-SURFACEUPDATE-0003");
            User::Panic(KMsg, 1);
            break;
            }
        }
    iIndex++;
    iLock.Signal();
    }

TOrderType CRequestOrder::GetOrderType(TInt aIndex) 
    { 
    return iOrderArray[aIndex].iType;
    }

TInt CRequestOrder::GetOrderNumber(TInt aIndex)
    {
    return iOrderArray[aIndex].iNumber;
    }
