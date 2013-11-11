// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements CWapSmsProvider and CWapSmsProviderWrite
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ws_prvdrTraces.h"
#endif

#include <e32std.h>
#include <wap_sock.h>
#include "ws_main.h"
#include "es_wsms.h"
#include "WAPDGRM.H"
#include "ws_obsvr.h"
#include "smsprot.h"
#include <es_mbuf.h>
#include "smsstacklog.h"

//
// implementation of CWapSmsProvider
//

    // CWapSmsProvider policies
    static _LIT_SECURITY_POLICY_C1(wapSmsProviderSetLocalNamePolicy, ECapabilityNetworkServices );
    static _LIT_SECURITY_POLICY_C1(wapSmsProviderSetOptionPolicy, ECapability_None);
    static _LIT_SECURITY_POLICY_C1(wapSmsProviderGetLengthIoctlPolicy,ECapability_None);
    static _LIT_SECURITY_POLICY_C1(wapSmsProviderGetMessageParamLengthIoctlPolicy,ECapability_None);
    static _LIT_SECURITY_POLICY_C1(wapSmsProviderGetMessageParametersIoctlPolicy,ECapabilityReadDeviceData);
    static _LIT_SECURITY_POLICY_C1(wapSmsProviderWritePolicy,ECapabilityNetworkServices);


/**
 *  Factory
 */
CWapSmsProvider* CWapSmsProvider::NewL(CWapSmsProtocol* aProtocol)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_NEWL_1, "CWapSmsProvider::NewL()");

    CWapSmsProvider* provider=new(ELeave) CWapSmsProvider(aProtocol);
    CleanupStack::PushL(provider);
    provider->iWapSmsProviderWrite = CWapSmsProviderWrite::NewL(*provider);
    CleanupStack::Pop();
    return provider;
    } // CWapSmsProvider::NewL


/**
 *  Constructor
 */
CWapSmsProvider::CWapSmsProvider(CWapSmsProtocol* aProtocol)
    :iMessageType(ESmartMessage)
    ,iProtocol(aProtocol)
    ,iSendPending(EFalse)
    ,iIoctlOutstanding(EFalse)
    ,iIsNewStyleClient(EFalse)
    ,iStatusReportScheme(EWapSmsDefault)
    {
    iRecvdMsgQueue.SetOffset(CWapDatagram::LinkOffset());
    } // CWapSmsProvider::CWapSmsProvider


/**
 *  Destructor
 */
CWapSmsProvider::~CWapSmsProvider()
    {
    iSAPLink.Deque();

    while(!iRecvdMsgQueue.IsEmpty())
        {
        CWapDatagram* msg = iRecvdMsgQueue.First();
        iRecvdMsgQueue.Remove(*msg);
        delete msg;
        }

    delete iWapSmsProviderWrite;
    } // CWapSmsProvider::~CWapSmsProvider


/**
 *  Return WAPSMS options
 */
TInt CWapSmsProvider::GetOption(TUint aLevel,TUint aName, TDes8& aOption)const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_GETOPTION_1, "CWapSmsProvider::GetOption");

    TInt ret=KErrNone;
    if (TInt(aLevel)==KWapSmsOptionLevel && TInt(aName)==KWapSmsOptionNameDCS)
        aOption = TPtrC8((TText8*)&iDataCodingScheme,sizeof(TWapSmsDataCodingScheme));
    else
        ret=iProtocol->GetOption(aLevel,aName,aOption,NULL);
    return ret;
    } // CWapSmsProvider::GetOption


/**
 *  Set WAPSMS options
 *  
 *  @capability None
 *  
 */
