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

/**
 @file
*/

#include "nm_signatures.h"
#include "nm_messages_base.h"
#include "nm_messages_errorrecovery.h"
#include "nm_node.h"
#include <elements/interfacetable.h>
#include <elements/metatype.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessMesC, "ElemNodeMessMesC");
#endif

using namespace Messages;
using namespace Elements;
using namespace Meta;

//We can safely reuse this UID for signature's TIDs (but they mean different things)
static const TInt KNodeMessagesImplementationUid = TEBase::ERealmId;

// --- TSignatureBase ---
EXPORT_C TSignatureBase::TSignatureBase()
	{
	}

EXPORT_C TSignatureBase::TSignatureBase(const TNodeSignal::TMessageId& aMessageId)
:	TNodeSignal(aMessageId)
	{
	}

EXPORT_C void TSignatureBase::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
	{
	const TNodeId& nodeId = address_cast<TNodeId>(aRecipient); //This message type operates on nodes
	__ASSERT_DEBUG(nodeId.Ptr(), User::Panic(KSpecAssert_ElemNodeMessMesC, 1));
	__ASSERT_ALWAYS(nodeId.Node().NodeId() == nodeId, User::Panic(KMessagesPanic, EDeceasedNodePanic)); //Address reuse protection (Ptr() may match something existing, but not necesserily the intended addresee).
	nodeId.Node().ReceivedL(aSender, nodeId, *this);
	}

EXPORT_C void TSignatureBase::Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, TInt aError)
    {
    NM_LOG((KNodeMessagesSubTag, _L8("TSignatureBase %08x:\tError(%d)"), this, aError));
    if (!aSender.IsNull())
        {
        NM_LOG((KNodeMessagesSubTag, _L8("Sending TError")));
        RClientInterface::OpenPostMessageClose(aRecipient, aSender, TEBase::TError(MessageId(), aError).CRef());
        }
    }


enum EMessageTypeId //message signatures only (NOT messages) messages are declared under class TMessage
	{
	ESignalBase                          =0,
	ENodeSignal                          =1,

	ESignatureBase                       =2,
	ESignatureNumber                     =3,
	ESignatureUid                        =4,
	ESignatureErrContext                 =5,
	ESignatureErrResponse                =6,
	//empty								=7,
	ESignatureNodeId                     =8,
	ESignatureNodeIdNumber               =9,
	ESignatureNodeIdNumberNumber         =10,
	ESignatureMessageIdNumber            =11,
	ESignatureAnyNumber                  =12,
	ESignatureMetaDataPtr                =13, //the data itslef is not copied it stays owned by whoever allocated it
	                                          //unless agreed otherwise upon the actuall message
	ESignatureNumberNumberNumber         =14,
	ESignatureRMessage2                  =15,
	ESignatureNumberRMessage2            =16,
	ESignatureAny                        =17,
	ESignatureNumberNumber               =18,
	ESignatureNodeIdAny                  =19,
	ESignatureNodeIdNodeId               =20,
	ESignatureNodeIdNodeIdNumber         =21,
	ESignatureNumberNumberNumberNumber   =22,
	ESignatureUidNumber                  =23,
	ESignatureNumberUidNumber            =24,
	ESignatureNodeIdClientType           =25,
	ESignatureClientTypeUidClientType    =26,
	ESignatureClientType                 =27,
	ESignatureClientTypeUid              =28,
	ESignatureMessageId                  =29,
	ESignatureNodeIdNumberNumberNumber   =30,
	ESignatureTypeId                     =31,
	ESignatureTypeIdAnyNumber			 =32
	};




EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSignalBase, KNodeMessagesImplementationUid, ESignalBase )
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TNodeSignal, KNodeMessagesImplementationUid, ENodeSignal )
	REGISTER_ATTRIBUTE( TNodeSignal, iMessageId, TMeta<TNodeSignal::TMessageId> )
