// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __SS_NODEMESSAGES_INTERNAL_H__
#define __SS_NODEMESSAGES_INTERNAL_H__

#include <comms-infras/ss_nodemessages.h>
#include <es_enum.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_partner.h>
#endif

#include <elements/nm_address.h>
#include <elements/nm_messages_base.h>
#include <comms-infras/ss_common.h>
#include <comms-infras/ss_logext.h>

class CSockManData; //must be seen in global namespace before being a friend in ESock namespace
//otherwise silly compiler confused since it interprets friend class as a forward declaration
//for no reason

class CParameterBundleBase;

namespace Messages
{
	class RNodeInterface;
}

namespace ESock
{
class MPlatsecApiExt;
class CCommsFactoryBase;

class CMetaConnectionProviderFactoryBase;


struct TFlowParams
/** Conveys attributes of a flow request
*/
	{
	enum TFlowRequestType
		{
		EImplicit, EExplicitSubConnection, EExplicitConnection
		};

	explicit TFlowParams(
		TInt aAddrFamily,
		TInt aSocketType,
		TInt aProtocol,
		TInt aFlowRequestType,
		TAny* aSessionProxy,
		TBool aFlowLoadsProtocol = ETrue
		) :
			iAddrFamily(aAddrFamily),
			iSocketType(aSocketType),
			iProtocol(aProtocol),
			iFlowRequestType(aFlowRequestType),
			iSessionProxy(aSessionProxy),
			iFlowLoadsProtocol(aFlowLoadsProtocol) {}

	explicit TFlowParams() :
		iAddrFamily(NULL), iSocketType(NULL), iProtocol(NULL),
		iFlowRequestType(NULL), iSessionProxy(NULL), iFlowLoadsProtocol(ETrue) {}

	TInt iAddrFamily;
	TInt iProtocol;
	TInt iSocketType;
	TInt iFlowRequestType;
	TAny* iSessionProxy;
	TBool iFlowLoadsProtocol;
	};


/**
@internalTechnology
Special Meta Type to increase refcount on RMetaExtensionContainers when they are posted.
Relies on fact that one serialisation will occur per post.
*/
NONSHARABLE_CLASS(TMetaExtensionContainerMetaType) : public Meta::MMetaType
	{
public:
	inline static Meta::MMetaType* NewL(const TAny* aMem, const TAny* aData)
		{
		return ::new ((TUint8*)aMem) TMetaExtensionContainerMetaType(reinterpret_cast<const RMetaExtensionContainerC*>(aData));
		}

	inline virtual TInt Load(TPtrC8& aBuffer)
		{
		TInt len = Length();
		if (aBuffer.Length() < len)
			{
			return KErrArgument;
			}
		Mem::Copy(const_cast<RMetaExtensionContainerC*>(iData), aBuffer.Ptr(), len);
		aBuffer.Set(aBuffer.Ptr()+len, aBuffer.Length()-len); //update pointer
		return KErrNone;
		}

	inline virtual TInt Store(TDes8& aBuffer) const
		{
		const_cast<RMetaExtensionContainerC*>(iData)->Open();
		TInt len = Length();
		if (aBuffer.MaxLength() - aBuffer.Length() < len)
			{
			return KErrOverflow;
			}
		aBuffer.Append((TUint8*)iData, len);
		return KErrNone;
		}

	inline virtual void Copy(const TAny* aData)
		{
		Mem::Copy((TAny*)iData, aData, Length());
		}

	inline virtual TInt Length() const
		{
		return sizeof(RMetaExtensionContainerC);
		}

private:
	inline TMetaExtensionContainerMetaType::TMetaExtensionContainerMetaType(const RMetaExtensionContainerC* aData)
		: iData(aData)
		{
		__ASSERT_DEBUG(iData!=NULL, User::Panic(_L("RMECMeta"),KErrArgument));
		}

private:
	const RMetaExtensionContainerC* iData;
	};

//
// Signature classes
//

DECLARE_MESSAGE_SIG_1(SigConnectionInfo, TConnectionInfo, ConnectionInfo)
DECLARE_MESSAGE_SIG_1(SigSubConNotification, CRefCountOwnedSubConNotification*, RefCountOwnedSubConNotification)
DECLARE_MESSAGE_SIG_1(SigPlaneNotification, CRefCountOwnedNotification*, RefCountOwnedNotification)
DECLARE_MESSAGE_SIG_1(SigSubConnOpenedEvent, TSubConnectionOpenedEvent, SubConnectionOpenedEvent)
DECLARE_MESSAGE_SIG_1(SigSubConnClosedEvent, TSubConnectionClosedEvent, SubConnectionClosedEvent)
DECLARE_MESSAGE_SIG_1(SigStateChange, Elements::TStateChange, StateChange)
DECLARE_MESSAGE_SIG_1(SigFlowParams, TFlowParams, FlowParams)

class CConnectionInfo;
DECLARE_MESSAGE_SIG_1(SigConnectionInfoPtr, ESock::CConnectionInfo*, Ptr)

#ifdef SYMBIAN_NETWORKING_UPS
struct TPolicyCheckRequestParams
/**
Parameters passed in a TPolicyCheckRequest message
*/
	{
	inline TPolicyCheckRequestParams()
		{ };

	inline TPolicyCheckRequestParams(const TProcessId& aProcessId, const TThreadId& aThreadId, TInt aPlatSecResult,
									 const TUpsDestinationAddr& aDestinationAddr, const TUpsDestinationAddrType& aDestinationAddrType)
	  : iProcessId(aProcessId), iThreadId(aThreadId), iPlatSecResult(aPlatSecResult),
	    iDestinationAddr(aDestinationAddr), iDestinationAddrType(aDestinationAddrType),
	    iCommsId(Messages::TNodeId::NullId())
	    
		{ };

	inline TPolicyCheckRequestParams(const TProcessId& aProcessId, const TThreadId& aThreadId, TInt aPlatSecResult,
									 const TUpsDestinationAddr& aDestinationAddr, const TUpsDestinationAddrType& aDestinationAddrType, const Messages::TNodeId& aCommsId)
		  : iProcessId(aProcessId), iThreadId(aThreadId), iPlatSecResult(aPlatSecResult),
		    iDestinationAddr(aDestinationAddr), iDestinationAddrType(aDestinationAddrType), iCommsId(aCommsId)
			{ };	    
	    
	TProcessId				iProcessId;
	TThreadId				iThreadId;
	TInt					iPlatSecResult;			// result of Platform Security policy check
	TUpsDestinationAddr		iDestinationAddr;		// destination (e.g. IP address where available)
	TUpsDestinationAddrType	iDestinationAddrType;	// form of iDestinationAddr (e.g. string, IP address etc)
	Messages::TNodeId 		iCommsId;
	};

DECLARE_MESSAGE_SIG_1(SigUpsParameters, TPolicyCheckRequestParams, Params)

struct TUPSStatusChangeParams
/**
Parameters passed in a UPS Status Change message
*/
	{
	inline TUPSStatusChangeParams()
		{ };
	
	inline TUPSStatusChangeParams(const TProcessId& aProcessId, const TThreadId& aThreadId, const Messages::TNodeId& aCommsId)
		  : iProcessId(aProcessId), iThreadId(aThreadId), iCommsId(aCommsId)
			{ };	    
	    
	TProcessId				iProcessId;
	TThreadId				iThreadId;
	Messages::TNodeId 		iCommsId;
	};

DECLARE_MESSAGE_SIG_1(SigUpsStatusChangeParameters, TUPSStatusChangeParams, Params)
#endif

template<TInt id, TInt32 realm> struct TMessageSigSelectionPrefs;
template<TInt id, TInt32 realm> struct TCFMessageSigConnPrefList;
template<TInt id, TInt32 realm> struct TMessageSigSelectComplete;
template<TInt id, TInt32 realm> struct TCFMessageTransportHookError;


//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TCFMessage
	{
  private:
  	enum
 	{
	ECFSubConnDataTransferred		= 94, //email details to remek including comparison of conn and subconn
	EStateChange                    = 95,
	
	ECFTransportNotification		= 100,  // For SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	
    ECFProvisionConnectionInfo      = 101,
	ECFDestroyOrphans               = 102
	};
protected:
	friend class ::CSockManData;

/*
 * Utility functions to register/deregister internal transport messages
 * @internalComponent
 * @released 9.3
 */
	static void RegisterL();
	static void DeRegister();

public:
    enum { ERealmId = 0x10285A57 }; //UID allocated on 21/11/2007
    /*==========================================================================
    <<Keys:>>
    [Name]:      <messagename>
    [Semantics]: Message semantics

    [Type]:      Request for <responsemsg>|Response to <requestmsg>|Notification/[Peerless]
                 Where:
                 Request      - request to perform an action. Sender should expect <responsemsg>
                                to ackowledge the completion of the action.
                 Response     - acknowledgement that the previous request is now completed.
                 Notification - a one-off message flagging an event.
                 Peerless     - the message is peerless, i.e.: the sender doesn't have to
                                be the recipient's client.
    [Fallible]:  Yes|No
                 Only applicable to requests and denotes whether the request can fail or not,
                 or, in other words, whether the request can be completed to with TError.

    [Structure]:
    Every message defined in this realm has at least:
    param        iSender      - the message sender
    param        iActivityId  - depending on the message type:
                 Request      - the activity id the response should be addressed to.
                                Request senders fill up this param to match the response
                                with the local activity. Activity id on the sender side
                                should uniquelly identify the activity expecting the response.
                 Response     - the activity on the recipient side expecting this response.
                                the activityid to put here has been previously conveyed
                                with the request.
                 Notification - ignored.
    ===========================================================================*/
    //[399TODO] RZ: the list of messages that follow is pretty long. I was then thinking
    //that perhaps we could chop it (with namespaces) into conceptual interfaces. Even better,
    //perhaps we could even make some runtime checks on message reception to panic if a message
    //received doesn't belong to any of the interfaces implemented locally.

//--Generic framework--
    //typedef Messages::TMessageSigVoid<ECFNull, TCFMessage::ERealmId> TNull;

   /****************************************************************************
    **
    ****************************************************************************
    **
    **
    **
    **
    **
    **
    ** INTERFACE: Unasigned
    **
    **
    **
    **
    **
    **
    ****************************************************************************
    **
    ****************************************************************************/

    /*==========================================================================
    [Name]:      TSubConnDataTransferred
    [Semantics]: [399TODO] RZ: Simon please finish.

    [Type]:
    [Fallible]:

    [Structure]:
    ===========================================================================*/
	typedef Messages::TMessageSigNumberNumberNumber<ECFSubConnDataTransferred, TCFMessage::ERealmId> TSubConnDataTransferred;

	/*==========================================================================
	[Name]:      TStateChange
	[Semantics]: Sender flags its state change. The state represents the legacy concept of a progress
			and is only sent during connection start or stop.

	[Type]:      Notification
	===========================================================================*/
	typedef TMessageSigStateChange<EStateChange, TCFMessage::ERealmId> TStateChange;

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	/*==========================================================================
	[Name]:      TTransportNotification
	[Semantics]: Sender sends this message to Data clients which will have a notification.
			Presently used to notify the data clients for the change in underlying bearer
			which will enable the SAP to used set the TCP receive window size and Maximum
			window size (which will be used for scaling negotiation). The window sizes will
			be retrieved using the pointer to the Lookup Table.

	[Type]:      Notification
	
	===========================================================================*/
	typedef Messages::TMessageSigVoid <ECFTransportNotification, TCFMessage::ERealmId> TTransportNotification;

#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

	   /**
	    The message provisions connection information during the selection request

	    @li Type:  Notification

	    IMPORTANT: There is no response to TProvisionConnectionInfo. If the recipient finds out
	    that the configuration received is in any way inadequate, it cannot flag this
	    fact until it is requested to do something (e.g.: TCFDataClient::Start)
	    */
	    typedef TMessageSigConnectionInfoPtr<ECFProvisionConnectionInfo, TCFMessage::ERealmId> TProvisionConnectionInfo;

	  typedef Messages::TMessageSigVoid <ECFDestroyOrphans, TCFMessage::ERealmId> TDestroyOrphans;
	};
} //namespace ESock

#endif
// __SS_NODEMESSAGES_INTERNAL_H__

