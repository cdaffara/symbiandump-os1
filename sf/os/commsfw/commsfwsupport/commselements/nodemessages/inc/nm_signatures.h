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
// nm_signature.h
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NM_SIGNATURES_H
#define SYMBIAN_NM_SIGNATURES_H

#include <elements/nm_signals.h>
#include <elements/nm_interfaces.h>
#include <elements/metatype.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessSigH, "ElemNodeMessSigH");
#endif

#define DECLARE_MESSAGE_SIG_1( sig, type, name ) \
struct T##sig : public Messages::TSignatureBase \
    { \
protected: \
    inline T##sig() {} \
    explicit T##sig( const Messages::TNodeSignal::TMessageId& aMessageId, type a##name ) : \
        Messages::TSignatureBase(aMessageId), \
        i##name(a##name) \
	    {} \
public: \
	   DECLARE_MVIP_CTR( T##sig ) \
   	   EXPORT_DATA_VTABLE_AND_FN \
	   type i##name; \
    }; \
template<TInt id, TInt32 realm> \
struct TMessage##sig : public T##sig, public Messages::TSignatureBase::TTypeToMessageId<id, realm> \
    { \
	explicit TMessage##sig(type a##name ) : \
	    T##sig(Messages::TNodeSignal::TMessageId(id, realm), a##name) \
	    {} \
    };

#define DECLARE_MESSAGE_SIG_2( sig, type, name, type2, name2 ) \
struct T##sig : public Messages::TSignatureBase \
    { \
protected: \
	inline T##sig() {} \
	explicit T##sig( const Messages::TNodeSignal::TMessageId& aMessageId, type a##name, type2 a##name2 ) : \
	   Messages::TSignatureBase(aMessageId), \
	   i##name(a##name), \
	   i##name2(a##name2) \
	    {} \
public: \
	   DECLARE_MVIP_CTR( T##sig ) \
   	   EXPORT_DATA_VTABLE_AND_FN \
	   type i##name; \
	   type2 i##name2; \
    }; \
template<TInt id, TInt32 realm> \
struct TMessage##sig : public T##sig, public Messages::TSignatureBase::TTypeToMessageId<id, realm> \
    { \
	explicit TMessage##sig(type a##name, type2 a##name2 ) : \
	    T##sig(Messages::TNodeSignal::TMessageId(id, realm), a##name, a##name2) \
        {} \
    };

#define DECLARE_MESSAGE_SIG_3( sig, type, name, type2, name2, type3, name3 ) \
struct T##sig : public Messages::TSignatureBase \
    { \
protected: \
	inline T##sig() {} \
	explicit T##sig( const Messages::TNodeSignal::TMessageId& aMessageId, type a##name, type2 a##name2, type3 a##name3 ) : \
	   Messages::TSignatureBase(aMessageId), \
	   i##name(a##name), \
	   i##name2(a##name2), \
	   i##name3(a##name3) \
	    {} \
public: \
	   DECLARE_MVIP_CTR( T##sig ) \
   	   EXPORT_DATA_VTABLE_AND_FN \
	   type i##name; \
	   type2 i##name2; \
	   type3 i##name3; \
    }; \
template<TInt id, TInt32 realm> \
struct TMessage##sig : public T##sig, public Messages::TSignatureBase::TTypeToMessageId<id, realm> \
    { \
	explicit TMessage##sig(type a##name, type2 a##name2, type3 a##name3 ) : \
	    T##sig(Messages::TNodeSignal::TMessageId(id, realm), a##name, a##name2, a##name3) \
        {} \
    };

#define DECLARE_MESSAGE_SIG_4( sig, type, name, type2, name2, type3, name3, type4, name4 ) \
struct T##sig : public Messages::TSignatureBase \
    { \
protected: \
	inline T##sig() {} \
	explicit T##sig( const Messages::TNodeSignal::TMessageId& aMessageId, type a##name, type2 a##name2, type3 a##name3, type4 a##name4 ) : \
	   Messages::TSignatureBase(aMessageId), \
	   i##name(a##name), \
	   i##name2(a##name2), \
	   i##name3(a##name3), \
	   i##name4(a##name4) \
	    {} \
public: \
	   DECLARE_MVIP_CTR( T##sig ) \
   	   EXPORT_DATA_VTABLE_AND_FN \
	   type i##name; \
       type2 i##name2; \
       type3 i##name3; \
	   type4 i##name4; \
    }; \
template<TInt id, TInt32 realm> \
struct TMessage##sig : public T##sig, public Messages::TSignatureBase::TTypeToMessageId<id, realm> \
    { \
	explicit TMessage##sig(type a##name, type2 a##name2, type3 a##name3, type4 a##name4 ) : \
	    T##sig(Messages::TNodeSignal::TMessageId(id, realm), a##name, a##name2, a##name3, a##name4) \
	    {} \
    };

namespace Messages
{

//-=========================================================
//
// TNodeSignal
//
//-=========================================================
class TNodeSignal : public TSignalBase
	{
public:
    class TMessageId
        {
        friend class TNodeSignal;
    public:

		typedef TUint TRealm;

        TBool operator==(const TMessageId& aMessageId) const
            {
            return aMessageId.iRealm == iRealm && aMessageId.iId == iId;
            }

        TBool operator!=(const TMessageId& aMessageId) const
            {
            return !((*this) == aMessageId);
            }

        TMessageId& operator=(const TMessageId& aMessageId)
            {
            iId = aMessageId.iId;
            iRealm = aMessageId.iRealm;
            return *this;
            }

        TUint MessageId() const
            {
            return iId;
            }

        TRealm Realm() const
            {
            return iRealm;
            }

        TBool IsNull() const
            {
            return MessageId() == KNullMessageId;
            }

        TMessageId(TUint aId, TRealm aRealm)
        :	iId(aId), iRealm(aRealm)
        	{ __ASSERT_DEBUG(!(aId & 0xFFFF0000), User::Panic(KSpecAssert_ElemNodeMessSigH, 1)); }

        TMessageId()
        :	iId(KNullMessageId), iRealm(0)
        	{}

    private:
        TUint16 iId;
        TRealm  iRealm;   //determines the realm for the iMessageId
                          //to avoid clashes with other realms.protected:
        };

    template<TInt id, TMessageId::TRealm realm>
    struct TTypeToMessageId
        {
        enum
            {
            EId    = id,
            ERealm = realm,
            };

        inline static TMessageId Id()
            {
            return TMessageId(id,realm);
            }
        };

	typedef TTypeToMessageId<KNullMessageId, 0> TNullMessageId;

	template<typename MSG>
	TBool IsMessage()
		{
		return iMessageId.iId == MSG::EId && iMessageId.iRealm == MSG::ERealm;
		}

protected:
	//We need to define the constructor or the PlacementNew (DECLARE_MVIP_CTR)
	//will use a default one. (And initialise attributes to 0xCCCCCCCC etc).
	TNodeSignal()
	:	TSignalBase()
		{
		}

	explicit TNodeSignal(const TMessageId& aMessageId)
	:	TSignalBase(),
	    iMessageId(aMessageId)
		{
		}

public:
	const TMessageId& MessageId() const
	    {
	    return iMessageId;
	    }

	void ClearMessageId()
	    {
	    iMessageId = TMessageId();
	    }

private:
	mutable TMessageId iMessageId;

public:
	EXPORT_DATA_VTABLE_AND_FN
	};

//-=========================================================
//
// message_cast
//
//-=========================================================
template <class MESSAGE>
MESSAGE& message_cast(Messages::TNodeSignal& aMessage)
    {
    __ASSERT_DEBUG(aMessage.MessageId() == MESSAGE::Id() && aMessage.IsTypeOf(Meta::STypeId(MESSAGE::TypeId())), User::Panic(Messages::KMessagesPanic,Messages::EMessageCastPanic));
    return static_cast<MESSAGE&>(aMessage);
    }

template <class MESSAGE>
const MESSAGE& message_cast(const Messages::TNodeSignal& aMessage)
    {
    __ASSERT_DEBUG(aMessage.MessageId() == MESSAGE::Id() && aMessage.IsTypeOf(Meta::STypeId(MESSAGE::TypeId())), User::Panic(Messages::KMessagesPanic,Messages::EMessageCastPanic));
    return static_cast<const MESSAGE&>(aMessage);
    }

template <class MESSAGE>
MESSAGE* message_cast(Messages::TNodeSignal* aMessage)
    {
    __ASSERT_DEBUG(aMessage ,User::Panic(Messages::KMessagesPanic,Messages::EMessageCastPanic));
    return (aMessage->MessageId() == MESSAGE::Id() && aMessage->IsTypeOf(Meta::STypeId(MESSAGE::TypeId()))) ?
    	static_cast<MESSAGE*>(aMessage) :
    	NULL;
    }

template <class MESSAGE>
const MESSAGE* message_cast(const Messages::TNodeSignal* aMessage)
    {
    __ASSERT_DEBUG(aMessage,User::Panic(Messages::KMessagesPanic,Messages::EMessageCastPanic));
    return (aMessage->MessageId() == MESSAGE::Id() && aMessage->IsTypeOf(Meta::STypeId(MESSAGE::TypeId()))) ?
	    static_cast<const MESSAGE*>(aMessage) :
	    NULL;
    }

//
// TSignatureBase
//
class TSignatureBase : public TNodeSignal
	{
public:
	IMPORT_C virtual void DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient);
	IMPORT_C virtual void Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, TInt aError);

protected:
	//We need to define the constructor or the PlacementNew (DECLARE_MVIP_CTR)
	//will use a default one. (And initialise attributes to 0xCCCCCCCC etc).
	IMPORT_C explicit TSignatureBase();
	IMPORT_C explicit TSignatureBase(const TNodeSignal::TMessageId& aMessageId);

private:
	//Forbidden ops.
	TSignatureBase(const TSignatureBase&);
	TSignatureBase& operator=(TSignatureBase);

public:
	DECLARE_MVIP_CTR(TSignatureBase)
	EXPORT_DATA_VTABLE_AND_FN

	// GCC enforces the C++ ISO standard rule that a class expose a public copy constructor
	// if a temporary variable is passed as a parameter to a function by reference like this:
	//
	// Func( TMessage() ); // will not compile since the copy constructor is private
	//
	// As this class does not, we provide this CRef function so a temporary can still be
	// passed like this:
	//
	// Func( TMessage().CRef() ); // use CRef() instead
	const TSignatureBase& CRef() const
		{
		return *this;
		}
	};

//
// Signature classes
//
template<TInt id, TInt32 realm>
struct TMessageSigVoid : public Messages::TSignatureBase, public Messages::TSignatureBase::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigVoid() :
	    Messages::TSignatureBase(Messages::TNodeSignal::TMessageId(id, realm))
	    { }
    };

DECLARE_MESSAGE_SIG_1(SigNumber, TInt, Value)
DECLARE_MESSAGE_SIG_1(SigUid, TUid, Uid)
DECLARE_MESSAGE_SIG_1(SigAny, TAny*, Ptr)
DECLARE_MESSAGE_SIG_1(SigNodeId, TNodeId, NodeId)
DECLARE_MESSAGE_SIG_1(SigRMessage2, const RMessage2, Message)
DECLARE_MESSAGE_SIG_1(SigMetaDataPtr, Meta::SMetaData*, Ptr)
DECLARE_MESSAGE_SIG_1(SigClientType, Messages::TClientType, ClientType)
DECLARE_MESSAGE_SIG_1(SigMessageId, TNodeSignal::TMessageId, MsgId)
DECLARE_MESSAGE_SIG_1(SigTypeId, Meta::STypeId, TypeId)
DECLARE_MESSAGE_SIG_2(SigNodeIdNumber, TNodeId, NodeId, TInt, Value)
DECLARE_MESSAGE_SIG_2(SigNodeIdClientType, TNodeId, NodeId, TClientType, ClientType)
DECLARE_MESSAGE_SIG_2(SigNumberNumber, TInt, Value1, TInt, Value2)
DECLARE_MESSAGE_SIG_2(SigAnyNumber, TAny*, Ptr, TInt, Value)
DECLARE_MESSAGE_SIG_2(SigUidNumber, TUid, Uid, TInt, Value)
DECLARE_MESSAGE_SIG_2(SigNodeIdAny, TNodeId, NodeId, TAny*, Ptr)
DECLARE_MESSAGE_SIG_2(SigNodeIdNodeId, TNodeId, NodeId1, TNodeId, NodeId2)
DECLARE_MESSAGE_SIG_2(SigNumberRMessage2, TInt, Value, RMessage2, Message)
DECLARE_MESSAGE_SIG_2(SigClientTypeUid, Messages::TClientType, ClientType, TUid, Uid)
DECLARE_MESSAGE_SIG_2(SigMessageIdNumber, TNodeSignal::TMessageId, MsgId, TInt, Value)
DECLARE_MESSAGE_SIG_3(SigNodeIdNumberNumber, TNodeId, NodeId, TInt, Value1, TInt, Value2)
DECLARE_MESSAGE_SIG_3(SigNumberNumberNumber, TInt, Value1, TInt, Value2, TInt, Value3)
DECLARE_MESSAGE_SIG_3(SigNodeIdNodeIdNumber, TNodeId, NodeId1, TNodeId, NodeId2, TInt, Value)
DECLARE_MESSAGE_SIG_3(SigNumberUidNumber, TInt, Value1, TUid, Uid, TInt, Value2)
DECLARE_MESSAGE_SIG_3(SigClientTypeUidClientType, Messages::TClientType, ClientType1, TUid, Uid, Messages::TClientType, ClientType2)
DECLARE_MESSAGE_SIG_3(SigTypeIdAnyNumber, Meta::STypeId, TypeId, TAny*, Ptr, TInt, Value)
DECLARE_MESSAGE_SIG_4(SigNumberNumberNumberNumber, TInt, Value1, TInt, Value2, TInt, Value3, TInt, Value4)
DECLARE_MESSAGE_SIG_4(SigNodeIdNumberNumberNumber, TNodeId, NodeId, TInt, Value1, TInt, Value2, TInt, Value3)

template<TInt id, TInt32 realm>
class TMessageExtensionRMessage2 : public TSigNumberRMessage2, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
	TMessageExtensionRMessage2(TInt aMsgId, const RMessage2& aMessage)
	:	TSigNumberRMessage2(TNodeSignal::TMessageId(id, realm), aMsgId, aMessage) {}
	inline TInt ExtensionMsgId() const
		{
		return iValue;
		}

protected:
	TMessageExtensionRMessage2()
	:	TSigNumberRMessage2()
		{}
    };

template<TInt id, TInt32 realm, TInt defValue = 0>
class TMessageSigNumberDefault : public TSigNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
    enum { EDefValue = defValue };
	TMessageSigNumberDefault(TInt aValue = defValue)
	:	TSigNumber(TNodeSignal::TMessageId(id, realm), aValue)
		{}
    };