TInt CWapSmsProvider::SetOption(TUint aLevel, TUint aName, const TDesC8& aOption)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_SETOPTION_1, "CWapSmsProvider::SetOption");
    if(!iSecurityChecker || (iSecurityChecker->CheckPolicy(wapSmsProviderSetOptionPolicy,"CWapSmsProvider SetOption policy check") != KErrNone))
        {
        return KErrPermissionDenied;
        }

    TInt ret=KErrNone;
    if (TInt(aLevel)==KWapSmsOptionLevel)
    {
        switch(TInt(aName))
        {
        case KWapSmsOptionNameDCS:
            {
            iDataCodingScheme = static_cast<TWapSmsDataCodingScheme>(*aOption.Ptr());
            break;
            }
        case KWapSmsOptionWapDatagram:
            {
            iMessageType=EWapDatagram;
            break;
            }
        case KWapSmsStatusReportScheme:
            {
            iStatusReportScheme = static_cast<TWapSmsStatusReportScheme>(*aOption.Ptr());
            break;              
            }
        case KWapSmsOptionNewStyleClient:
            {
            iIsNewStyleClient = ETrue;
            break;
            }
        case KWapSmsOptionOKToDeleteMessage:
            {
            //Get the first message from the queue
            CWapDatagram* msg = iRecvdMsgQueue.First();
            //Find and delete from SAR
            TBool found=iProtocol->FindAndDeleteMsg(*msg);
            if(!found)
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_SETOPTION_2, "CWapSmsProvider::SetOption: Error. Couldn't find the message in the SAR for deletion");
                break;
                }
            //Remove from the queue
            iRecvdMsgQueue.Remove(*msg);
            delete msg;
            break;
            }
        default:
            ret=KErrNotSupported;
        }

    }
    else
    {
        ret=iProtocol->SetOption(aLevel,aName,aOption,NULL);
    }
    return ret;
    } // CWapSmsProvider::SetOption


/**
 *  Shutdown the SAP
 */
void CWapSmsProvider::Shutdown(TCloseType aType)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_SHUTDOWN_1, "CWapSmsProvider::Shutdown");
    if (aType!=CServProviderBase::EImmediate)
        iSocket->CanClose();
    } // CWapSmsProvider::Shutdown


/**
 *  Setup datagram with socket specific options and forward to protocol
 *  Return zero to block the send - will be completed by CWapSapMessageSender
 *  
 *  @capability NetworkServices
 */
TInt CWapSmsProvider::Write(RMBufChain& aBufChain, TUint /*options*/, TSockAddr* aAddr)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_WRITE_1, "CWapSmsProvider::Write()");

        if(!iSecurityChecker || (iSecurityChecker->CheckPolicy(wapSmsProviderWritePolicy,"CWapSmsProvider Write policy check") != KErrNone))
        {
        return KErrPermissionDenied;
        }
    // @note: LOGIFH2A2 macro for logging esock write
#ifdef SMSLOGGERIF
    TInt length = aBufChain.Length();
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_WRITE_2, "CWapSmsProvider::Write [%d bytes]", length);
    TBuf8<0x100> dumpBuf;
    if(length > 0x100)
        {

        TInt parts=0;
        TInt offset = 0;
        while (offset < length)
            {
            aBufChain.CopyOut(dumpBuf, offset);
            offset += length;
            LOGIF2(_L8("ESock WAP concantonated part: %d"),parts++);
            LOGIF2(_L8("ESOCK WRITE: %S"),&dumpBuf);
            LOGIFH2A2(_L8("ESOCK WRITE: "),dumpBuf);
            }

    }
    else
    {
        aBufChain.CopyOut(dumpBuf, 0);
        LOGIF2(_L8("ESOCK WRITE: %S"),&dumpBuf);
        LOGIFH2A2(_L8("ESOCK WRITE: "),dumpBuf);
    }
#endif

    // Note that if this fails somehow it still frees the buf chain and sets itself active - it's
    // not clear to me whether this is good behaviour but it's the pre-mbuf behaviour too
    iWapSmsProviderWrite->Start(aBufChain, *aAddr);
    return KErrNone;
    } // CWapSmsProvider::Write


/**
 *  Read a datagram off the queue
 */
