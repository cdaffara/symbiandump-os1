// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the CSmsProvider service access point (SAP) class.
// Includes
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsprovTraces.h"
#endif

#include "smsprot.h"

#include <es_ver.h>
#include <es_mbuf.h>

#include "Gsmuelem.h"
#include "gsmubuf.h"
#include "Gsmumsg.h"

#include "smsustrm.h"
#include "smspmain.h"
#include "smspfacadestor.h"

#include "smsstacklog.h"

// CSmsProvider policies
//
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlDeleteSmsMessagePolicy,ECapabilityWriteUserData);
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlEnumerateSmsMessagesPolicy,ECapabilityReadUserData );
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlReadMessageSucceededPolicy,ECapabilityReadUserData );
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlReadMessageFailedPolicy,ECapabilityReadUserData );
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlSendSmsMessagePolicy,ECapabilityNetworkServices );
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlWriteSmsMessagePolicy,ECapabilityWriteUserData );
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlReadSmsParamsPolicy,ECapability_None );
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlCompleteReadSmsParamsPolicy,ECapability_None );
static _LIT_SECURITY_POLICY_C1(smsProviderIoctlWriteSmsParamsPolicy,ECapabilityWriteDeviceData );

// following not implemented as too paranoid
//static _LIT_SECURITY_POLICY_C1(smsProviderIoctlSelectModemPresentPolicy,ECapabilityWriteDeviceData );
//static _LIT_SECURITY_POLICY_C1(smsProviderIoctlSelectModemNotPresentPolicy,ECapabilityWriteDeviceData );
static _LIT_SECURITY_POLICY_C1(smsProviderSetLocalNamePolicy,ECapabilityNetworkServices);
static _LIT_SECURITY_POLICY_C1(smsProviderWritePolicy,ECapability_None);

/**
 *  2 Phase constructor.
 *  
 *  @param aProtocol a reference to the SMS protocol object.
 *  @leave Leaves if ContructL() leaves, or not enough memory is available.
 *  @return a new CSmsProvider object.
 *  
 */
CSmsProvider* CSmsProvider::NewL(CSmsProtocol& aProtocol)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_NEWL_1, "CSmsProvider::NewL");
    
    CSmsProvider* self =new(ELeave) CSmsProvider(aProtocol);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_NEWL_2, "-> CSmsProvider::NewL - done");
    
    return self;
    }

/**
 *  C'tor
 *  
 *  @param aProtocol a reference to the SMS protocol object.
 */
CSmsProvider::CSmsProvider(CSmsProtocol& aProtocol)
: iProtocol(aProtocol),iEnumSocket(EFalse),iNumOfEnumeratedMessages(0)
    {
    }

/**
 *  2nd Phase of construction.
 *  Subscribes to the protocol as an observer, and creates our send and
 *  receive buffers.
 *  
 */
void CSmsProvider::ConstructL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_CONSTRUCTL_1, "CSmsProvider::ConstructL");
    
    iProtocol.AddSmsMessageObserverL(*this);
    SetObserverAddedToProtocol(ETrue);
    iRecvBufSegArray=new(ELeave) CArrayPtrFlat<CBufSeg>(8);
    iSendBufSeg = CBufSeg::NewL(KSmsMaxSegmentLength);
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_CONSTRUCTL_2, "-> CSmsProvider::ConstructL - done");
    }

/**
 *  D'tor
 *  
 *  Removes this SAP as an observer of the SMS protocol, and
 *  frees the send and receive buffers.
 *  
 */
CSmsProvider::~CSmsProvider()
    {
    if( ObserverAddedToProtocol() )
        {
        iProtocol.RemoveSmsMessageObserver(*this);
        }
    if( iRecvBufSegArray )
        {
        iRecvBufSegArray->ResetAndDestroy();
        delete iRecvBufSegArray;
        }
    delete iSendBufSeg;
    }

/**
 *  Does nothing.  Implementation of pure virtual CServProviderBase::Start().
 *  
 */
void CSmsProvider::Start()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_START_1, "CSmsProvider::Start");
    }

/**
 *  Returns the local address of this SAP.  Implementation of
 *  pure virtual CServProviderBase::LocalName().
 *  
 */
void CSmsProvider::LocalName(TSockAddr& aAddr) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_LOCALNAME_1, "CSmsProvider::LocalName");
    aAddr = iLocalAddress;
    }

/**
 *  Sets the local address of this SAP by binding it to the protocol.
 *  The protocol ensures that there are no duplicate observers, and
 *  then calls back on the CSmsProvider::SetLocalAddress() method
 *  to set the address.
 *  
 *  Implementation of the pure virtual CServProviderBase::SetLocalName().
 *  
 *  @capability NetworkServices
 */
TInt CSmsProvider::SetLocalName(TSockAddr& aAddr)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SETLOCALNAME_1, "CSmsProvider::SetLocalName");
    
    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderSetLocalNamePolicy,"CSmsProvider SetLocal Name policy check") != KErrNone) )
        {
        return KErrPermissionDenied;
        }
    TSmsAddr& smsAddr=static_cast<TSmsAddr&>(aAddr);
    if( ( smsAddr.SmsAddrFamily() == ESmsAddrApplication8BitPort  || smsAddr.SmsAddrFamily() == ESmsAddrApplication16BitPort ) && smsAddr.Port() == 0 )
        {
        if( !iProtocol.AllocateLocalAddress(smsAddr) )
            {
            return KErrInUse;
            }
        }
    return iProtocol.BindSmsMessageObserver(*this,smsAddr);
    }

/**
 *  Called by the protocol to retrieve the remote name of the connection.
 *  This protocol is not connection oriented so this is not supported.
 *  
 *  Implementation of the pure virtual CServProviderBase::RemName().
 *  
 */
void CSmsProvider::RemName(TSockAddr& /*aAddr*/) const
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_REMNAME_1, "CSmsProvider::RemName");
    BULLSEYE_RESTORE
    }

/**
 *  Called by the protocol to set the remote name of the connection.
 *  This protocol is not connection oriented so this is not supported.
 *  
 *  Implementation of the pure virtual CServProviderBase::SetRemName().
 *  
 */
