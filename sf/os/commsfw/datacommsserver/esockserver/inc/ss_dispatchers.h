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
 @internalTechnology
*/

#ifndef SS_DISPATCHERS_H
#define SS_DISPATCHERS_H

#include <elements/nm_node.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_common.h>

namespace ESock
{

//in-process,in/cross-thread node extension message
struct TCFItfExt
    {
protected:
	explicit TCFItfExt()
		{
		}

	IMPORT_C void ForwardMessageL(const Messages::TRuntimeCtxId& aSender,
	                              Messages::TSignalBase& aMessage,
	                              Messages::ANode& aNode,
	                              const Messages::TClientType& aClientType,
	                              const Messages::TClientType& aClientTypeExclude = Messages::TClientType(0, Messages::TClientType::ELeaving));
    IMPORT_C TAny* FetchInterfaceL(Messages::ANode& aNode, TInt aInterface);
	IMPORT_C TAny* FetchInterfaceOrForwardMsgL(const Messages::TRuntimeCtxId& aSender,
	                                           Messages::TSignalBase& aMessage,
	                                           Messages::ANode& aNode, TInt aInterface,
	                                           const Messages::TClientType& aClientType,
	                                           const Messages::TClientType& aClientTypeExclude = Messages::TClientType(0, Messages::TClientType::ELeaving));
	IMPORT_C NetInterfaces::TInterfaceControl* FetchInterfaceControlOrForwardMsgL(const Messages::TRuntimeCtxId& aSender,
	                                                                              Messages::TSignalBase& aMessage,
	                                                                              Messages::ANode& aNode,
																				  TInt aInterface,
									  		   		  							  const Messages::TClientType& aClientType,
									  		   		  							  const Messages::TClientType& aClientTypeExclude = Messages::TClientType(0, Messages::TClientType::ELeaving));
    };

template <TUint32 CLIENT_TYPE, TUint32 CLIENT_FLAGS = 0, TUint32 CLIENT_TYPE_EXCLUDE = 0, TUint32 CLIENT_FLAGS_EXCLUDE = Messages::TClientType::ELeaving>
struct TCFClientItfExt : protected TCFItfExt
    {
protected:
	explicit TCFClientItfExt()
	:	TCFItfExt()
		{
		}

	inline void ForwardMessageL(const Messages::TRuntimeCtxId& aSender, Messages::TSignalBase& aMessage, Messages::ANode& aNode)
		{
		TCFItfExt::ForwardMessageL(aSender, aMessage, aNode, Messages::TClientType(CLIENT_TYPE,CLIENT_FLAGS), Messages::TClientType(CLIENT_TYPE_EXCLUDE,CLIENT_FLAGS_EXCLUDE));
		}

	inline TAny* FetchInterfaceOrForwardMsgL(const Messages::TRuntimeCtxId& aSender, Messages::TSignalBase& aMessage, Messages::ANode& aNode, TInt aInterface)
		{
		return TCFItfExt::FetchInterfaceOrForwardMsgL(aSender, aMessage, aNode, aInterface, Messages::TClientType(CLIENT_TYPE,CLIENT_FLAGS), Messages::TClientType(CLIENT_TYPE_EXCLUDE,CLIENT_FLAGS_EXCLUDE));
		}

	inline NetInterfaces::TInterfaceControl* FetchInterfaceControlOrForwardMsgL(const Messages::TRuntimeCtxId& aSender, Messages::TSignalBase& aMessage, Messages::ANode& aNode, TInt aInterface)
		{
		return TCFItfExt::FetchInterfaceControlOrForwardMsgL(aSender, aMessage, aNode, aInterface, Messages::TClientType(CLIENT_TYPE,CLIENT_FLAGS), Messages::TClientType(CLIENT_TYPE_EXCLUDE,CLIENT_FLAGS_EXCLUDE));
		}
    };

//TCFClientItfExt and its hierarchy is supposed to have 0 lenght (when merged into a hierarchy that is).
//There are several functions taking (for example) TCFSigRMessage2Ext as Messages::TSignalBase
//which explicitly work with this assumtion. If considering adding data members to TCFClientItfExt
//hierarchy, find and fix all of these functions.
class TCFClientItfExtSizeTest : public Messages::TSignalBase, public TCFItfExt {};
__ASSERT_COMPILE(sizeof(TCFClientItfExtSizeTest)==sizeof(Messages::TSignalBase));

/**
	Template to assemble a function dispatching message suitable for use as a MESSAGE
	template parameter of the TCFMessageDispatcher template.
*/
template <TInt INTERFACE_ID, class INTERFACE, typename PARAM_STRUCT,
          TUint32 CLIENT_TYPE, TUint32 CLIENT_FLAGS = 0, TUint32 CLIENT_TYPE_EXCLUDE = 0, TUint32 CLIENT_FLAGS_EXCLUDE = 0>
struct TCFFunctionDispatcher : public TCFClientItfExt<CLIENT_TYPE, CLIENT_FLAGS, CLIENT_TYPE_EXCLUDE, CLIENT_FLAGS_EXCLUDE>
    {
protected:
    typedef void (INTERFACE::*TFunction)(PARAM_STRUCT&);

public:
    explicit TCFFunctionDispatcher()
    :	TCFClientItfExt<CLIENT_TYPE, CLIENT_FLAGS, CLIENT_TYPE_EXCLUDE, CLIENT_FLAGS_EXCLUDE>()
        {
        }

