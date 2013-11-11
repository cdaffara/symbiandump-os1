// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NODEMESSAGES_H
#define SYMBIAN_NODEMESSAGES_H

#define SYMBIAN_NETWORKING_UPS
#include <es_enum.h>
#include <elements/nm_signatures.h>
#include <elements/nm_address.h>
#include <elements/nm_messages_base.h>
#include <comms-infras/ss_common.h>
#include <comms-infras/ss_logext.h>
#include <es_sock.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_partner.h>
#endif


#include <comms-infras/metatype.h>
#include <elements/factory.h>

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
#include <comms-infras/es_event.h>
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_upsaccesspointconfigext.h>
#endif
#include <comms-infras/es_parameterfamily.h>
#include <comms-infras/ss_parameterfamilybundle.h>


#include <comms-infras/ss_refcountowner.h>

#include <comms-infras/ss_commsprov.h>

namespace ESock
{


/**
Factory query payload base class.
Note: The multiple inheritance is safe to do as the vptr size is always less than STypeID
size.

@publishedPartner
@released
*/
class TFactoryQueryBase : public Meta::SMetaDataNetCtor, public Factories::MFactoryQuery
	{
public:
	static const TInt KMaxInlineFactoryQuerySize = 48;
	};

/**
Attribute creation policy using Sockman globals.

@publishedPartner
@released
*/
class TVCFromSockManAttributeCreationPolicy
	{
public:
	IMPORT_C static Meta::SMetaData* New(TPtrC8& aSrc, TPtr8& aDst);
	};

class TAlwaysFindFactoryQuery : public TFactoryQueryBase
	{
protected:
	IMPORT_C virtual Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aObjectInfo);

public:
	IMPORT_C explicit TAlwaysFindFactoryQuery ();

	DECLARE_MVIP_CTR(TAlwaysFindFactoryQuery)
	EXPORT_DATA_VTABLE_AND_FN
	};

class TMetaConnectionFactoryQuery : public TFactoryQueryBase
	{
public:
	IMPORT_C TMetaConnectionFactoryQuery();

	IMPORT_C explicit TMetaConnectionFactoryQuery(const TProviderInfo& aProviderInfo, const TUid aTierImplUid);

protected:
	IMPORT_C virtual Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo);

public:
	//We have it by value here as we need the XMetaConnectionFactoryQuery object
	//to be sent in a message.
	TProviderInfo iProviderInfo;
	TUid iTierImplUid; // Tier Implementation Uid == Tier factory id

	DECLARE_MVIP_CTR(TMetaConnectionFactoryQuery)
	EXPORT_DATA_VTABLE_AND_FN
	};

class TTierTypeIdFactoryQuery : public TFactoryQueryBase
    {
public:
    IMPORT_C TTierTypeIdFactoryQuery();

    IMPORT_C explicit TTierTypeIdFactoryQuery(const TUid aTierTypeId);

protected:
    IMPORT_C virtual Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo);

public:
    TUid iTierTypeId;

    DECLARE_MVIP_CTR(TTierTypeIdFactoryQuery)
    EXPORT_DATA_VTABLE_AND_FN
    };

class TDefaultConnectionFactoryQuery : public TFactoryQueryBase
/** Class implementing MFactoryQuery to find a connection provider.

@publishedPartner
@released
*/
	{
public:
	IMPORT_C explicit TDefaultConnectionFactoryQuery(const Messages::TNodeId& aMCprId);


	IMPORT_C explicit TDefaultConnectionFactoryQuery();

protected:
	IMPORT_C virtual Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo);

public:
	const Messages::TNodeId iMCprId;
	DECLARE_MVIP_CTR(TDefaultConnectionFactoryQuery)
	EXPORT_DATA_VTABLE_AND_FN
	};

class TDefaultSCPRFactoryQuery : public TFactoryQueryBase
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C TDefaultSCPRFactoryQuery();

	//used to look up sub-connection from control side
	IMPORT_C TDefaultSCPRFactoryQuery(const Messages::TNodeId& aControlProvider, TSubConnOpen::TSubConnType aSCPRType);
protected:
	IMPORT_C virtual Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo);
public:
	Messages::TNodeId 					iControlProvider;
	TSubConnOpen::TSubConnType 			iSCPRType; //default/reserved

	DECLARE_MVIP_CTR(TDefaultSCPRFactoryQuery)
	EXPORT_DATA_VTABLE_AND_FN
	};

class TDefaultFlowFactoryQuery : public TFactoryQueryBase
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C TDefaultFlowFactoryQuery();

	IMPORT_C TDefaultFlowFactoryQuery(const Messages::TNodeId& aSCprId);
	IMPORT_C TDefaultFlowFactoryQuery(const Messages::TNodeId& aCprId, const Messages::TNodeId& aSCprId);
	IMPORT_C TDefaultFlowFactoryQuery(const Messages::TNodeId& aCprId, const Messages::TNodeId& aSCprId, Messages::TNodeSignal::TMessageId aMessageId, TInt aProtocolType);
protected:
	IMPORT_C virtual Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo);
public:
	Messages::TNodeId					iCprId;
	Messages::TNodeId					iSCprId;
	Messages::TNodeSignal::TMessageId 	iMessageId;
	TInt			iProtocolType;

	DECLARE_MVIP_CTR(TDefaultFlowFactoryQuery)
	EXPORT_DATA_VTABLE_AND_FN
	};

class TDefaultProtocolIntfFactoryQuery : public TFactoryQueryBase
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C TDefaultProtocolIntfFactoryQuery();
	IMPORT_C TDefaultProtocolIntfFactoryQuery(const Messages::TNodeId& aCprId);

protected:
	IMPORT_C virtual Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo);