TInt CSmsProvider::SetRemName(TSockAddr& /*aAddr*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SETREMNAME_1, "CSmsProvider::SetRemName");
    return KErrNotSupported;
    BULLSEYE_RESTORE
    }

/**
 *  Returns the current value of an option setting for this SAP.
 *  No settings are currently defined.
 *  Implementation of pure virtual CServProviderBase::GetOption().
 *  
 */
TInt CSmsProvider::GetOption(TUint /*aLevel*/,TUint /*aName*/,TDes8& /*aOption*/) const
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_GETOPTION_1, "CSmsProvider::GetOption");
    return 0;
    BULLSEYE_RESTORE
    }

/**
 *  Called to perform specific IO control by the client.  All of the SMS protocol
 *  services are provided through this interface.
 *  
 *  The local address of this SAP must already be bound, and only one ioctl request
 *  may be outstanding at any one time.
 *  
 *  A resulting socket error of KErrEof can result from a KErrNoMemory during a
 *  preceding write to the socket.
 *  
 *  Implementation of pure virtual CServProviderBase::Ioctl().
 *  
 *  @param aLevel the IOCTL level.  Only KSolSmsProv is supported.
 *  @param aName the IOCTL name.
 *  @param aOption the IOCTL option.
 *  
 */
void CSmsProvider::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_IOCTL_1, "CSmsProvider::Ioctl [aLevel=%u, aName=%u provider=0x%08x]",  aLevel, aName, (TUint)this);
    
    // Panic in debug mode if this call is invalid in this SAPs current state
    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrUnbound,SmspPanic(KSmspPanicWrongSmsAddressFamily));
    __ASSERT_DEBUG(!IoctlOutstanding(),SmspPanic(KSmspPanicIoctlAlreadyOutstanding));
    // Gracefully handle invalid calls in release build
    if( iLocalAddress.SmsAddrFamily()==ESmsAddrUnbound )
        {
        iSocket->Error(KErrNotReady,MSocketNotify::EErrorIoctl);
        return;
        }
    if( IoctlOutstanding() )
        {
        iSocket->Error(KErrInUse,MSocketNotify::EErrorIoctl);
        return;
        }
    // General state is OK, now try to service the request
    iName=aName;
    switch( aLevel )
        {
        case KSolSmsProv:
            {
            switch( iName )
                {
                case KIoctlSupportOODClass0SmsMessages:
                    {
                    if( iProtocol.iReassemblyStore )
                        {
                        if( iProtocol.iReassemblyStore->IsSeparateClass0StoreSupported() )
                            {
                            iSocket->IoctlComplete(NULL);
                            }
                        else
                            {
                            iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                            }
                        }
                    else
                        {
                        iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                        }
                    } break;
                case KIoctlSendSmsMessage:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlSendSmsMessagePolicy,"CSmsProvider Ioctl SendSmsMessage policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrLocalOperation,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    __ASSERT_DEBUG(aOption!=NULL,SmspPanic(KSmspPanicOptionBufferNull));
                    // Handle bad requests gracefully
                    if( iLocalAddress.SmsAddrFamily()==ESmsAddrLocalOperation )
                        {
                        iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    if( aOption == NULL )
                        {
                        iSocket->Error(KErrArgument,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    // Read message from socket
                    CSmsMessage* smsmessage=NULL;
                    TRAPD(ret,(smsmessage=InternalizeMessageL()));
                    if( ret!=KErrNone )
                        {
                        iSendBufSeg->Reset();
                        iSocket->Error(ret,MSocketNotify::EErrorIoctl);
                        }
                    else
                        {
                            // Pass the message to the protocol for sending
                        TPckgBuf<TUint> buf;
                        buf.Copy(*aOption);
                        SetIoctlOutstanding(ETrue);
                        iProtocol.SendSmsMessage(smsmessage,*this, buf());
						}
                    } break;
                case KIoctlEnumerateSmsMessages:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlEnumerateSmsMessagesPolicy, "CSmsProvider Ioctl EnumerateSmsMessages policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    // Handle bad requests gracefully
                    if( iLocalAddress.SmsAddrFamily()==ESmsAddrSendOnly )
                        {
                        iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    
                    SetIoctlOutstanding(ETrue);
                    iProtocol.EnumeratePhone(*this);
                    } break;
                case KIoctlWriteSmsMessage:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlWriteSmsMessagePolicy,"CSmsProvider IoctlWriteSmsMessage policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    // Read message from the socket
                    CSmsMessage*smsmessage=NULL;
                    TRAPD(ret,(smsmessage=InternalizeMessageL()));
                    if( ret!=KErrNone )
                        {
                        iSendBufSeg->Reset();
                        iSocket->Error(ret,MSocketNotify::EErrorIoctl);
                        }
                    else
                        {
                        // Pass message to the protocol for writing
                        SetIoctlOutstanding(ETrue);
                        iProtocol.WriteSmsMessage(smsmessage,*this);
                        }
                    } break;
                case KIoctlDeleteSmsMessage:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlDeleteSmsMessagePolicy,"CSmsProvider Ioctl DeleteSmsMessage policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    // Handle bad requests gracefully
                    if( iLocalAddress.SmsAddrFamily()==ESmsAddrSendOnly )
                        {
                        iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    // Read message from the socket
                    CSmsMessage*smsmessage=NULL;
                    TRAPD(ret,(smsmessage=InternalizeMessageL()));
                    if( ret!=KErrNone )
                        {
                        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_IOCTL_3, "-> CSmsProvider::Ioctl - CSmsProvider::InternalizeMessageL [ret=%d]", ret);
                        iSendBufSeg->Reset();
                        iSocket->Error(ret, MSocketNotify::EErrorIoctl);
                        }
                    else
                        {
                        // Pass request to protocol
                        SetIoctlOutstanding(ETrue);
                        iProtocol.DeleteSmsMessage(smsmessage,*this);
                        }
                    } break;
                case KIoctlReadMessageSucceeded:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlReadMessageSucceededPolicy,"CSmsProvider Ioctl ReadMessageSucceeded policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    __ASSERT_DEBUG(NumSegments(iSegmentIndex==NumSegments(iRecvBufSegArray->At(0)->Size())),SmspPanic(KSmspPanicBadClientIoctlCall));
                    // Delete entry from reassemblystore
                    CSmsMessage*smsmessage=NULL;
                    TRAPD(ret,(smsmessage=InternalizeMessageL(iRecvBufSegArray->At(0))));
                    if( ret==KErrNone )
                        {
                        TRAP(ret,(iProtocol.DeleteSMSFromReaStoreL( *smsmessage )));
                        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_IOCTL_4, "-> CSmsProvider::Ioctl - CSmsProvider::DeleteSMSFromReaStoreL [ret=%d]", ret);
                        }
                    else
                        {
                        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_IOCTL_5, "-> CSmsProvider::Ioctl - CSmsProvider::InternalizeMessageL [ret=%d]", ret);
                        }
                    delete smsmessage;
                    // Looking for more sms left in the store
                    // This is now done after finishing the readprocess
                    iProtocol.MessageReadedSuccessfully();
                    if( iEnumSocket )
                        {
                        --iNumOfEnumeratedMessages;
                        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_IOCTL_6, "-> CSmsProvider::Ioctl - [iNumOfEnumeratedMessages=%d]", iNumOfEnumeratedMessages);
                        if( iNumOfEnumeratedMessages <= 0 )
                            {
                            iProtocol.iPhoneEnumerationObserver=NULL;
                            iEnumSocket=EFalse;
                            iProtocol.MessageReadedSuccessfully();
                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_IOCTL_7, "-> CSmsProvider::Ioctl - [iNumOfEnumeratedMessages=NULL]");
                            }
                        }
                    // Remove the message from the receive buffer & complete
                    delete iRecvBufSegArray->At(0);
                    iRecvBufSegArray->At(0) = NULL;
                    iRecvBufSegArray->Delete(0);
    
                    iSegmentIndex=0;
                    iSocket->IoctlComplete(NULL);
                    } break;
                case KIoctlReadMessageFailed:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlReadMessageFailedPolicy,"CSmsProvider Ioctl ReadMessageFailed policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    __ASSERT_DEBUG(NumSegments(iSegmentIndex<=NumSegments(iRecvBufSegArray->At(0)->Size())),SmspPanic(KSmspPanicBadClientIoctlCall));
                    // Handle bad requests gracefully
                    if( iLocalAddress.SmsAddrFamily()==ESmsAddrSendOnly )
                        {
                        if( iEnumSocket )
                            {
                            --iNumOfEnumeratedMessages;
                            if( iNumOfEnumeratedMessages <= 0 )
                                {
                                iProtocol.iPhoneEnumerationObserver=NULL;
                                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_IOCTL_8, "-> CSmsProvider::Ioctl - fail [iNumOfEnumeratedMessages=NULL]");
                                iEnumSocket=EFalse;
                                iProtocol.MessageReadedSuccessfully();
                                }
                            }
                        iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    // Re-notify the socket that data is available, reset the segment
                    // index of the current message back to the start & complete
                    iSocket->NewData(iSegmentIndex);
                    iSegmentIndex=0;
                    iSocket->IoctlComplete(NULL);
                    } break;
                case KIoctlReadSmsParams:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlReadSmsParamsPolicy,"CSmsProvider Ioctl ReadSmsParams policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    // Handle bad requests gracefully
                    if( iLocalAddress.SmsAddrFamily()!=ESmsAddrLocalOperation )
                        {
                        iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                        return;
                        }
                   
                   // Pass request on to the protocol
                   SetIoctlOutstanding(ETrue);
                   iProtocol.ReadSmsParameters(*this);
                    } break;
                case KIoctlCompleteReadSmsParams:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlCompleteReadSmsParamsPolicy,"CSmsProvider Ioctl CompleteReadSmsParams policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    __ASSERT_DEBUG(NumSegments(iSegmentIndex==NumSegments(iRecvBufSegArray->At(0)->Size())),SmspPanic(KSmspPanicBadClientIoctlCall));
                    // Handle bad requests gracefully
                    if ( iLocalAddress.SmsAddrFamily()==ESmsAddrSendOnly )
                        {
                        iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    // Remove the parameter list from the receive buffer & complete
                    delete iRecvBufSegArray->At(0);
                    iRecvBufSegArray->At(0) = NULL;
                    iRecvBufSegArray->Delete(0);
                    iSegmentIndex=0;
                    iSocket->IoctlComplete(NULL);
                    } break; 
                case KIoctlWriteSmsParams:
                    {
                    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderIoctlWriteSmsParamsPolicy,"CSmsProvider Ioctl WriteSmsParams policy check") != KErrNone) )
                        {
                        iSocket->Error(KErrPermissionDenied,MSocketNotify::EErrorIoctl);
                        return;
                        }
                    // Read parameters from the socket
                    CMobilePhoneSmspList*mobilePhoneSmspList=NULL;
                    TRAPD(ret,(mobilePhoneSmspList=InternalizeParametersL()));
                    if( ret!=KErrNone )
                        {
                        iSendBufSeg->Reset();
                        iSocket->Error(ret,MSocketNotify::EErrorIoctl);
                        }
                    else
                        {
                        // Pass parameters to the protocol for writing.
                        // CSmsWriteParams takes ownership of mobilePhoneSmspList.
                        SetIoctlOutstanding(ETrue);
                        iProtocol.WriteSmsParameters(mobilePhoneSmspList, *this);
                        }
                    } break;
                default:
                    {
                    // Panic in debug build
                    __ASSERT_DEBUG(EFalse,SmspPanic(KSmspUndefinedName));
                    // Error gracefully in release build
                    iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
                    } break;
                }
            } break;
        default:
            {
            // Unsupported ioctl level, panic in debug build
            __ASSERT_DEBUG(EFalse,SmspPanic(KSmspUndefinedLevel));
            // Gracefully error in release build
            iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
            } break;
        }
    }