template<TInt id, TInt32 realm, TInt defValue = 0>
class TMessageSigNumberNumberDefault : public TSigNumberNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
    enum { EDefValue = defValue };
	TMessageSigNumberNumberDefault(TInt aValue1, TInt aValue2 = defValue)
	:	TSigNumberNumber(TNodeSignal::TMessageId(id, realm), aValue1, aValue2)
		{}
    };

template<TInt id, TInt32 realm, TInt defValue1 = 0, TInt defValue2 = 0>
class TMessageSigNumberDefaultNumberDefault : public TSigNumberNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
    enum { EDefValue1 = defValue1, EDefValue2 = defValue2 };
	TMessageSigNumberDefaultNumberDefault(TInt aValue1 = defValue1, TInt aValue2 = defValue2)
	:	TSigNumberNumber(TNodeSignal::TMessageId(id, realm), aValue1, aValue2)
		{}
    };

template<TInt id, TInt32 realm, TInt defValue = 0>
class TMessageSigAnyNumberDefault : public TSigAnyNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
    enum { EDefValue = defValue };
	TMessageSigAnyNumberDefault(TAny* aPtr, TInt aValue = defValue)
	:	TSigAnyNumber(TNodeSignal::TMessageId(id, realm), aPtr, aValue)
		{}
    };