public:
	Messages::TNodeId			iCprId;
	DECLARE_MVIP_CTR(TDefaultProtocolIntfFactoryQuery)
	EXPORT_DATA_VTABLE_AND_FN
	};


DECLARE_MESSAGE_SIG_1(SigFamilyBundle, RCFParameterFamilyBundleC, FamilyBundle)
DECLARE_MESSAGE_SIG_2(SigNumberFamilyBundle, TInt, Value, RCFParameterFamilyBundleC, FamilyBundle)
DECLARE_MESSAGE_SIG_3(SigPlayerRoleUidFactoryQuery, TInt, FactoryContainerType, TUid, Uid, ESock::TFactoryQueryBase*, Query)
DECLARE_MESSAGE_SIG_1(SigSubConnParams, CSubConParameterBundle*, SubConParameterBundle)
DECLARE_MESSAGE_SIG_2(SigNumberSubConnParams, TInt, Value, const CSubConParameterBundle*, SubConParameterBundle)
DECLARE_MESSAGE_SIG_2(SigBundleRMessage2, CRefCountOwnedParameterBundle*, Bundle, const RMessage2*, Message)
DECLARE_MESSAGE_SIG_1(SigBundle, CRefCountOwnedParameterBundle*, Bundle)
DECLARE_MESSAGE_SIG_3(SigNodeIdClientTypeUid, Messages::TNodeId, NodeId, Messages::TClientType, ClientType, TUid, Uid)
DECLARE_MESSAGE_SIG_3(SigNodeIdClientTypeNumber, Messages::TNodeId, NodeId, Messages::TClientType, ClientType, TUint, Value)


/**
Message signature to carry and own a reference on a RMetaExtensionContainerC
The reference will be automatically aquired upon explicit construction
The reference will be automatically released after dispatch or on error
The RMetaExtensionContainerC to be posted MUST be open else a panic will occur (in udeb)
*/
struct TSigMetaExtContainer : public Messages::TSignatureBase
    {
public:
    IMPORT_C virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
    IMPORT_C virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

protected:
    inline TSigMetaExtContainer() {}
    inline explicit TSigMetaExtContainer(const Messages::TNodeSignal::TMessageId& aMessageId, RMetaExtensionContainerC& aConfig)
        : Messages::TSignatureBase(aMessageId), iConfig(aConfig)
        {
        }
    
public:
    DECLARE_MVIP_CTR( TSigMetaExtContainer )
    EXPORT_DATA_VTABLE_AND_FN
    RMetaExtensionContainerC iConfig;
    };

template<TInt id, TInt32 realm>
class TMessageSigMetaExtContainer : public TSigMetaExtContainer, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
    explicit TMessageSigMetaExtContainer(RMetaExtensionContainerC& aConfig)
        : TSigMetaExtContainer(Messages::TNodeSignal::TMessageId(id, realm), aConfig)
        {
        }
    };



template<TInt id, TInt32 realm>
class TMessageSigFamilyBundleDefaultToNull : public TSigFamilyBundle, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
/**
Message to carry a RCFParameterFamilyBundle.
Can be instantiated with no parameters, in which case the null RCFParameterFamilyBundle is used
*/
    {
public:
	TMessageSigFamilyBundleDefaultToNull(const RCFParameterFamilyBundleC& aFamilyBundle = RCFParameterFamilyBundleC())
	:	TSigFamilyBundle(Messages::TNodeSignal::TMessageId(id, realm), aFamilyBundle)
		{}
    };

template<TInt id, TInt32 realm>
class TMessageSigNumberDefaultFamilyBundleDefaultToNull : public TSigNumberFamilyBundle, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
/**
Message to carry a Number and a RCFParameterFamilyBundle.
Can be instantiated with no parameters, in which case 0 and the null RCFParameterFamilyBundle are used.
*/
    {
public:
	TMessageSigNumberDefaultFamilyBundleDefaultToNull(TInt aValue = 0, const RCFParameterFamilyBundleC& aFamilyBundle = RCFParameterFamilyBundleC())
	:	TSigNumberFamilyBundle(Messages::TNodeSignal::TMessageId(id, realm), aValue, aFamilyBundle)
		{}
    };

template<TInt id, TInt32 realm>
class TCFMessageSigNodeIdClientTypeNumberDefault : public TSigNodeIdClientTypeNumber, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
	{
public:
	TCFMessageSigNodeIdClientTypeNumberDefault(Messages::TNodeId aNodeId, Messages::TClientType aClientType, TUint aValue = 0)
	:	TSigNodeIdClientTypeNumber(Messages::TNodeSignal::TMessageId(id, realm), aNodeId, aClientType, aValue)
		{}
	};

} //namespace ESock

namespace Meta
	{
	// Ugly hack: mingw does not evaulate macros early enough to pass as template arguments so we hard code the
	// value for the x86gcc platform.  A compile time assertion ensures that the value stays correct.
	#if defined(__GCCXML__) || defined(__X86GCC__)
		#define KSigPlayerRoleUidFactory_iQuery	20
	#else
		#define KSigPlayerRoleUidFactory_iQuery	( _FOFF(ESock::TSigPlayerRoleUidFactoryQuery, iQuery) )
	#endif

	__ASSERT_COMPILE( KSigPlayerRoleUidFactory_iQuery == _FOFF(ESock::TSigPlayerRoleUidFactoryQuery, iQuery) );

	typedef Messages::TSignatureWithPolymorphicPayloadMetaType<ESock::TSigPlayerRoleUidFactoryQuery, KSigPlayerRoleUidFactory_iQuery, ESock::TVCFromSockManAttributeCreationPolicy> TMetaHandler;
	}


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <addressupdate.h>
#endif

#endif //SYMBIAN_NODEMESSAGES_H