TInt CWapSmsProvider::GetData(RMBufChain& aBufChain, TUint aLength, TUint /*options*/,TSockAddr* aAddr)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_GETDATA_1, "CWapSmsProvider::GetData()");

    CWapDatagram* msg = iRecvdMsgQueue.First();
    if(!iIsNewStyleClient)
        {
        iRecvdMsgQueue.Remove(*msg);
        }
    TInt err = msg->WapDatagram(aBufChain, aLength);

    //@note: LOGIFH2A2 macro for logging esock getdata
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_GETDATA_2, "CWapSmsProvider::GetData");
    // Logging migrated to WapDatagram() for ease of descriptor ref

    if (err > 0 && aAddr)
        {
        TWapAddr* wapAddr = reinterpret_cast<TWapAddr*>(aAddr);
        TInt toPort,fromPort;
        msg->Ports(fromPort,toPort);
        wapAddr->SetWapPort(static_cast<TWapPortNumber>(fromPort));
        TBuf8<KMaxSockAddrSize> addrBuf;
        addrBuf.Copy(msg->FromAddress());
        wapAddr->SetWapAddress(addrBuf);
        }
    if(!iIsNewStyleClient)
        {
        delete msg;
        }
    return err > 0? 1: err;     // datagrams are atoms not byte counts
    } // CWapSmsProvider::GetData


/**
 *  New data has arrived notify ESOCK.
 */
void CWapSmsProvider::NewData(CWapDatagram* aMsg)
    {
    TBool notifyEsock = EFalse;
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_NEWDATA_1, "CWapSmsProvider::NewData");

    if(iIoctlOutstanding && iName==KSOGetLength && iIsNewStyleClient)
        {
        TPckgBuf<TInt> buf= aMsg->WapDatagramLength();
        iSocket->IoctlComplete(&buf);
        iIoctlOutstanding= EFalse;
        iName= NULL;
        notifyEsock = ETrue;
        }
    else if(iIoctlOutstanding && iName==KSOGetMessageParametersLength && iIsNewStyleClient)
        {
        CBufFlat* buffer = aMsg->SmsExternalisedStream();
        TPckgBuf<TInt> buf = buffer->Size();
        iSocket->IoctlComplete(&buf);
        iIoctlOutstanding= EFalse;
        iName= NULL;
        notifyEsock = ETrue;
        }
    else if(iIoctlOutstanding && iName==KSOGetMessageParameters && iIsNewStyleClient)
        {
        CBufFlat* buffer = aMsg->SmsExternalisedStream();
        TPtr8 buf = buffer->Ptr(0);
        iSocket->IoctlComplete(&buf);
        iIoctlOutstanding= EFalse;
        iName= NULL;
        notifyEsock = ETrue;
        }
    else if(iName!=KSOGetLength && iName!=KSOGetMessageParametersLength && iName!=KSOGetMessageParameters && iIsNewStyleClient)
        {
        notifyEsock= EFalse;
        }

    if(!iIsNewStyleClient || notifyEsock)
        iSocket->NewData(1);
    //else we notify ESock in IOCTL for new client
    } // CWapSmsProvider::NewData


/**
 *  Error happened, notify ESOCK
 */
void CWapSmsProvider::Error(TInt aError, TUint aOperationMask)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_ERROR_1, "CWapSmsProvider::Error [aError=%d, mask=%u] ", aError, aOperationMask);

    iSocket->Error(aError, aOperationMask);
    } // CWapSmsProvider::Error


/**
 *  Return the offset to the dblquelink
 */
TInt CWapSmsProvider::LinkOffset()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_LINKOFFSET_1, "CWapSmsProvider::LinkOffset");

    return _FOFF(CWapSmsProvider,iSAPLink);
    } // CWapSmsProvider::LinkOffset


/**
 *  Return the address associated with the sap
 */
void CWapSmsProvider::LocalName(TSockAddr& aAddr) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_LOCALNAME_1, "CWapSmsProvider::LocalName");

    Mem::Copy(&aAddr,&iLocalAddress,sizeof(TSockAddr));
    } // CWapSmsProvider::LocalName


/**
 *  Set the local address of the sap - called by RSocket::Bind
 *  
 *  @capability NetworkServices
 */