template<TInt id, TInt32 realm, TInt defValue1 = 0>
class TMessageSigNodeIdNumberDefault : public TSigNodeIdNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
    TMessageSigNodeIdNumberDefault(const TNodeId& aNodeId, TInt aValue = defValue1)
    :   TSigNodeIdNumber(TNodeSignal::TMessageId(id, realm), aNodeId, aValue)
        {}
    };

template<TInt id, TInt32 realm, TInt defValue1 = 0, TInt defValue2 = 0>
class TMessageSigNodeIdNumberDefaultNumberDefault : public TSigNodeIdNumberNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
public:
	TMessageSigNodeIdNumberDefaultNumberDefault(const TNodeId& aNodeId, TInt aValue1 = defValue1, TInt aValue2 = defValue2)
	:	TSigNodeIdNumberNumber(TNodeSignal::TMessageId(id, realm), aNodeId, aValue1, aValue2)
		{}
    };

template<TInt id, TInt32 realm>
struct TMessageSigNum24Num8 : protected TSigNumber, public TSignatureBase::TTypeToMessageId<id, realm>
    {
	explicit TMessageSigNum24Num8(TInt aValue24, TInt aValue8) :
	    TSigNumber(TNodeSignal::TMessageId(id, realm), (aValue24<<8)|(aValue8&0xFF)){}

	TUint Value24() const { return iValue>>8;}
	TUint Value8() const { return iValue&0xFF;}
    };