	explicit TCFFunctionDispatcher(TFunction aFunction, const PARAM_STRUCT& aParams)
	:	TCFClientItfExt<CLIENT_TYPE, CLIENT_FLAGS, CLIENT_TYPE_EXCLUDE, CLIENT_FLAGS_EXCLUDE>(),
		iParam(aParams),
		iFunction(aFunction)
		{
		}

protected:
	void DispatchL(Messages::TSignalBase& aThisDispatcher, Messages::ANode* aItf, TPtrC8& /* aParams */)
        {
        INTERFACE* itf = reinterpret_cast<INTERFACE*>(TCFClientItfExt<CLIENT_TYPE, CLIENT_FLAGS, CLIENT_TYPE_EXCLUDE, CLIENT_FLAGS_EXCLUDE>::FetchInterfaceOrForwardMsgL(aThisDispatcher, *aItf, INTERFACE_ID));
        if (itf)
            {
            (itf->*iFunction)(iParam);
            }
        }
    PARAM_STRUCT iParam;
    TFunction iFunction;
    };

/* Example of using TCFFunctionDispatcher in combination with TCFMessageDispatcher
Interface Id must be in api_ext_list.h enum TInt.

Let's assume it's EApiExtSomeInterface

*.h file would contain

// The extension interface declaration (functions must follow the signature pattern given by TCFFunctionDispatcher)
class MSomeExtensionInterface
    {
    public:
    virtual Fn1(TSomeParametersNotMemoryAllocated& aParam) = 0;
    virtual Fn2(TSomeOtherParametersNotMemoryAllocated& aParam) = 0;
    };

// The individual structures
struct TSomeParametersNotMemoryAllocated
    {
    TUint iInt;
    TBuf8<20> iBuf;
    };

struct TSomeOtherParametersNotMemoryAllocated
    {
    };

// Declaration of dispatchers
typedef TCFFunctionDispatcher<EApiExtSomeInterface, MSomeExtensionInterface, TSomeParametersNotMemoryAllocated,
                        RNodeInterface::ECtrl> TFDSomeParametersNotMemoryAllocated;

typedef TCFMessageDispatcher<ESomeMessageId, TFDSomeParametersNotMemoryAllocated, Messages::ANode> TMDSomeParametersNotMemoryAllocated;

// There is a way how to get ESomeMessageId in case it needs to be unique amongst other messages

****.cpp file would contain

// The interface Uid
const TInt KSomeExtensionInterfaceUid = 0xDEADBEEF

// The interface messages implementation table
const TImplementationProxy SomeParametersImplementationTable[] =
	{
	MVIP_CTR_ENTRY(ESomeParam1, TMDSomeParametersNotMemoryAllocated),
	MVIP_CTR_ENTRY(ESomeParam2, TMDSomeOtherParametersNotMemoryAllocated),
	};

// Constructors
DEFINE_MVIP_CTR(TMDSomeParametersNotMemoryAllocated);
DEFINE_MVIP_CTR(TMDSomeOtherParametersNotMemoryAllocated);

// Attribute table(s)
START_ATTRIBUTE_TABLE(TMDSomeParametersNotMemoryAllocated, KSomeExtensionInterfaceUid, ESomeParam1)
	REGISTER_ATTRIBUTE(TTMDSomeParametersNotMemoryAllocated, iInt, TMetaNumber)
	REGISTER_ATTRIBUTE(TMDSomeParametersNotMemoryAllocated, iBuf, TMeta<TBuf8<20>>)
END_ATTRIBUTE_TABLE_BASE(Messages::TSignalBase, 0)

START_ATTRIBUTE_TABLE(TMDSomeOtherParametersNotMemoryAllocated, KSomeExtensionInterfaceUid, ESomeParam2)
END_ATTRIBUTE_TABLE_BASE(Messages::TSignalBase, 0)

// The registration functions
void TSomeExtInterfaceMessageTables::RegisterL()
	{
	TlsGlobals().RegisterInterfaceL(TUid::Uid(KSomeExtensionInterfaceUid), sizeof(SomeParametersImplementationTable) / sizeof(SignatureImplementationTable[0]), SomeParametersImplementationTable);
	}

void TSomeExtInterfaceMessageTables::DeRegister()
	{
	TlsGlobals().DeregisterInterface(TUid::Uid(KSomeExtensionInterfaceUid));
	}

**/

//most common signature for messages originating on user side
class TCFSigRMessage2Ext : public Messages::TSignalBase, public TCFClientItfExt<TCFClientType::ECtrl>
    {
protected:
    TCFSigRMessage2Ext(const RMessage2& aMessage)
    :   TCFClientItfExt<TCFClientType::ECtrl>(),
        iMessage(aMessage)
            {
            }

    TCFSigRMessage2Ext()
    	{
    	}

protected:
	IMPORT_C virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

protected:
    RMessage2 iMessage;

public:
    EXPORT_DATA_VTABLE_AND_FN
    };

} //namespace ESock

#endif
// SS_DISPATCHERS_H





