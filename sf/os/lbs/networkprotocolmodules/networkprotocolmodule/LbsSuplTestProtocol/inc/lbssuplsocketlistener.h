// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 @test
*/
#ifndef LBS_SUPLSOCKETLISTENER_H
#define LBS_SUPLSOCKETLISTENER_H


//-----------------------------------------------------------------------------------
#include <e32base.h>
#include <in_sock.h>

const TInt KSuplMaxMessageLength = 8192; // Enough to keep all the assistance data

// CSuplSocketListener: active object wrapping socket read requests
class CSuplSocketListener : public CActive
    {

public:
    static CSuplSocketListener* NewL(RSocket& aSocket, RBuf8& aBuffer);
    static CSuplSocketListener* NewLC(RSocket& aSocket, RBuf8& aBuffer);
    ~CSuplSocketListener();
    void ReadMessage(TRequestStatus& aStatus);

    //Implemented functions from CActive
    void DoCancel();
    void RunL();    
    
protected:
    CSuplSocketListener(RSocket& aSocket, RBuf8& aBuffer);
    void ConstructL();
    void IssueRead(TDes8& aBuffer);
    void SuplMessageLength();

private:
    RSocket& iSocket; // Socket to listen to
    RBuf8& iBuffer; // To read data in
    TRequestStatus* iUserStatus; // Status of the client AO
    TSockXfrLength iFragmentLength; // Length of a chunk of message read from the socket
    TBool iIsFirstFragment; // Set to false after the first fragment of a message is received
    TInt iLengthReadSoFar; // Count of bytes read after each call to RecvOneOrMore completes
    TInt iSuplMessageLength; // Total message of the length as indicated by first byte of the message
    };
    
#endif //LBS_SUPLSOCKETLISTENER_H