/**
 *  Cancels an outstanding ioctl.
 *  Since there can only be a single ioctl request outstanding, the parameters
 *  must match those of the original request.
 *  Implementation of the pure virtual CServProviderBase::CancelIoctl().
 *  
 *  @param aLevel the level of the ioctl request to cancel.
 *  @param aName the name of the ioctl request to cancel.
 *  
 */
void CSmsProvider::CancelIoctl(TUint aLevel, TUint aName)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_CANCELIOCTL_1, "CSmsProvider::CancelIoctl [aLevel=%u, aName=%u]", aLevel, aName);
    
    // Panic in debug mode if this call is invalid in this SAPs current state
    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrUnbound,SmspPanic(KSmspPanicWrongSmsAddressFamily));
    __ASSERT_DEBUG(iName==aName,SmspPanic(ESmspBadIoctlName));
    if( iName != aName )
        {
        return;
        }
    switch( aLevel )
        {
        case KSolSmsProv:
            {
            // Request cancel via protocol
            switch( iName )
                {
                case KIoctlSendSmsMessage:
                    {
                    iProtocol.CancelSendSmsMessage(*this);
                    } break;
                case KIoctlEnumerateSmsMessages:
                    {
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    iProtocol.CancelEnumeratePhone(*this);
                    iEnumSocket=EFalse;
                    iProtocol.iPhoneEnumerationObserver=NULL;
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_CANCELIOCTL_2, "-> CSmsProvider::CancelIoctl - [iNumOfEnumeratedMessages=NULL]");
                    } break;
                case KIoctlWriteSmsMessage:
                    {
                    iProtocol.CancelWriteSmsMessage(*this);
                    } break;
                case KIoctlDeleteSmsMessage:
                    {
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    iProtocol.CancelDeleteSmsMessage(*this);
                    } break;
                case KIoctlReadMessageSucceeded:
                case KIoctlReadMessageFailed:
                    {
                    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly,SmspPanic(KSmspPanicWrongSmsAddressFamily));
                    } break;
                case KIoctlReadSmsParams:
                    {
                    iProtocol.CancelReadSmsParams();
                    } break;
                case KIoctlWriteSmsParams:
                    {
                    iProtocol.CancelWriteSmsParams();
                    } break;
                default:
                    {
                    __ASSERT_DEBUG(EFalse,SmspPanic(KSmspUndefinedName));
                    } break;
                }
            } break;
        default:
            {
            __ASSERT_DEBUG(EFalse,SmspPanic(KSmspUndefinedLevel));
            } break;
        }
    }