TInt CWapSmsProvider::SetLocalName(TSockAddr& aAddr)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_SETLOCALNAME_1, "CWapSmsProvider::SetLocalName()");

        if(!iSecurityChecker || (iSecurityChecker->CheckPolicy(wapSmsProviderSetLocalNamePolicy,"CWapSmsProvider SetLocalName policy check") != KErrNone))
        {
        return KErrPermissionDenied;
        }
    TWapAddr* wapAddr = reinterpret_cast<TWapAddr*>(&aAddr);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_SETLOCALNAME_2, "CWapSmsProvider::SetLocalName %d",wapAddr->WapPort());
    // Due ESOCK interface port EWapPortUnspecified value (-1)  can be transferred as a maximum unsigned 16 int
    if (wapAddr->WapPort()==EWapPortUnspecified || wapAddr->WapPort()==static_cast<TUint16>(EWapPortUnspecified))
        {
        if(!iProtocol->AllocateLocalAddress(iLocalAddress))
            return KErrInUse;
        else return KErrNone;
        }


    TInt ret=iProtocol->AddrAlreadyUsedByWAP(*wapAddr,this);
    if(ret == KErrInUse) return ret;
    else if(ret == KErrAlreadyExists) return KErrNone;

    TSmsAddr addr;
    if(wapAddr->Port() <=255)
        addr.SetSmsAddrFamily(ESmsAddrApplication8BitPort);
    else
        addr.SetSmsAddrFamily(ESmsAddrApplication16BitPort);

    addr.SetPort(wapAddr->Port());

    if((iProtocol->SmsProtocol()->SmsAddrIsAlreadyUsed(NULL,addr)))
        return KErrInUse;

    Mem::Copy(&iLocalAddress,&aAddr,sizeof(TSockAddr));
    TInt err;
    TRAP(err,ret = iProtocol->CheckSarL(*wapAddr,this));
    if(err!=KErrNone)
            return err;
    if(ret!=KErrNone)
        {
        Error(ret,MSocketNotify::EErrorAllOperations);
        }
    return KErrNone;
    } // RSocket::Bind


/**
 *  Returns true if aAddr matches the local address of the sap
 */
TBool CWapSmsProvider::MatchesLocalAddress(const TWapAddr& aAddr)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_MATCHESLOCALADDRESS_1, "CWapSmsProvider::MatchesLocalAddress");

    return (iLocalAddress == aAddr);
    } // CWapSmsProvider::MatchesLocalAddress

//


/**
 *  Called to perform specific IO control by the client (i.e. Wap Messaging API).The client is able
 *  to get the size of the datagram once it has been received or confirm the receipt
 *  of the message.
 *  
 *  Only one ioctl request may be outstanding at any one time.
 *  
 *  Implementation of pure virtual CServProviderBase::Ioctl().
 *  
 *  @param aLevel the IOCTL level. // Can be only KSolWapProv
 *  @param aName the IOCTL name.
 *  @param aOption the IOCTL option.
 *  
 */