END_ATTRIBUTE_TABLE_BASE( TSignalBase, ESignalBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSignatureBase, KNodeMessagesImplementationUid, ESignatureBase )
END_ATTRIBUTE_TABLE_BASE( TNodeSignal, ENodeSignal )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigErrContext, KNodeMessagesImplementationUid, ESignatureErrContext )
	REGISTER_ATTRIBUTE( TSigErrContext, iErrContext, TMeta<TErrContext> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigErrResponse, KNodeMessagesImplementationUid, ESignatureErrResponse )
	REGISTER_ATTRIBUTE( TSigErrResponse, iErrResponse, TMeta<TErrResponse> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumber, KNodeMessagesImplementationUid, ESignatureNumber )
	REGISTER_ATTRIBUTE( TSigNumber, iValue, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigUid, KNodeMessagesImplementationUid, ESignatureUid )
	REGISTER_ATTRIBUTE( TSigUid, iUid, TMeta<TUid> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeId, KNodeMessagesImplementationUid, ESignatureNodeId )
	REGISTER_ATTRIBUTE( TSigNodeId, iNodeId, TMeta<TNodeId> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdNumber, KNodeMessagesImplementationUid, ESignatureNodeIdNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdNumber, iNodeId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdNumber, iValue, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdClientType, KNodeMessagesImplementationUid, ESignatureNodeIdClientType )
	REGISTER_ATTRIBUTE( TSigNodeIdClientType, iNodeId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdClientType, iClientType, TMeta<TClientType> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdNumberNumber, KNodeMessagesImplementationUid, ESignatureNodeIdNumberNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdNumberNumber, iNodeId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdNumberNumber, iValue1, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdNumberNumber, iValue2, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigMessageIdNumber, KNodeMessagesImplementationUid, ESignatureMessageIdNumber )
	REGISTER_ATTRIBUTE( TSigMessageIdNumber, iMsgId, TMeta<TNodeSignal::TMessageId> )
	REGISTER_ATTRIBUTE( TSigMessageIdNumber, iValue, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberNumberNumber, KNodeMessagesImplementationUid, ESignatureNumberNumberNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumberNumber, iValue1, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumberNumber, iValue2, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumberNumber, iValue3, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberNumberNumberNumber, KNodeMessagesImplementationUid, ESignatureNumberNumberNumberNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumberNumberNumber, iValue1, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumberNumberNumber, iValue2, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumberNumberNumber, iValue3, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumberNumberNumber, iValue4, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdNodeIdNumber, KNodeMessagesImplementationUid, ESignatureNodeIdNodeIdNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdNodeIdNumber, iNodeId1, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdNodeIdNumber, iNodeId2, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdNodeIdNumber, iValue, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigMetaDataPtr, KNodeMessagesImplementationUid, ESignatureMetaDataPtr )
	REGISTER_ATTRIBUTE( TSigMetaDataPtr, iPtr, TMeta<Meta::SMetaData*> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigRMessage2, KNodeMessagesImplementationUid, ESignatureRMessage2 )
	REGISTER_ATTRIBUTE( TSigRMessage2, iMessage, TMeta<RMessage2> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberRMessage2, KNodeMessagesImplementationUid, ESignatureNumberRMessage2 )
	REGISTER_ATTRIBUTE( TSigNumberRMessage2, iValue, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberRMessage2, iMessage, TMeta<RMessage2> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigAny, KNodeMessagesImplementationUid, ESignatureAny )
	REGISTER_ATTRIBUTE( TSigAny, iPtr, TMeta<TAny*> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigAnyNumber, KNodeMessagesImplementationUid, ESignatureAnyNumber )
	REGISTER_ATTRIBUTE( TSigAnyNumber, iPtr, TMeta<TAny*> )
	REGISTER_ATTRIBUTE( TSigAnyNumber, iValue, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigUidNumber, KNodeMessagesImplementationUid, ESignatureUidNumber )
	REGISTER_ATTRIBUTE( TSigUidNumber, iUid, TMeta<TUid> )
	REGISTER_ATTRIBUTE( TSigUidNumber, iValue, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberNumber, KNodeMessagesImplementationUid, ESignatureNumberNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumber, iValue1, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberNumber, iValue2, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdAny, KNodeMessagesImplementationUid, ESignatureNodeIdAny )
	REGISTER_ATTRIBUTE( TSigNodeIdAny, iNodeId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdAny, iPtr, TMeta<TAny*> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdNodeId, KNodeMessagesImplementationUid, ESignatureNodeIdNodeId )
	REGISTER_ATTRIBUTE( TSigNodeIdNodeId, iNodeId1, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdNodeId, iNodeId2, TMeta<TNodeId> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberUidNumber, KNodeMessagesImplementationUid, ESignatureNumberUidNumber )
	REGISTER_ATTRIBUTE( TSigNumberUidNumber, iValue1, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberUidNumber, iUid, TMeta<TUid> )
	REGISTER_ATTRIBUTE( TSigNumberUidNumber, iValue2, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigClientTypeUidClientType, KNodeMessagesImplementationUid, ESignatureClientTypeUidClientType )
	REGISTER_ATTRIBUTE( TSigClientTypeUidClientType, iClientType1, TMeta<TClientType> )
	REGISTER_ATTRIBUTE( TSigClientTypeUidClientType, iUid, TMeta<TUid> )
	REGISTER_ATTRIBUTE( TSigClientTypeUidClientType, iClientType2, TMeta<TClientType> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigClientType, KNodeMessagesImplementationUid, ESignatureClientType )
	REGISTER_ATTRIBUTE( TSigClientType, iClientType, TMeta<TClientType> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigClientTypeUid, KNodeMessagesImplementationUid, ESignatureClientTypeUid )
	REGISTER_ATTRIBUTE( TSigClientTypeUid, iClientType, TMeta<TClientType> )
	REGISTER_ATTRIBUTE( TSigClientTypeUid, iUid, TMeta<TUid> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigMessageId, KNodeMessagesImplementationUid, ESignatureMessageId )
	REGISTER_ATTRIBUTE( TSigMessageIdNumber, iMsgId, TMeta<TNodeSignal::TMessageId> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdNumberNumberNumber, KNodeMessagesImplementationUid, ESignatureNodeIdNumberNumberNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdNumberNumberNumber, iNodeId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdNumberNumberNumber, iValue1, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdNumberNumberNumber, iValue2, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdNumberNumberNumber, iValue3, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigTypeId, KNodeMessagesImplementationUid, ESignatureTypeId )
	REGISTER_ATTRIBUTE( TSigTypeId, iTypeId, TMeta<STypeId> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigTypeIdAnyNumber, KNodeMessagesImplementationUid, ESignatureTypeIdAnyNumber )
	REGISTER_ATTRIBUTE( TSigTypeIdAnyNumber, iTypeId, TMeta<Meta::STypeId> )
	REGISTER_ATTRIBUTE( TSigTypeIdAnyNumber, iPtr, TMeta<TAny*> )
	REGISTER_ATTRIBUTE( TSigTypeIdAnyNumber, iValue, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

DEFINE_MVIP_CTR(TSignatureBase);
DEFINE_MVIP_CTR(TSigErrContext);
DEFINE_MVIP_CTR(TSigErrResponse);
DEFINE_MVIP_CTR(TSigNumber);
DEFINE_MVIP_CTR(TSigUid);
DEFINE_MVIP_CTR(TSigNodeId);
DEFINE_MVIP_CTR(TSigNodeIdNumber);
DEFINE_MVIP_CTR(TSigNodeIdNumberNumber);
DEFINE_MVIP_CTR(TSigMessageIdNumber);
DEFINE_MVIP_CTR(TSigNumberNumberNumber);
DEFINE_MVIP_CTR(TSigNumberNumberNumberNumber);
DEFINE_MVIP_CTR(TSigNodeIdNodeIdNumber);
DEFINE_MVIP_CTR(TSigMetaDataPtr)
DEFINE_MVIP_CTR(TSigRMessage2)
DEFINE_MVIP_CTR(TSigNumberRMessage2)
DEFINE_MVIP_CTR(TSigAny)
DEFINE_MVIP_CTR(TSigAnyNumber)
DEFINE_MVIP_CTR(TSigUidNumber)
DEFINE_MVIP_CTR(TSigNumberNumber)
DEFINE_MVIP_CTR(TSigNodeIdAny)
DEFINE_MVIP_CTR(TSigNodeIdNodeId)
DEFINE_MVIP_CTR(TSigNumberUidNumber)
DEFINE_MVIP_CTR(TSigNodeIdClientType)
DEFINE_MVIP_CTR(TSigClientTypeUidClientType)
DEFINE_MVIP_CTR(TSigClientType)
DEFINE_MVIP_CTR(TSigClientTypeUid)
DEFINE_MVIP_CTR(TSigMessageId);
DEFINE_MVIP_CTR(TSigNodeIdNumberNumberNumber);
DEFINE_MVIP_CTR(TSigTypeId);
DEFINE_MVIP_CTR(TSigTypeIdAnyNumber);

const TImplementationProxy SignatureImplementationTable[] =
	{
	//NOTE: Entries must be sorted for the binary search to work efficiently!
	MVIP_CTR_ENTRY(ESignatureBase,TSignatureBase),							//2
	MVIP_CTR_ENTRY(ESignatureNumber,TSigNumber),							//3
	MVIP_CTR_ENTRY(ESignatureUid,TSigUid),									//4
	MVIP_CTR_ENTRY(ESignatureErrContext,TSigErrContext),					//5
	MVIP_CTR_ENTRY(ESignatureErrResponse,TSigErrResponse),					//6
//empty slot
	MVIP_CTR_ENTRY(ESignatureNodeId,TSigNodeId),							//8
	MVIP_CTR_ENTRY(ESignatureNodeIdNumber,TSigNodeIdNumber),				//9
	MVIP_CTR_ENTRY(ESignatureNodeIdNumberNumber,TSigNodeIdNumberNumber),	//10
	MVIP_CTR_ENTRY(ESignatureMessageIdNumber,TSigMessageIdNumber),	    	//11
	MVIP_CTR_ENTRY(ESignatureAnyNumber,TSigAnyNumber),						//12
	MVIP_CTR_ENTRY(ESignatureMetaDataPtr,TSigMetaDataPtr),					//13
	MVIP_CTR_ENTRY(ESignatureNumberNumberNumber,TSigNumberNumberNumber),	//14
	MVIP_CTR_ENTRY(ESignatureRMessage2,TSigRMessage2),						//15
	MVIP_CTR_ENTRY(ESignatureNumberRMessage2,TSigNumberRMessage2),			//16
	MVIP_CTR_ENTRY(ESignatureAny,TSigAny),									//17
	MVIP_CTR_ENTRY(ESignatureNumberNumber,TSigNumberNumber),				//18
	MVIP_CTR_ENTRY(ESignatureNodeIdAny,TSigNodeIdAny),						//19
	MVIP_CTR_ENTRY(ESignatureNodeIdNodeId,TSigNodeIdNodeId),				//20
	MVIP_CTR_ENTRY(ESignatureNodeIdNodeIdNumber,TSigNodeIdNodeIdNumber),	//21
	MVIP_CTR_ENTRY(ESignatureNumberNumberNumberNumber,TSigNumberNumberNumberNumber),//22
	MVIP_CTR_ENTRY(ESignatureUidNumber,TSigUidNumber),						//23
	MVIP_CTR_ENTRY(ESignatureNumberUidNumber,TSigNumberUidNumber),			//24
	MVIP_CTR_ENTRY(ESignatureNodeIdClientType,TSigNodeIdClientType),		//25
	MVIP_CTR_ENTRY(ESignatureClientTypeUidClientType,TSigClientTypeUidClientType),	//26
	MVIP_CTR_ENTRY(ESignatureClientType,TSigClientType),					//27
	MVIP_CTR_ENTRY(ESignatureClientTypeUid,TSigClientTypeUid),				//28
	MVIP_CTR_ENTRY(ESignatureMessageId,TSigMessageId),         				//29
	MVIP_CTR_ENTRY(ESignatureNodeIdNumberNumberNumber,TSigNodeIdNumberNumberNumber),//30
	MVIP_CTR_ENTRY(ESignatureTypeId,TSigTypeId),         				    //31
	MVIP_CTR_ENTRY(ESignatureTypeIdAnyNumber,TSigTypeIdAnyNumber)		    //32
	};


/*ststic*/ void TEBase::RegisterL(CGlobals& aGlobals)
	{
	aGlobals.RegisterInterfaceL(TUid::Uid(KNodeMessagesImplementationUid), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	}

/*ststic*/ void TEBase::DeRegister(CGlobals& aGlobals)
	{
	aGlobals.DeregisterInterface(TUid::Uid(KNodeMessagesImplementationUid));
	}


EXPORT_C Meta::SMetaData* TVCFromGlobalsMessagePayloadCreationPolicy::New(TPtrC8& aSrc, TPtr8& aDst)
	{
	Meta::SMetaData* payload = NULL;
	Meta::CMetaDataVirtualCtorInPlace* vctr = TlsGlobals::Get().VirtualCtor();
	if (vctr)
		{
		__ASSERT_DEBUG(aSrc.Length() >= sizeof(STypeId), User::Panic(KSpecAssert_ElemNodeMessMesC, 2)); //At least the type id must be present
		TUid uid = *reinterpret_cast<const TUid*>(aSrc.Ptr());
		if (vctr->IsInterfaceRegistered(uid))
			{
			payload = vctr->New(aSrc, aDst);
			}
		}
	return payload;
	}

EXPORT_C TSignatureWithPolymorphicPayloadMetaTypeBase::TSignatureWithPolymorphicPayloadMetaTypeBase(TUint8* aData, SMetaData** aPayload, TInt aHostSize)
:	iData(aData),
	iPayload(aPayload),
	iHostSize(aHostSize)
	{
	}

EXPORT_C TInt TSignatureWithPolymorphicPayloadMetaTypeBase::InternalLoad(TPtrC8& aBuffer, TDes8& aTempBuffer)
	{
	*iPayload = NULL; //will be set from the buffer, needs to be initialised for length
	if (aBuffer.Length() < iHostSize)
		{
		return KErrArgument;
		}

    Mem::Copy(iData, aBuffer.Ptr(), iHostSize); //including the ptr (length)
	aBuffer.Set(aBuffer.Ptr() + iHostSize, aBuffer.Length() - iHostSize); //update pointer

	TInt payloadLength = *(reinterpret_cast<TInt32*>(iPayload)); //payload length only
	if (payloadLength)
		{
		if (payloadLength < 0 || aBuffer.Length() < payloadLength || payloadLength > aTempBuffer.MaxLength())
			{
			return KErrArgument;
			}
		aTempBuffer.Copy(aBuffer.Ptr(), payloadLength); //aBuffer already moved to the end
		}
	return KErrNone; //regardless of *iData being initialised (NULL if not)
	}

EXPORT_C TInt TSignatureWithPolymorphicPayloadMetaTypeBase::Store(TDes8& aBuffer) const
	{
	TInt len = Length();
	if (aBuffer.MaxLength() - aBuffer.Length() < len) //including payload if present
		{
		return KErrOverflow;
		}

	TInt offsetToPayload = (TUint8*)iPayload - iData;
	TPtr8 payloadPtr(const_cast<TUint8*>(aBuffer.Ptr()) + aBuffer.Length() + offsetToPayload, sizeof(SMetaData*));
	aBuffer.Append(iData, iHostSize); //Store all including the ptr

	len = 0; //Reset the len - now it means payload length only
	if (*iPayload)
		{
		len = (*iPayload)->Length(); //payload length only
		TInt error = (*iPayload)->Store(aBuffer);
		if (error!=KErrNone)
			{
			return error;
			}
		}

	//Store the length where the pointer is stored
	payloadPtr.Append(reinterpret_cast<TUint8*>(&len), sizeof(SMetaData*));
	return KErrNone;
	}

EXPORT_C void TSignatureWithPolymorphicPayloadMetaTypeBase::Copy(const TAny* aData)
	{
	TInt offset = (TUint8*)iPayload - iData;
	const TUint8* srcdata = reinterpret_cast<const TUint8*>(aData) + sizeof(SMetaData*) - offset;
	Mem::Copy(iData, srcdata, iHostSize);

	SMetaData* const* srcpayload = reinterpret_cast<SMetaData* const*>(aData);
	if (*iPayload && *srcpayload)
		{
		(*iPayload)->Copy(**srcpayload);
		}
	}

EXPORT_C TInt TSignatureWithPolymorphicPayloadMetaTypeBase::Length() const
	{
	TInt len = iHostSize; //including the ptr
	if (*iPayload)
		{
		len += (*iPayload)->Length(); //the payload
		}
	return len;
	}




