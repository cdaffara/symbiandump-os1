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

#ifndef CMockPhoneBookStoreMessHandler_H
#define CMockPhoneBookStoreMessHandler_H

#include "mmockmesshandlerbase.h"

class CMmMessageManager;
class CMmMessageRouter;

typedef CArrayPtrFlat<TENStoreResponse> CRespArray;

class CMockPhoneBookStoreMessHandler : public CBase, public MMockMessHandlerBase
    {
public: 
    static CMockPhoneBookStoreMessHandler* NewL(CMmMessageRouter* aMessageManager);
    virtual ~CMockPhoneBookStoreMessHandler();

public: 
    virtual TInt ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage);
	virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);

private:
    CMockPhoneBookStoreMessHandler(CMmMessageRouter* aMessageManager);
    void ConstructL();

private:  
    CMmMessageRouter* iMessageRouter; 
    
    RPointerArray<CRespArray> iEnRespArray;
    };

#endif // CMockPhoneBookStoreMessHandler_H