//
// Signature classes - helper classes
//
class TVCFromGlobalsMessagePayloadCreationPolicy
	{
public:
	IMPORT_C static Meta::SMetaData* New(TPtrC8& aSrc, TPtr8& aDst);
	};

class TSignatureWithPolymorphicPayloadMetaTypeBase : public Meta::MMetaType
	{
protected:
	IMPORT_C TSignatureWithPolymorphicPayloadMetaTypeBase(TUint8* aData, Meta::SMetaData** aPayload, TInt aHostSize);
	IMPORT_C TInt InternalLoad(TPtrC8& aBuffer, TDes8& aTempBuffer);
	IMPORT_C virtual TInt Store(TDes8& aBuffer) const;
	IMPORT_C virtual void Copy(const TAny* aData);
	IMPORT_C virtual TInt Length() const;

protected:
	TUint8* iData;
	Meta::SMetaData** iPayload;
	TInt iHostSize;
	};

/**
The class that determins how the payload will be loaded. A templated class that says the payload attribute offset and
the payload atrribute creation class ( Policy class ). The loading of the payload will be determined by the attribute
creation class.

Template parameters:
class TSIGNATURE - The signature class.
TInt PAYLOADATTRIBOFFSET - The offset where the payload is persent in the signature class.
class TATTRIBUTECREATIONPOLICY - This class determines how the payload should be constructued. The attribute policty class
should implement the Meta::SMetaData* New(TPtrC8& aSrc, TPtr8& aDst) function.
TInt PAYLOADBUFFERMAXLEN - The maximum payload length. Used internally during the load time.

@internalTechnology
*/
template <class TSIGNATURE, TInt PAYLOADATTRIBOFFSET, class TATTRIBUTECREATIONPOLICY, TInt PAYLOADBUFFERMAXLEN = TSignalBase::KMaxInlineMessageSize>
class TSignatureWithPolymorphicPayloadMetaType : public TSignatureWithPolymorphicPayloadMetaTypeBase
{
public:
	inline static MMetaType* NewL(const TAny* aMem, const TAny* aData)
		{
		// For some reason this assertion does not compile under x86gcc which complains
		// that a default template argument has been specified for a templated member
		// function.  There is no such function so this is probably a parser bug.
		#if !defined(__GCCXML__) && !defined(__X86GCC__)
			__ASSERT_COMPILE(PAYLOADATTRIBOFFSET == __Align8(PAYLOADATTRIBOFFSET)); //must be alligned!
		#endif
		return ::new ((TUint8*)aMem) TSignatureWithPolymorphicPayloadMetaType(aData);
		}

	inline virtual TInt Load(TPtrC8& aBuffer)
		{
		TBuf8<PAYLOADBUFFERMAXLEN> buffer;
		TInt error = TSignatureWithPolymorphicPayloadMetaTypeBase::InternalLoad(aBuffer, buffer);
		if (error == KErrNone && buffer.Length() > 0)
			{
			TPtrC8 src(buffer);
			TPtr8 dst(iData + iHostSize, buffer.Length()); //always enough since the STypeId is bigger than the virtual table ptr etc
			*iPayload = TATTRIBUTECREATIONPOLICY::New(src, dst);
			}
		return error;
		}

private:
	TSignatureWithPolymorphicPayloadMetaType(const TAny* aData)
	:	TSignatureWithPolymorphicPayloadMetaTypeBase(const_cast<TUint8*>(reinterpret_cast<const TUint8*>(aData) + sizeof(Meta::SMetaData*) - PAYLOADATTRIBOFFSET),
	                                                 const_cast<Meta::SMetaData**>(reinterpret_cast<Meta::SMetaData* const*>(aData)),
	                                                 sizeof(TSIGNATURE) - sizeof(TSignalBase))
		{
		}
	};

} // namespace Messages

#endif // SYMBIAN_NM_SIGNATURE_H