void CWapSmsProvider::Ioctl(TUint aLevel,TUint aName,TDes8 * /*aOption*/)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_IOCTL_1, "CWapSmsProvider::Ioctl [aLevel=%u, aName=%u]", aLevel, aName);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_IOCTL_2, "CWapSmsProtocol::Ioctl [provider=0x%08x]",this);

    iName=aName;
    switch (aLevel)
        {
        case KSolWapProv:
            {
            if(iIoctlOutstanding || !iIsNewStyleClient)
                {
                Error(KErrInUse,MSocketNotify::EErrorIoctl);
                break;
                }
            switch (iName)
                {
                case KSOGetLength:
                //
                // Get the length
                //
                    {
                    if(!iSecurityChecker || (iSecurityChecker->CheckPolicy(wapSmsProviderGetLengthIoctlPolicy,"CWapSmsProvider GetLength Ioctl policy check") != KErrNone))
                        {
                        Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    iIoctlOutstanding = ETrue;
                    //check the queue for any other message for this client and call new data if any exist
                    if(!iRecvdMsgQueue.IsEmpty())
                        {
                        CWapDatagram* msg = iRecvdMsgQueue.First();
                        //check the datagram.
                        if(msg->IsComplete())
                            {
                            NewData(msg);
                            }
                        else
                            {
                            //  else notify the client with error.
                            //  Note:this can happen if client uses 8-Bit port number for 7-Bit datagram
                            Error(KErrGeneral,MSocketNotify::EErrorIoctl);
                            iIoctlOutstanding = EFalse;
                            }

                        }
                    break;
                    }
                
                case KSOGetMessageParametersLength:
                //
                // Get the Message Parameters Length
                //
                    {
                    if(!iSecurityChecker || (iSecurityChecker->CheckPolicy(wapSmsProviderGetMessageParamLengthIoctlPolicy,"CWapSmsProvider KSOGetMessageParametersLength Ioctl policy check") != KErrNone))
                        {
                        Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    iIoctlOutstanding = ETrue;
                    //check the queue for any other message for this client and call new data if any exist
                    if(!iRecvdMsgQueue.IsEmpty())
                        {
                        CWapDatagram* msg = iRecvdMsgQueue.First();
                        //check the datagram.
                        if(msg->IsComplete())
                            {
                            NewData(msg);
                            }
                        else
                            {
                            //  else notify the client with error.
                            //  Note:this can happen if client uses 8-Bit port number for 7-Bit datagram
                            Error(KErrGeneral,MSocketNotify::EErrorIoctl);
                            iIoctlOutstanding = EFalse;
                            }

                        }
                    break;
                    }
                    
                case KSOGetMessageParameters:
                //
                // Get the Message Parameters
                //
                    {
                    if(!iSecurityChecker || (iSecurityChecker->CheckPolicy(wapSmsProviderGetMessageParametersIoctlPolicy,"CWapSmsProvider GetMessageParameters Ioctl policy check") != KErrNone))
                        {
                        Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    iIoctlOutstanding = ETrue;
                    //check the queue for any other message for this client and call new data if any exist
                    if(!iRecvdMsgQueue.IsEmpty())
                        {
                        CWapDatagram* msg = iRecvdMsgQueue.First();
                        //check the datagram.
                        if(msg->IsComplete())
                            {
                            NewData(msg);
                            }
                        else
                            {
                            //  else notify the client with error.
                            //  Note:this can happen if client uses 8-Bit port number for 7-Bit datagram
                            Error(KErrGeneral,MSocketNotify::EErrorIoctl);
                            iIoctlOutstanding = EFalse;
                            }

                        }
                    break;
                    }


                default:
                //
                // Unsupported ioctl name
                //
                    {
                    // Error gracefully
                    Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                    }
                }
            break;
            }
        default:
            // Gracefully error in release build
            Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
        }
    } // CServProviderBase::Ioctl


/**
 *  Cancel IOCTL- can only have one outstanding IOCTL at a time
 */
void CWapSmsProvider::CancelIoctl(TUint aLevel,TUint aName)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_CANCELIOCTL_1, "CWapSmsProvider::CancelIoctl()");

    if(!iIoctlOutstanding || (aName!=iName) || (aLevel!=KSolWapProv))
        {
        Error(KErrNotFound,MSocketNotify::EErrorIoctl);
        }
    else
        {
        iIoctlOutstanding=EFalse;
        }
    } // CWapSmsProvider::CancelIoctl


/**
 *  Start the provider- does nothing
 */
void CWapSmsProvider::Start()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_START_1, "CWapSmsProvider::Start()");

    } // CWapSmsProvider::Start

void CWapSmsProvider::AddToQueue(CWapDatagram* aMsg)
/**
 *  Adding the datagram to received messages queue
 */
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_ADDTOQUEUE_1, "CWapSmsProvider::AddToQueue...");

    iRecvdMsgQueue.AddLast(*aMsg);
    NewData(aMsg);
    } // CWapSmsProvider::AddToQueue


TInt CWapSmsProvider::SecurityCheck(MProvdSecurityChecker* aSecurityChecker)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDER_SECURITYCHECK_1, "CWapSmsProvider::SecurityCheck()");

    iSecurityChecker=aSecurityChecker;
    return KErrNone;
    } // CWapSmsProvider::SecurityCheck


TWapSmsStatusReportScheme CWapSmsProvider::GetStatusReportScheme()
    {
    return iStatusReportScheme;
    }

    
TWapSmsDataCodingScheme CWapSmsProvider::GetDataCodingScheme()
    {
    return iDataCodingScheme;
    }


//
// implementation of CWapSmsProviderWrite
//


/**
 *  2 phase constructor
 */
