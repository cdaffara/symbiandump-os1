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

#ifndef __COMMSBUFASYNCREQINTERNAL_INL__
#define __COMMSBUFASYNCREQINTERNAL_INL__

inline TCommsBufAsyncRequestInternal::TCommsBufAsyncRequestInternal(RCommsBufAsyncRequest& aRequest, TDblQueLink& aLink)
: iRequest(aRequest),
iLink(aLink),
iMinSize(0),
iMaxSize(KMaxTInt)
    {
    
    }

inline TCommsBufAsyncRequestInternal::TCommsBufAsyncRequestInternal(RCommsBufAsyncRequest& aRequest, TDblQueLink& aLink, TInt aMinSize, TInt aMaxSize)
 :  iRequest(aRequest),
 iLink(aLink),
 iMinSize(aMinSize),
 iMaxSize(aMaxSize)
    {
    
    }

#endif // __COMMSBUFASYNCREQINTERNAL_INL__