/**
 *  Sets an option on the SAP.  No options are currently implemented.
 *  
 *  Implements the pure virtual CServProviderBase::SetOption().
 *  
 */
TInt CSmsProvider::SetOption(TUint /*aLevel*/,TUint /*aName*/,const TDesC8& /*aOption*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SETOPTION_1, "CSmsProvider::SetOption()");
    return 0;
    BULLSEYE_RESTORE
    }

/**
 *  Called by the socket server to write data into the send queue.
 *  The data is a single segment of a serialized SMS message.
 *  
 *  A KErrNoMemory condition arising in this write will not be reported directly (due to
 *  the ESOCK design), instead it will be reflected in a KErrEof from the subsequent Ioctl()
 *  call.
 *  
 *  Implementation of the pure virtual CServProviderBase::Write().
 *  
 *  @param aBufChain the data to write into the send buffer.
 *  @param aOptions not used.
 *  @param aAddr not used.
 *  @return 1 (datagram written) on success, otherwise an error.
 *  
 *  @capability None
 */
TInt CSmsProvider::Write(RMBufChain& aBufChain, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_WRITE_1, "CSmsProvider::Write");
    
    if( !iSecurityChecker || (iSecurityChecker->CheckPolicy(smsProviderWritePolicy,"CSmsProvider Write policy check") != KErrNone) )
        {
        return KErrPermissionDenied;
        }
    __ASSERT_DEBUG(iLocalAddress.SmsAddrFamily()!=ESmsAddrUnbound,SmspPanic(KSmspPanicWrongSmsAddressFamily));
    
    /// @note: LOGIFH2A2 macro for logging esock write
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_WRITE_2, "-> CSmsProvider::Write [%d bytes]", aBufChain.Length());
#ifdef SMSLOGGERIF
	HBufC8* debugBuf = HBufC8::New(aBufChain.Length());
	if(debugBuf)
		{
		TPtr8 debugData = debugBuf->Des();
		aBufChain.CopyOut(debugData);
		LOGIF2(_L8("ESOCK WRITE: %S"),&debugData);
		LOGIFH2A2(_L8("ESOCK WRITE: "),debugData);
		delete debugBuf;
		}
#endif
    
    TUint bytesCopied = 0;
    // Append all of the mbufs to the send buffer
    TInt ret = KErrNone;
    TMBufIter iter(aBufChain);
    while( iter.More() )
        {
        RMBuf* p = iter++;
        TRAP(ret, iSendBufSeg->InsertL(iSendBufSeg->Size(), p->Ptr(), p->Length()));
        if(ret != KErrNone)
            break;
        bytesCopied += p->Length();
        }
    if( ret != KErrNone )
        {
        iSendBufSeg->Reset();	// it has always done this, but won't innocent data get zapped?
        }
    else
        {
        aBufChain.Free();	   // we accepted it all; flag these by consuming all buffers
        }
    return (ret == KErrNone)? 1: ret;
    }

/**
 *  Called by the socket server to retrieve data that this SAP has indicated
 *  is waiting in its buffers.
 *  
 *  Implentation of the pure virtual CServProviderBase::GetData().
 *  
 *  Once the provider has indicated new data is available using
 *  MSocketNotify::NewData(), the socket server will call on this method
 *  to retrieve the serialized SMS message in segments.
 *  Once the sockets client has streamed out the entire message, it
 *  will call on ioctl with KIoctlReadMessageSucceeded which resets the internal
 *  counters.
 *  
 *  @param aBufChain the buffer to insert the data.
 *  @param aLength not used.
 *  @param aOptions not used.
 *  @param aAddr not used.
 *  @return 1 (datagram read) on success, otherwise an error.
 *  
 */