CWapSmsProviderWrite* CWapSmsProviderWrite::NewL(CWapSmsProvider& aWapSmsProvider)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDERWRITE_NEWL_1, "CWapSmsProviderWrite::NewL");
    CWapSmsProviderWrite* self = new (ELeave) CWapSmsProviderWrite(aWapSmsProvider);
    CleanupStack::PushL(self);
    self->iMsgSender = CWapSapMessageSender::NewL(aWapSmsProvider.iProtocol->SmsProtocol(), &aWapSmsProvider);
    CleanupStack::Pop(self);
    return self;
    } // CWapSmsProviderWrite::NewL


/**
 *  Constructor
 */
CWapSmsProviderWrite::CWapSmsProviderWrite(CWapSmsProvider& aWapSmsProvider)
    :CActive(EPriorityStandard)
    ,iWapSmsProvider(aWapSmsProvider)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDERWRITE_CTOR_1, "CWapSmsProviderWrite::CWapSmsProviderWrite()");

    CActiveScheduler::Add(this);
    } // CWapSmsProviderWrite::CWapSmsProviderWrite


/**
 *  Destructor
 */
CWapSmsProviderWrite::~CWapSmsProviderWrite()
    {
    Cancel();
    delete iMsgSender;
    delete iDes;
    } // CWapSmsProviderWrite::~CWapSmsProviderWrite


void CWapSmsProviderWrite::Start(RMBufChain& aBufChain, TSockAddr& aAddr)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDERWRITE_START_1, "CWapSmsProviderWrite::Start");
    iWapAddr = reinterpret_cast<TWapAddr&>(aAddr);
    delete iDes;
    iDes = NULL;
    TRAPD(err, (iDes = HBufC8::NewL(aBufChain.Length())) );
    if(err == KErrNone)
        {
        TPtr8 desBuf(iDes->Des());
        desBuf.SetLength(aBufChain.Length());
        aBufChain.CopyOut(desBuf, 0);
        // Logging migrated from CWapSmsProvider::GetData
        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDERWRITE_START_2, "ESOCK READ: \"%s\"", *iDes);
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
        LogSmsIfHexBuf(_L8("ESOCK READ"), *iDes);
        TBuf<40> timestamp;
        SmsTimeStampL(timestamp);
        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS,CSMSSTACKTESTUTILS_PRINTANDTESTDECODEL_1, "%S",timestamp);
#endif
        }
    aBufChain.Free();

    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, err);
    SetActive();
    } // CWapSmsProviderWrite::Start


void CWapSmsProviderWrite::RunL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDERWRITE_RUNL_1, "CWapSmsProviderWrite::RunL");
    User::LeaveIfError(iStatus.Int());

    //no need to use cleanup stack
    CWapDatagram* datagram = CWapDatagram::NewL(*iDes);

    if (iWapSmsProvider.iDataCodingScheme == EWapSms7BitDCS)
        datagram->SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    else
        datagram->SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
    
    if (iWapSmsProvider.iStatusReportScheme == EWapSmsTPSRR)
        {
        datagram->SetStatusReportScheme(ETPSRRScheme);
        }
    else
        {
        datagram->SetStatusReportScheme(EDefaultScheme);
        } 
        
    TBuf<KMaxSockAddrSize> addrBuf;
    addrBuf.Copy(iWapAddr.WapAddress());
    datagram->SetToAddress(addrBuf);
    TInt fromPort = iWapSmsProvider.iLocalAddress.WapPort();
    datagram->SetPorts(fromPort, iWapAddr.WapPort());

    iMsgSender->SendDatagramL(datagram); // takes ownership of datagram
    } // CWapSmsProviderWrite::RunL


TInt CWapSmsProviderWrite::RunError(TInt aError)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDERWRITE_RUNERROR_1, "CWapSmsProviderWrite::RunError");
    iWapSmsProvider.Error(aError, MSocketNotify::EErrorSend);
    return KErrNone;
    } // CWapSmsProviderWrite::RunError


void CWapSmsProviderWrite::DoCancel()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROVIDERWRITE_DOCANCEL_1, "CWapSmsProviderWrite::DoCancel");
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrCancel);
    } // CWapSmsProviderWrite::DoCancel


// EOF - WS_PRVDR.CPP
