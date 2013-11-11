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
* Description:   DRM Play Server
*
*/




#ifndef DRMCUSTOMCOMMANDASYNCAO_H
#define DRMCUSTOMCOMMANDASYNCAO_H

#include <e32base.h>
#include "DRMPlaySession.h"
// #include <mdaaudiosampleplayer.h>

class CDRMPlayServerSession;

class CDRMCustomCommandAsyncAO : public CActive
    {
public:
    enum TCustomCommandType
        {
        ECustomCommandWithResult,
        ECustomCommandWithoutResult,
        };
public:
    static CDRMCustomCommandAsyncAO* NewL( const RMessage2& aMessage, 
                                           TCustomCommandType aType,
                                           CDRMPlayServerSession& aParent );
    ~CDRMCustomCommandAsyncAO();
    
    TMMFMessageDestinationPckg& GetMMFMessageDestinationPckg();
    TInt GetMMFMessageFunction();
    // Returns referecens to descriptors,
    // the descriptors are still owned by this object
    HBufC8* GetData1FromClient();
    HBufC8* GetData2FromClient();
    HBufC8* GetDataToClient();
    
    TInt SetActive();
    
    void TransferOwnershipL( const RMessage2& aMessage );
    
protected:
    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );
    
private:
    CDRMCustomCommandAsyncAO( TCustomCommandType aType, CDRMPlayServerSession& aParent );
    void ConstructL( const RMessage2& aMessage );
    
private:
    TCustomCommandType iCustomCommandType;
    TBool iAmCompleted;
    //TAsyncCallbackStatePckgBuf  iCallbackState;
    CDRMPlayServerSession& iParent;
    TMMFMessageDestinationPckg iMMFMessageDestinationPckg;
    TInt iMMFMessageFunction;
    HBufC8* iData1FromClient;
    HBufC8* iData2FromClient;
    HBufC8* iDataToClient;
    RMessage2* iMessage;
    };

#endif // DRMCUSTOMCOMMANDASYNCAO_H

// End of File