TInt CSmsProvider::GetData(RMBufChain& aBufChain, TUint /*aLength*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
    {
    __ASSERT_DEBUG((iLocalAddress.SmsAddrFamily()!=ESmsAddrUnbound) && (iLocalAddress.SmsAddrFamily()!=ESmsAddrSendOnly),SmspPanic(KSmspPanicWrongSmsAddressFamily));

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_GETDATA_1, "CSmsProvider::GetData [provider=0x%08x]", (TUint)this);
    
    // Get the segmented buffer of first message
    CBufSeg* recvbufseg=iRecvBufSegArray->At(0);
    TInt size=recvbufseg->Size();
    __ASSERT_DEBUG(iSegmentIndex<NumSegments(size),SmspPanic(KSmspPanicBadClientMessageRead));

    // Caculate the position of the next segment of the serialized message,
    // insert into the buffer parameter and update our segment counter
    TInt pos=iSegmentIndex*KSmsMaxSegmentLength;
    TInt length = pos+KSmsMaxSegmentLength>size? size-pos: KSmsMaxSegmentLength;
    
    TRAPD(err, aBufChain.AllocL(length));
    if( err == KErrNone )
        {
        // For want of a segmented buffer copy for Mbufs we have a little loop. Because we're
        // reading consecutive data out of the CBufSeg there shouldn't be a bad performance hit
        // (see CBufSeg::Ptr() doco; CBufBase::Read() uses this)
        TInt segPos = 0;
        TMBufIter iter(aBufChain);
        while( segPos < length )
            {
            RMBuf* p = iter++;
            TInt readSize = Min(p->Size(), length - segPos);
            recvbufseg->Read(pos + segPos, p->Buffer(), readSize);
            segPos += readSize;
            }
        }
    else if( err == KErrNoMBufs )
        {
        return KErrNoMBufs;	// ask ESock to call us back when some buffers are free
        }
    else
        {
        iSocket->Error(err, MSocketNotify::EErrorRecv);
        }
    ++iSegmentIndex;
    return 1;	// datagrams are counted as atoms not bytes
    }

/**
 *  Called by the socket server to indicate the provider should connect to
 *  a peer.  Not a connection oriented protocol so this is not implemented.
 *  
 *  Implementation of the pure virtual CServProviderBase::ActiveOpen().
 *  
 */
void CSmsProvider::ActiveOpen()
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_ACTIVEOPEN_1, "CSmsProvider::ActiveOpen [does nothing]");
    BULLSEYE_RESTORE
    }

/**
 *  Called by the socket server to indicate the provider should connect to
 *  a peer.  Not a connection oriented protocol so this is not implemented.
 *  
 *  Implementation of the pure virtual CServProviderBase::ActiveOpen().
 *  
 */
void CSmsProvider::ActiveOpen(const TDesC8& /*aConnectionData*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_ACTIVEOPEN1_1, "CSmsProvider::ActiveOpen [does nothing]");
    BULLSEYE_RESTORE
    }

/**
 *  Called by the socket server to indicate the provider should wait for an
 *  incoming client connection request.  Not a connection oriented protocol
 *  so this is not implemented.
 *  
 *  Implementation of the pure virtual CServiceProviderBase::PassiveOpen().
 *  
 */
TInt CSmsProvider::PassiveOpen(TUint /*aQueSize*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_PASSIVEOPEN_1, "CSmsProvider::PassiveOpen [not supported]");
    return KErrNotSupported;
    BULLSEYE_RESTORE
    }

/**
 *  Called by the socket server to indicate the provider should wait for an
 *  incoming client connection request.  Not a connection oriented protocol
 *  so this is not implemented.
 *  
 *  Implementation of the pure virtual CServiceProviderBase::PassiveOpen().
 *  
 */
TInt CSmsProvider::PassiveOpen(TUint /*aQueSize*/,const TDesC8& /*aConnectionData*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_PASSIVEOPEN1_1, "CSmsProvider::PassiveOpen [not supported]");
    return KErrNotSupported;
    BULLSEYE_RESTORE
    }

/**
 *  Called by the socket server to shutdown a connection.
 *  
 *  Implementation of the pure virtual CServProviderBase::Shutdown().
 *  
 */
void CSmsProvider::Shutdown(TCloseType aOption)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SHUTDOWN_1, "CSmsProvider::Shutdown [aOption=%d]", aOption);
    
    TInt messagesInBuffer = iRecvBufSegArray->Count();
    for( TInt index = 0; index < messagesInBuffer; ++index )
        {
        // Read message from the socket
        CSmsMessage*smsmessage=NULL;
        TRAPD(ret,(smsmessage=InternalizeMessageL(iRecvBufSegArray->At(index))));
        if( ret == KErrNone )
            {
            TRAP(ret, (iProtocol.iReassemblyStore->SetMessagePassedToClientL(*smsmessage, EFalse)));
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SHUTDOWN_2, "-> CSmsProvider::Shutdown - SetMessagePassedToClientL [ret=%d]", ret);
            }
        else
            {
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SHUTDOWN_3, "-> CSmsProvider::Shutdown - CSmsProvider::InternalizeMessageL leave [ret=%d]", ret);
            }
        delete smsmessage;
        }
    
    if( iEnumSocket && iProtocol.iPhoneEnumerationObserver == this )
        {
        iEnumSocket=EFalse;
        iProtocol.iPhoneEnumerationObserver=NULL;
        }
    if( ObserverAddedToProtocol() )
        {
        iProtocol.CancelSendSmsMessage(*this);
        }    
    if( aOption!=CServProviderBase::EImmediate )
        {
        iSocket->CanClose();
        }
    } 

/**
 *  Called by the socket server to shutdown a connection.  Simply calls
 *  the single parameter version shutdown method.
 *  
 *  Implementation of the pure virtual CServProviderBase::Shutdown().
 *  
 */
