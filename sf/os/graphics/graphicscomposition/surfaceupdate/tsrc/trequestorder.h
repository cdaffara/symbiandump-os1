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

#ifndef __TREQUESTORDER_H__
#define __TREQUESTORDER_H__

#include <e32base.h>

enum TOrderType
    {
    EOrderErr = 0,
    EOrderBefore,
    EOrderComposition,
    EOrderAfter,
    };

NONSHARABLE_CLASS(CRequestOrder): public CBase
    {
public:
    static CRequestOrder* NewL(TInt aNumberAsynchronousRequests);
    ~CRequestOrder();
    
    void ResetAll();
    TInt GetIndex();
    
    void SetOrder(TOrderType aOrderType) ;
    TOrderType GetOrderType(TInt aIndex);
    TInt GetOrderNumber(TInt aIndex);
    
private:
    void ConstructL();
    CRequestOrder(const TInt aNumberAsynchronousRequests);

    struct TOrderArray
        {
        TOrderType iType;
        TInt iNumber;
        };
    
    TInt iIndex;
    TInt iOrderIndexBefore;
    TInt iOrderIndexComposition;
    TInt iOrderIndexAfter;
    TInt iNumberAsynchronousRequests;
    TOrderArray* iOrderArray;
    RFastLock iLock;
    };

#endif /*__TREQUESTORDER_H__*/
