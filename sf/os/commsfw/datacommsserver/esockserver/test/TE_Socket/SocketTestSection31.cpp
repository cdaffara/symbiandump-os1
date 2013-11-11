// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the cpp file for ESOCK basic socket test section 31
// 

// EPOC includes
#include <e32base.h>
#include <ss_std.h>

// Test system includes
#include "SocketTestSection31.h"
#include <test/es_dummy.h>

const TInt KBufferLength= 60000;



// Test step 31.1
// Sending more than 16k packets.
// Stream
// Blocking mode
// Using pdummy
// 
const TDesC& CSocketTest31_1::GetTestName()
    {
    _LIT(ret,"Test31.1");
    return ret;
    }

enum TVerdict CSocketTest31_1::InternalDoTestStepL( void )
    {
    TVerdict verdict = EPass;

    Logger().WriteFormat(_L("Test Purpose: Sending large amount of data (more than 16K,60k) by using Stream"));
    
    // connect to esock
    Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyFour;
    TInt ret = ss.Connect(pref);
    CleanupClosePushL(ss);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret); 
    
    Logger().WriteFormat(_L("Finding for dummy protocol 4"));
    TProtocolDesc protoInfo;
    ret = ss.FindProtocol(_L("Dummy Protocol 4"), protoInfo);
    Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    Logger().WriteFormat(_L("Open a socket on Dummy Protocol 4"));
    RSocket sock;
    CleanupClosePushL (sock);
    ret = sock.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
    Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    Logger().WriteFormat(_L("Perform connect"));
    TRequestStatus status;
    TSockAddr addr;
    sock.Connect(addr, status);
    User::WaitForRequest(status);
    TESTL(KErrNone == status.Int());
    
    
    // Setting options
    // Force the Send to 'flow off'
    TBool flowOff = ETrue;
    ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
    Logger().WriteFormat(_L("SetOpt (flow off) returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    // Setting up buffer
    HBufC8* writebuf = HBufC8::NewMaxLC(KBufferLength);
    HBufC8* readbuf  = HBufC8::NewMaxLC(KBufferLength);
   
    TPtr8 ptrwritebuf(const_cast<TUint8*>(writebuf->Ptr()), KBufferLength);
    TPtr8 ptrreadbuf(const_cast<TUint8*>(readbuf->Ptr()), KBufferLength);
   
    ptrwritebuf.SetLength(ptrwritebuf.MaxLength());
    ptrreadbuf.SetLength(ptrreadbuf.MaxLength());
   
    // Filling buffer
    ptrwritebuf.Repeat(_L8("Stream-packet: helloworld"));
   
    TRequestStatus txStat;
    TRequestStatus rxStat;
    TSockXfrLength txlen;
    TSockXfrLength rxlen;
    TInt txSize = 0, txCnt = 0;;
    TInt rxSize = 0, rxCnt = -1;
    TBool txSentAll = EFalse;
    TBool rxRecvAll = EFalse;
       
    while(rxSize < KBufferLength)
        {
        // sending packets
        if(txStat.Int() != KRequestPending && !txSentAll)
            {
            RDebug::Printf("tx status:%d, ", txStat.Int());
            TPtrC8 ptrtempbuf(ptrwritebuf.Mid(txSize));
            sock.Send(ptrtempbuf, 0, txStat, txlen);
                       
            // Now enable the write to complete, by ending the 'flow off'
            flowOff = EFalse;
            ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
            TESTL(KErrNone == ret);
            
            User::WaitForRequest(txStat);
            ++txCnt;
            txSize += txlen();
            
            if(txStat!=KErrNone)
                {
                Logger().WriteFormat(_L("Error in sending packet : %d "), txStat.Int());
                break;
                }
            RDebug::Printf("tx #%d, +%d\n", txCnt, txlen());
            if(txSize >= KBufferLength)
                {
                txSentAll=ETrue;
                }
            }
        
        // receiving packets
        if((rxStat.Int() != KRequestPending) && !rxRecvAll)
            {
            RDebug::Printf("rx status:%d, ", rxStat.Int());
            ++rxCnt;
            
            sock.RecvOneOrMore(ptrreadbuf, 0, rxStat, rxlen);
            User::WaitForRequest(rxStat);
            rxSize += rxlen();
            
            if(rxStat!=KErrNone)
                {
                Logger().WriteFormat(_L("Error in receiving packet : %d "), rxStat.Int());
                break;
                }
            RDebug::Printf("rx #%d, +%d\n", rxCnt, rxlen());
            
            if(rxSize >= KBufferLength)
                {
                rxRecvAll=ETrue;
                }
            }
        }
    
    CleanupStack::PopAndDestroy(2, writebuf);   //  writebuf and readbuf    
    CleanupStack::PopAndDestroy(2); // sock, ss
    
    SetTestStepResult(EPass);
    return TestStepResult();
    }

// Test step 31.2
// Sending more than 16k packets.
// Stream
// Non-Blocking mode
// Using pdummy
// 
const TDesC& CSocketTest31_2::GetTestName()
    {
    _LIT(ret,"Test31.2");
    return ret;
    }

enum TVerdict CSocketTest31_2::InternalDoTestStepL( void )
    {
    TVerdict verdict = EPass;

    Logger().WriteFormat(_L("Test Purpose: Sending large amount of data (more than 16K,60k) by using Stream"));
    
    // connect to esock
    Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyFour;
    TInt ret = ss.Connect(pref);
    CleanupClosePushL(ss);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret); 
    
    Logger().WriteFormat(_L("Finding for dummy protocol 4"));
    TProtocolDesc protoInfo;
    ret = ss.FindProtocol(_L("Dummy Protocol 4"), protoInfo);
    Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    Logger().WriteFormat(_L("Open a socket on Dummy Protocol 4"));
    RSocket sock;
    CleanupClosePushL (sock);
    ret = sock.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
    Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    Logger().WriteFormat(_L("Perform connect"));
    TRequestStatus status;
    TSockAddr addr;
    sock.Connect(addr, status);
    User::WaitForRequest(status);
    TESTL(KErrNone == status.Int());
    
    
    // Setting options
    // Force the Send to 'flow off'
    TBool flowOff = ETrue;
    ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
    Logger().WriteFormat(_L("SetOpt (flow off) returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    ret = sock.SetOpt(KSONonBlockingIO, KSOLSocket, 1);
    Logger().WriteFormat(_L("SetOpt NonBlockingIO returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    // Setting up buffer
    HBufC8* writebuf = HBufC8::NewMaxLC(KBufferLength);
    HBufC8* readbuf  = HBufC8::NewMaxLC(KBufferLength);
   
    TPtr8 ptrwritebuf(const_cast<TUint8*>(writebuf->Ptr()), KBufferLength);
    TPtr8 ptrreadbuf(const_cast<TUint8*>(readbuf->Ptr()), KBufferLength);
   
    ptrwritebuf.SetLength(ptrwritebuf.MaxLength());
    ptrreadbuf.SetLength(ptrreadbuf.MaxLength());
   
    // Filling buffer
    ptrwritebuf.Repeat(_L8("Stream-packet: helloworld"));
   
    TRequestStatus txStat;
    TRequestStatus rxStat;
    TSockXfrLength txlen;
    TSockXfrLength rxlen;
    TInt txSize = 0, txCnt = 0;;
    TInt rxSize = 0, rxCnt = -1;
    TBool txSentAll = EFalse;
    TBool rxRecvAll = EFalse;
       
    while(rxSize < KBufferLength)
        {
        // sending packets
        if(txStat.Int() != KRequestPending && !txSentAll)
            {
            RDebug::Printf("tx status:%d, ", txStat.Int());
            TPtrC8 ptrtempbuf(ptrwritebuf.Mid(txSize));
            sock.Send(ptrtempbuf, 0, txStat, txlen);
                        
            // Now enable the write to complete, by ending the 'flow off'
            flowOff = EFalse;
            ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
            TESTL(KErrNone == ret);
            
            User::WaitForRequest(txStat);
            ++txCnt;
            txSize += txlen();
            
            if(txStat!=KErrNone)
                {
                Logger().WriteFormat(_L("Error in sending packet : %d "), txStat.Int());
                break;
                }
            RDebug::Printf("tx #%d, +%d\n", txCnt, txlen());
            if(txSize >= KBufferLength)
                {
                txSentAll=ETrue;
                }
            }
        
        // receiving packets
        if((rxStat.Int() != KRequestPending) && !rxRecvAll)
            {
            RDebug::Printf("rx status:%d, ", rxStat.Int());
            ++rxCnt;
            
            sock.RecvOneOrMore(ptrreadbuf, 0, rxStat, rxlen);
            User::WaitForRequest(rxStat);
            rxSize += rxlen();
            
            if(rxStat!=KErrNone && rxStat!=KErrWouldBlock)
                {
                Logger().WriteFormat(_L("Error in receiving packet : %d "), rxStat.Int());
                break;
                }
            RDebug::Printf("rx #%d, +%d\n", rxCnt, rxlen());
            
            if(rxSize >= KBufferLength)
                {
                rxRecvAll=ETrue;
                }
            }
        }
    
    CleanupStack::PopAndDestroy(2, writebuf);   //  writebuf and readbuf    
    CleanupStack::PopAndDestroy(2); // sock, ss
    
    SetTestStepResult(EPass);
    return TestStepResult();
    }

// Test step 31.3
// Sending more than 16k packets.
// Datagram
// Blocking mode
// 
const TDesC& CSocketTest31_3::GetTestName()
    {
    _LIT(ret,"Test31.3");
    return ret;
    }

enum TVerdict CSocketTest31_3::InternalDoTestStepL( void )
    {
    TVerdict verdict = EPass;

    Logger().WriteFormat(_L("Test Purpose: Repeat send over 64K datagram using TSockXfrLength."));

    // connect to esock
    Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    
    // protocol allowing unlimited datagram size
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyFive;
    TInt ret = ss.Connect(pref);

    CleanupClosePushL(ss);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Finding for dummy protocol 5"));
    TProtocolDesc protoInfo;
    ret = ss.FindProtocol(_L("Dummy Protocol 5"), protoInfo);
    Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Opening a socket"));
    RSocket sock;
    ret = sock.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
    CleanupClosePushL(sock);
    Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    TPckgBuf<TUint> lenbuf;
    Logger().WriteFormat(_L("Getting SendBuf length"));
    ret = sock.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
    Logger().WriteFormat(_L("GetOpt returned %S, length %d"), 
        &EpocErrorToText(ret), lenbuf());
    TESTL(KErrNone == ret);

    TUint x(KBufferLength);
    lenbuf() = x;
    Logger().WriteFormat(_L("Setting SendBuf length to %d"), lenbuf());
    ret = sock.SetOpt(KSOSendBuf, KSOLSocket, lenbuf);
    Logger().WriteFormat(_L("SetOpt returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Getting SendBuf length"));
    ret = sock.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
    Logger().WriteFormat(_L("GetOpt returned %S, length %d"), &EpocErrorToText(ret), lenbuf());
    TESTL(KErrNone == ret);
    TESTL(lenbuf() == x);
    
    // Force the SendTo to 'flow off'
   TBool flowOff = ETrue;
   ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
   Logger().WriteFormat(_L("SetOpt (flow off) returned %S"), &EpocErrorToText(ret));
   TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Try sending datagram over protocol allowing unlimited datagram size"));
    TInetAddr      destAddr;
    TRequestStatus status;

    // Setting up buffer
    HBufC8* writebuf = HBufC8::NewMaxLC(KBufferLength);
    HBufC8* readbuf  = HBufC8::NewMaxLC(KBufferLength);
   
    TPtr8 ptrwritebuf(const_cast<TUint8*>(writebuf->Ptr()), KBufferLength);
    TPtr8 ptrreadbuf(const_cast<TUint8*>(readbuf->Ptr()), KBufferLength);
   
    ptrwritebuf.SetLength(ptrwritebuf.MaxLength());
    ptrreadbuf.SetLength(ptrreadbuf.MaxLength());
   
    // Filling buffer
    ptrwritebuf.Repeat(_L8("Datagram-packet: helloworld"));
   
    TRequestStatus txStat;
    TRequestStatus rxStat;
    TSockXfrLength txlen;
    TSockXfrLength rxlen;
    TInt txSize = 0, txCnt = 0;;
    TInt rxSize = 0, rxCnt = -1;
    TBool txSentAll = EFalse;
    TBool rxRecvAll = EFalse;
       
    while(rxSize < KBufferLength)
        {
        // sending packets
        if(txStat.Int() != KRequestPending && !txSentAll)
            {
            RDebug::Printf("tx status:%d, ", txStat.Int());
            TPtrC8 ptrtempbuf(ptrwritebuf.Mid(txSize));
            sock.SendTo(ptrtempbuf, destAddr, 0, txStat, txlen);
            
            // Now enable the write to complete, by ending the 'flow off'
            flowOff = EFalse;
            ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
            TESTL(KErrNone == ret);
            
            User::WaitForRequest(txStat);
            ++txCnt;
            txSize += txlen();
            
            if(txStat!=KErrNone)
                {
                Logger().WriteFormat(_L("Error in sending packet : %d "), txStat.Int());
                break;
                }
            RDebug::Printf("tx #%d, +%d\n", txCnt, txlen());
            if(txSize >= KBufferLength)
                {
                txSentAll=ETrue;
                }
            }
        
        // receiving packets
        if((rxStat.Int() != KRequestPending) && !rxRecvAll)
            {
            RDebug::Printf("rx status:%d, ", rxStat.Int());
            ++rxCnt;
            
            ptrreadbuf.Zero();
            sock.RecvFrom(ptrreadbuf,destAddr, 0, rxStat, rxlen);
            User::WaitForRequest(rxStat);
            rxSize += ptrreadbuf.Length();
            
            if(rxStat!=KErrNone)
                {
                Logger().WriteFormat(_L("Error in receiving packet : %d "), rxStat.Int());
                break;
                }
            RDebug::Printf("rx #%d, +%d\n", rxCnt, ptrreadbuf.Length());
            
            if(rxSize >= KBufferLength)
                {
                rxRecvAll=ETrue;
                }
            }
        }
    
    CleanupStack::PopAndDestroy(2, writebuf);   //  writebuf and readbuf    
    CleanupStack::PopAndDestroy(2); // sock, ss
    
    SetTestStepResult(EPass);
    return TestStepResult();
    }

// Test step 31.4
// Sending more than 16k packets.
// Datagram
// Non-Blocking mode
// 
const TDesC& CSocketTest31_4::GetTestName()
    {
    _LIT(ret,"Test31.4");
    return ret;
    }

enum TVerdict CSocketTest31_4::InternalDoTestStepL( void )
    {
    TVerdict verdict = EPass;

    Logger().WriteFormat(_L("Test Purpose: Repeat send over 64K datagram using TSockXfrLength."));

    // connect to esock
    Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss;
    
    // protocol allowing unlimited datagram size
    TSessionPref pref;
    pref.iAddrFamily = KDummyAddrFamily;
    pref.iProtocol = KDummyFive;
    TInt ret = ss.Connect(pref);

    CleanupClosePushL(ss);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Finding for dummy protocol 5"));
    TProtocolDesc protoInfo;
    ret = ss.FindProtocol(_L("Dummy Protocol 5"), protoInfo);
    Logger().WriteFormat(_L("FindProtocol returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Opening a socket"));
    RSocket sock;
    ret = sock.Open(ss, protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol);
    CleanupClosePushL(sock);
    Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    TPckgBuf<TUint> lenbuf;
    Logger().WriteFormat(_L("Getting SendBuf length"));
    ret = sock.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
    Logger().WriteFormat(_L("GetOpt returned %S, length %d"), 
        &EpocErrorToText(ret), lenbuf());
    TESTL(KErrNone == ret);

    TUint x(KBufferLength);
    lenbuf() = x;
    Logger().WriteFormat(_L("Setting SendBuf length to %d"), lenbuf());
    ret = sock.SetOpt(KSOSendBuf, KSOLSocket, lenbuf);
    Logger().WriteFormat(_L("SetOpt returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Getting SendBuf length"));
    ret = sock.GetOpt(KSOSendBuf, KSOLSocket, lenbuf);
    Logger().WriteFormat(_L("GetOpt returned %S, length %d"), &EpocErrorToText(ret), lenbuf());
    TESTL(KErrNone == ret);
    TESTL(lenbuf() == x);
    
    // Setting option
    // Force the SendTo to 'flow off'
    TBool flowOff = ETrue;
    ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
    Logger().WriteFormat(_L("SetOpt (flow off) returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    
    ret = sock.SetOpt(KSONonBlockingIO, KSOLSocket, 1);
    Logger().WriteFormat(_L("SetOpt NonBlockingIO returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);

    Logger().WriteFormat(_L("Try sending datagram over protocol allowing unlimited datagram size"));
    TInetAddr      destAddr;
    TRequestStatus status;

    // Setting up buffer
    HBufC8* writebuf = HBufC8::NewMaxLC(KBufferLength);
    HBufC8* readbuf  = HBufC8::NewMaxLC(KBufferLength);
   
    TPtr8 ptrwritebuf(const_cast<TUint8*>(writebuf->Ptr()), KBufferLength);
    TPtr8 ptrreadbuf(const_cast<TUint8*>(readbuf->Ptr()), KBufferLength);
   
    ptrwritebuf.SetLength(ptrwritebuf.MaxLength());
    ptrreadbuf.SetLength(ptrreadbuf.MaxLength());
   
    // Filling buffer
    ptrwritebuf.Repeat(_L8("Datagram-packet: helloworld"));
   
    TRequestStatus txStat;
    TRequestStatus rxStat;
    TSockXfrLength txlen;
    TSockXfrLength rxlen;
    TInt txSize = 0, txCnt = 0;;
    TInt rxSize = 0, rxCnt = -1;
    TBool txSentAll = EFalse;
    TBool rxRecvAll = EFalse;
       
    while(rxSize < KBufferLength)
        {
        // sending packets
        if(txStat.Int() != KRequestPending && !txSentAll)
            {
            RDebug::Printf("tx status:%d, ", txStat.Int());
            TPtrC8 ptrtempbuf(ptrwritebuf.Mid(txSize));
            sock.SendTo(ptrtempbuf, destAddr, 0, txStat, txlen);
                        
            // Now enable the write to complete, by ending the 'flow off'
            flowOff = EFalse;
            ret = sock.SetOpt(KDummyOptionSetFlowOffWrite, 0, flowOff);
            TESTL(KErrNone == ret);
            
            User::WaitForRequest(txStat);
            ++txCnt;
            txSize += txlen();
            
            if(txStat!=KErrNone)
                {
                Logger().WriteFormat(_L("Error in sending packet : %d "), txStat.Int());
                break;
                }
            RDebug::Printf("tx #%d, +%d\n", txCnt, txlen());
            if(txSize >= KBufferLength)
                {
                txSentAll=ETrue;
                }
            }
        
        // receiving packets
        if((rxStat.Int() != KRequestPending) && !rxRecvAll)
            {
            RDebug::Printf("rx status:%d, ", rxStat.Int());
            ++rxCnt;
            
            ptrreadbuf.Zero();
            sock.RecvFrom(ptrreadbuf,destAddr, 0, rxStat, rxlen);
            User::WaitForRequest(rxStat);
            rxSize += ptrreadbuf.Length();
            //rxSize += rxlen();
            
            if(rxStat!=KErrNone && rxStat!=KErrWouldBlock)
                {
                Logger().WriteFormat(_L("Error in receiving packet : %d "), rxStat.Int());
                break;
                }
            RDebug::Printf("rx #%d, +%d\n", rxCnt, ptrreadbuf.Length());
            
            if(rxSize >= KBufferLength)
                {
                rxRecvAll=ETrue;
                }
            }
        }
    
    CleanupStack::PopAndDestroy(2, writebuf);   //  writebuf and readbuf    
    CleanupStack::PopAndDestroy(2); // sock, ss
    
    SetTestStepResult(EPass);
    return TestStepResult();
    }


// Test step 31.5
// Using fixed async request slot pool and system free request pool connections
// 
const TDesC& CSocketTest31_5::GetTestName()
    {
    _LIT(ret,"Test31.5");
    return ret;
    }


CSocketTest31_5::CSockReq* CSocketTest31_5::CSockReq::NewLC()
    {
    CSockReq* sockReq = new(ELeave) CSockReq;
    CleanupStack::PushL(sockReq);
    return sockReq;
    }

CSocketTest31_5::CSockReq::~CSockReq()
    {
    TInt ret = iReq.Int();
    iSock.Close();
    if(ret == KRequestPending)
        {
        User::WaitForRequest(iReq);
        }
    }

TInt CSocketTest31_5::MakePendingRequestL()
    {
    CSockReq* req = CSockReq::NewLC();
    iSocks.AppendL(req);
    CleanupStack::Pop(req);
    
    TInt idx = iSocks.Count() - 1;
    TInt ret = req->iSock.Open(iSess, KDummyAddrFamily, KSockDatagram, KDummyOne);
    Logger().WriteFormat(_L("sock[%d].Open() returned %S"), idx, &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    req->iSock.Read(req->iData, req->iReq);
    User::After(250 * 1000);    // short delay to allow for almost-instant failure
    Logger().WriteFormat(_L("sock[%d].Read() status %S"), idx, &EpocErrorToText(req->iReq.Int()));
    return req->iReq.Int();
    }

enum TVerdict CSocketTest31_5::InternalDoTestStepL( void )
    {
    TVerdict verdict = EPass;

    Logger().WriteFormat(_L("Test Purpose: Test bounded and unbounded (default) async request slot pool."));

    Logger().WriteFormat(_L("Trying %d pending requests against default (unbounded pool) connection"), KESockDefaultMessageSlots + 1);
    TInt ret = iSess.Connect();
    CleanupClosePushL(iSess);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    for(TInt sockNum = 0; sockNum <= KESockDefaultMessageSlots; ++sockNum)
        {
        ret = MakePendingRequestL();
        TESTL(ret == KRequestPending);
        }
    iSocks.ResetAndDestroy();
    CleanupStack::PopAndDestroy(&iSess);

    Logger().WriteFormat(_L("Trying %d pending requests against old default (KESockDefaultMessageSlots pool) connection"), KESockDefaultMessageSlots + 1);
    ret = iSess.Connect(KESockDefaultMessageSlots);
    CleanupClosePushL(iSess);
    Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
    TESTL(KErrNone == ret);
    for(TInt sockNum = 0; sockNum < KESockDefaultMessageSlots; ++sockNum)
        {
        ret = MakePendingRequestL();
        TESTL(ret == KRequestPending);
        }
    ret = MakePendingRequestL();
    TESTL(ret == KErrServerBusy);
    iSocks.ResetAndDestroy();
    CleanupStack::PopAndDestroy(&iSess);

    SetTestStepResult(EPass);
    return TestStepResult();
    }