void CSmsProvider::Shutdown(TCloseType aOption, const TDesC8& /*aDisconnectionData*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SHUTDOWN1_1, "CSmsProvider::Shutdown");
	Shutdown(aOption);
	}

/**
 *  Called by the socket server to indicate the provider should choose
 *  a local address and bind to it.  Not supported by this protocol.
 *  
 *  Implementation of the pure virtual CServProviderBase::AutoBind().
 *  
 */
void CSmsProvider::AutoBind()
	{
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_AUTOBIND_1, "CSmsProvider::AutoBind [does nothing]");
    BULLSEYE_RESTORE
	}

/**
 *  Called by the SMS protocol to obtain the local address of this SAP.
 *  
 *  @return the local address of this SAP.
 *  
 */
const TSmsAddr& CSmsProvider::GetLocalAddress() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_GETLOCALADDRESS_1, "CSmsProvider::GetLocalAddress");
	return iLocalAddress;
	}

/**
 *  Called by the SMS protocol to bind a local address to this SAP.
 *  
 */
void CSmsProvider::SetLocalAddress(const TSmsAddr& aSmsAddr)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SETLOCALADDRESS_1, "CSmsProvider::SetLocalAddress");
	iLocalAddress = aSmsAddr;
	}

/**
 *  Called by the protocol when the modem connection state has changed.
 *  Notifies any standard KIoctlSelect requests of this event.
 *  
 *  @param aStatus either KIoctlSelectModemPresent or KIoctlSelectModemNotPresent.
 *  
 */
void CSmsProvider::ModemNotificationCompleted(TInt aStatus)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_MODEMNOTIFICATIONCOMPLETED_1, "CSmsProvider::ModemNotificationCompleted [aStatus=%d]", aStatus);

	if( !IoctlOutstanding() )
	    {
		iSocket->Error(aStatus,MSocketNotify::EErrorIoctl);
	    }
	}

/**
 *  Called by the protocol when a message send request made by this
 *  SAP has completed.
 *  
 *  @param aStatus the result of the send.
 *  
 */
void CSmsProvider::MessageSendCompleted(TInt aStatus)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_MESSAGESENDCOMPLETED_1, "CSmsProvider::MessageSendCompleted [aStatus=%d]", aStatus);
    
    iSocket->Error(aStatus,MSocketNotify::EErrorIoctl);
    SetIoctlOutstanding(EFalse);
    }

/**
 *  Called by the protocol when a message has been received for this SAP.
 *  
 *  @param aSmsMessage the received message.
 *  @return KErrNone if the message was serialized to the receive buffer successfully.
 *  
 */
TInt CSmsProvider::MessageReceived(const CSmsMessage& aSmsMessage,TDes& /*aDes*/)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_MESSAGERECEIVED_1, "CSmsProvider::MessageReceived");
    
    // Attempt to serial the message to the receive buffer & notify
    // the socket of the new data
    TInt numnewsegments=0;
    TRAPD(ret,(numnewsegments=ExternalizeMessageL(aSmsMessage,ETrue)));
    if( ret==KErrNone )
        {
        iSocket->NewData(numnewsegments);
        }
    return ret;
    }

/**
 *  Informs protocol whether client confirms received message
 */
TBool CSmsProvider::ClientConfirmsMessage() const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_CLIENTCONFIRMSMESSAGE_1, "CSmsProvider::ClientConfirmsMessage");
    
    return ETrue;
    }

/**
 *  Informs protocol whether address is ued by the observer
 */
TInt CSmsProvider::SmsAddrIsDuplicate(const MSmsMessageObserver* aObserver, const TSmsAddr& aAddr) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SMSADDRISDUPLICATE_1, "CSmsProvider::SmsAddrIsDuplicate");
    
    if( this == aObserver )
        {
        return EFalse;
        }
    return iLocalAddress==aAddr;
    }

/**
 *  Called by the protocol when an enumeration of the phone's message stores
 *  requested by this SAP has completed.
 *  
 *  @param aStatus the result of the enumeration.
 *  
 */
void CSmsProvider::EnumeratePhoneCompleted(TInt aStatus)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_ENUMERATEPHONECOMPLETED_1, "CSmsProvider::EnumeratePhoneCompleted [aStatus=%d]", aStatus);
    
    // Attempt to serialize all enumerated messages to the receive buffer
    TInt numnewsegments=0;
    TInt count=0;
    if( aStatus==KErrNone )
        {
        // Save current message count in case we need to rollback on error
        TInt nummessages=iRecvBufSegArray->Count();
        TRAP(aStatus, (numnewsegments=ExternalizeEnumeratedMessagesL(count)));
        if( aStatus==KErrNone )
            {
            // Success, obtain the message count and notify socket of the new data
            if( numnewsegments>0 )
                {
                iSocket->NewData(numnewsegments);
                }
            }
        else
            {
            // Error, rollback the messages we added
            for( TInt i=(iRecvBufSegArray->Count()-nummessages)-1; i>=0; i-- )
                {
                delete iRecvBufSegArray->At(i);
                iRecvBufSegArray->Delete(i);
                }
            }
        }
    // On success, complete with the count of messages enumerated
    if( aStatus==KErrNone )
        {
        TPckgBuf<TInt> buf;
        buf()=count;
        if( count>0 )
            {
            iEnumSocket=ETrue;
            }
        else
            {
            iEnumSocket=EFalse;
            iProtocol.iPhoneEnumerationObserver=NULL;
            }
        iNumOfEnumeratedMessages=count;
        iSocket->IoctlComplete(&buf);
        }
    else
        {
        iEnumSocket=EFalse;
        if( iProtocol.iPhoneEnumerationObserver == this)
            {
            iProtocol.iPhoneEnumerationObserver=NULL;
            }
        iSocket->Error(aStatus,MSocketNotify::EErrorIoctl);
        }
    SetIoctlOutstanding(EFalse);
    }

/**
 *  Called by the protocol when a request to write a message to
 *  a phone store by this SAP has completed.
 *  Implements the pure virtual MSmsMessageObserver::MessageWriteCompleted().
 *  
 *  @param aStatus the result of the write operation.
 *  
 */
void CSmsProvider::MessageWriteCompleted(TInt aStatus, const CSmsMessage* aSmsMessage)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_MESSAGEWRITECOMPLETED_1, "CSmsProvider::MessageWriteCompleted [aStatus=%d]", aStatus);

	// If no errors at present populate the buffer
	if( aStatus == KErrNone )
	    {
		TRAP(aStatus, (PopulateBufferWithPDUSlotsL(*aSmsMessage)));
	    }
	if( aStatus != KErrNone )
		{
		iSocket->Error(aStatus,MSocketNotify::EErrorIoctl);
		}
	SetIoctlOutstanding(EFalse);
	} // CSmsProvider::MessageWriteCompleted


/**
 *  Create and populate a buffer containing store type and PDU slot indexes
 *  
 *  @param aSmsMessage the message containing the slot information.
 *  
 */
void CSmsProvider::PopulateBufferWithPDUSlotsL(const CSmsMessage& aSmsMessage)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_POPULATEBUFFERWITHPDUSLOTSL_1, "CSmsProvider::PopulateBufferWithPDUSlotsL");
    
    // Create buffer for store id and PDU slot indexes based on size of slot array
    
    HBufC8* buf = HBufC8::NewL(aSmsMessage.iSlotArray.Count()+1 * sizeof(TUint));
    buf->Des().Append(aSmsMessage.Storage());
    
    TInt count = aSmsMessage.iSlotArray.Count();
    for( TInt index=0; index<count; ++index )
        {
        buf->Des().Append(aSmsMessage.iSlotArray[index].iIndex);
        }
    
    TPtr8 textBufPtr(buf->Des());
    
    iSocket->IoctlComplete(&textBufPtr);
    delete buf;

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_POPULATEBUFFERWITHPDUSLOTSL_2, "-> CSmsProvider::PopulateBufferWithPDUSlotsL - done");
    }

/**
 *  Called by the protocol when a request to delete a message from
 *  a phone store by this SAP has completed.
 *  Implements the pure virtual MSmsMessageObserver::MessageDeleteCompleted().
 *  
 *  @param aStatus the result of the deletion.
 *  
 */
void CSmsProvider::MessageDeleteCompleted(TInt aStatus)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_MESSAGEDELETECOMPLETED_1, "CSmsProvider::MessageDeleteCompleted [aStatus=%d]", aStatus);
    iSocket->Error(aStatus,MSocketNotify::EErrorIoctl);
    SetIoctlOutstanding(EFalse);
    }

/**
 *  Called by the CSmsReadParams object when a read all SMS parameter sets
 *  requested by this SAP has completed.
 *  
 *  @param aStatus the result of the read operation.
 *  @param aSmspList SMS parameter list
 *  
 */
void CSmsProvider::ReadSmsParamsCompleted(TInt aStatus, CMobilePhoneSmspList* aSmspList)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_READSMSPARAMSCOMPLETED_1, "CSmsProvider::ReadSmsParamsCompleted [aStatus=%d]", aStatus);
    
    TInt numNewSegments=0;
    
    if( aStatus == KErrNone )
        {
        // Attempt to serial the parameters to the receive buffer & notify
        // the socket of the new data
        __ASSERT_DEBUG(aSmspList != NULL,SmspPanic(KSmspPanicParameterBufferNull));
        TRAP(aStatus,(numNewSegments=ExternalizeParametersL(*aSmspList)));
        }
    if( aStatus == KErrNone )
        {
        iSocket->NewData(numNewSegments);
        TPckgBuf<TInt> buf;
        buf()=1;
        iSocket->IoctlComplete(&buf);
        }
    else
        {
        iSocket->Error(aStatus,MSocketNotify::EErrorIoctl);
        }
    SetIoctlOutstanding(EFalse);
    }

/**
 *  Called by the CSmsWriteParams object when a request to write SMS
 *  parameters to a phone store by this SAP has completed.
 *  
 *  @param aStatus the result of the write operation.
 *  
 */
void CSmsProvider::WriteSmsParamsCompleted(TInt aStatus)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_WRITESMSPARAMSCOMPLETED_1, "CSmsProvider::WriteSmsParamsCompleted [aStatus=%d]", aStatus);
    iSocket->Error(aStatus,MSocketNotify::EErrorIoctl);
    SetIoctlOutstanding(EFalse);
    }

/**
 *  Internal function called after a phone store enumeration request to
 *  serialize the messages to the receive buffer.
 *  
 *  @leave Leaves if any individual ExternalizeMessageL() call leaves.
 *  @return the total number of segments the messages were split into.
 *  
 */
TInt CSmsProvider::ExternalizeEnumeratedMessagesL(TInt& aCount)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_EXTERNALIZEENUMERATEDMESSAGESL_1, "CSmsProvider::ExternalizeEnumeratedMessagesL");
    
    TInt numnewsegments(0);
    numnewsegments=iProtocol.ExternalizeEnumeratedMessagesL(*this,aCount);
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_EXTERNALIZEENUMERATEDMESSAGESL_2, "-> CSmsProvider::ExternalizeEnumeratedMessagesL - done");
    
    return numnewsegments;
    }

/**
 *  Internal function used to serialize a message into the receive buffer.
 *  Each serialized message is split into smaller segments which form the
 *  basic unit of data passed back to the socket server in the GetData() method.
 *  
 *  @param aSmsMessage the message to serialize.
 *  @param aAppend specifies whether the message is inserted at the start or end of the buffer.
 *  @leave Leaves if the message could not be serialized or inserted into the buffer.
 *  @return the number of segments the message was split into.
 *  
 */
TInt CSmsProvider::ExternalizeMessageL(const CSmsMessage& aSmsMessage,TBool aAppend)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_EXTERNALIZEMESSAGEL_1, "CSmsProvider::ExternalizeMessageL()");
    
    // Create a new segmented buffer for the serialization of this message
    CBufSeg* recvbufseg = CBufSeg::NewL(KSmsMaxSegmentLength);
    CleanupStack::PushL(recvbufseg);
    
    // Attempt to serialize this message into the buffer
    RBufWriteStream writestream(*recvbufseg);
    writestream.Open(*recvbufseg);
    CleanupClosePushL(writestream);
    writestream << aSmsMessage;
    
    // Append / insert this buffer at the end / start of the other serialized message buffers
    if( aAppend )
        {
        iRecvBufSegArray->AppendL(recvbufseg);
        }
    else
        {
        iRecvBufSegArray->InsertL(0,recvbufseg);
        }
    CleanupStack::PopAndDestroy();  //  writestream
    CleanupStack::Pop();            //  recvbufseg
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_EXTERNALIZEMESSAGEL_2, "-> CSmsProvider::ExternalizeMessageL - done");
    
    return NumSegments(recvbufseg->Size());
    }

/**
 *  Internal function to deserialize a message from the send buffer.
 *  Each serialized message is split into smaller segments which form the basic
 *  unit of data passed to this SAP from the socket server in the Write() method.
 *  
 *  @leave Leaves if the message could not be de-serialized.
 *  @return the de-serialized CSmsMessage object.
 *  
 */
CSmsMessage* CSmsProvider::InternalizeMessageL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_INTERNALIZEMESSAGEL_1, "CSmsProvider::InternalizeMessageL()");
    
    // Initialize the read stream with the buffer
    RBufReadStream readstream(*iSendBufSeg);
    readstream.Open(*iSendBufSeg,0);
    CleanupClosePushL(readstream);

    // Create a buffer and message to store the result
    CSmsBufferBase* buffer = CSmsBuffer::NewL();
    CSmsMessage* smsmessage= CSmsMessage::NewL(iProtocol.FileSession(), CSmsPDU::ESmsDeliver,buffer);
    CleanupStack::PushL(smsmessage);

    // De-serialize the message from using the read stream
    readstream >> *smsmessage;

    CleanupStack::Pop();  //  smsmessage
    CleanupStack::PopAndDestroy();  //  readstream
    iSendBufSeg->Reset();

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_INTERNALIZEMESSAGEL_2, "-> CSmsProvider::InternalizeMessageL - done");
    
    return smsmessage;
    }

/**
 *  Internal function used to serialize SMS parameters into the receive buffer.
 *  Each serialized parameter object is split into smaller segments which form the
 *  basic unit of data passed back to the socket server in the GetData() method.
 *  
 *  @param aMobilePhoneSmspList the parameters to serialize.
 *  @leave Leaves if the parameters could not be serialized or inserted into the buffer.
 *  @return the number of segments the parameters was split into.
 *  
 */
TInt CSmsProvider::ExternalizeParametersL(const CMobilePhoneSmspList& aMobilePhoneSmspList)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_EXTERNALIZEPARAMETERSL_1, "CSmsProvider::ExternalizeParametersL");
    
    // Create a new segmented buffer for the serialization of this message
    CBufSeg* recvBufSeg = CBufSeg::NewL(KSmsMaxSegmentLength);
    CleanupStack::PushL(recvBufSeg);

    // Attempt to serialize this message into the buffer
    RBufWriteStream writeStream(*recvBufSeg);
    writeStream.Open(*recvBufSeg);
    CleanupClosePushL(writeStream);
    writeStream << aMobilePhoneSmspList;
    writeStream.CommitL();

    // Append start of the other serialized message buffers
    CleanupStack::PopAndDestroy();	//writeStream
    iRecvBufSegArray->InsertL(0,recvBufSeg);
    CleanupStack::Pop(recvBufSeg);

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_EXTERNALIZEPARAMETERSL_2, "-> CSmsProvider::ExternalizeParametersL - done");
    
    return NumSegments(recvBufSeg->Size());
    }

/**
 *  Internal function to retrieve a SMS parameters from the send buffer.
 *  Each serialized object is split into smaller segments which form the basic
 *  unit of data passed to this SAP from the socket server in the Write() method.
 *  
 *  @leave Leaves if the parameters could not retrieved.
 *  @return the retrieved CMobilePhoneSmspList object.
 *  
 */
CMobilePhoneSmspList* CSmsProvider::InternalizeParametersL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_INTERNALIZEPARAMETERSL_1, "CSmsProvider::InternalizeParametersL");
    
    // Initialize the read stream with the buffer
    RBufReadStream readStream(*iSendBufSeg);
    readStream.Open(*iSendBufSeg,0);
    CleanupClosePushL(readStream);

    // Create a parameter object to store the result
    CMobilePhoneSmspList* mobilePhoneSmspList = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(mobilePhoneSmspList);

    // De-serialize the message from using the read stream
    readStream >> *mobilePhoneSmspList;

    CleanupStack::Pop(mobilePhoneSmspList);
    CleanupStack::PopAndDestroy();	//readStream
    iSendBufSeg->Reset();

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_INTERNALIZEPARAMETERSL_2, "-> CSmsProvider::InternalizeParametersL - done");
    
    return mobilePhoneSmspList;
    }

CSmsMessage* CSmsProvider::InternalizeMessageL(	CBufSeg* aBufSeg)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_INTERNALIZEMESSAGEL1_1, "CSmsProvider::InternalizeMessageL");
    
    RBufReadStream readstream(*aBufSeg);
    readstream.Open(*aBufSeg,0);
    CleanupClosePushL(readstream);
    CSmsBufferBase* buffer = CSmsBuffer::NewL();
    CSmsMessage* smsmessage= CSmsMessage::NewL(iProtocol.FileSession(),CSmsPDU::ESmsDeliver,buffer);
    
    CleanupStack::PushL(smsmessage);
    readstream >> *smsmessage;
    
    CleanupStack::Pop();  //  smsmessage
    CleanupStack::PopAndDestroy();  //  readsream
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_INTERNALIZEMESSAGEL1_2, "-> CSmsProvider::InternalizeMessageL - done");
    
    return smsmessage;
    }

TInt CSmsProvider::SecurityCheck(MProvdSecurityChecker* aSecurityChecker)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROVIDER_SECURITYCHECK_1, "CSmsProvider::SecurityCheck");
    iSecurityChecker = aSecurityChecker;
    return KErrNone;
    }
