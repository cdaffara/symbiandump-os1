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
 @publishedPartner
 @released since 9.5
*/

#ifndef SYMBIAN_NM_ADDRESS_H
#define SYMBIAN_NM_ADDRESS_H

#include <elements/nm_common.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessAdrH, "ElemNodeMessAdrH");
#endif

class TChipReceiver;

namespace Messages
{

class ANode;
class RClientInterface;
class RNodeInterface;
class TRuntimeCtxIdOp;
class TNodeIdOp;
class TSignalBase;
class TNodePeerId;


class TRuntimeCtxId
/**
TRuntimeCtxId is the baseclass intended for addressing message
(TSignalBase) destinations, i.e.: As implied by the respective API,
the sender of a TSignalBase must supply a TRuntimeCtxId object.

TRuntimeCtxId defines the maximum size of the addressing structure
and its bedrock composition  (namely: the destination thread of execution).

@ref TSignalBase
@publishedPartner
@released
*/
	{
	friend class TRuntimeCtxIdOp;
	friend class RNodeInterface;     //to directly access TRuntimeCtxId::KIsNullMask8 and avoid a bit costier TRuntimeCtxIdOp
	friend class TChipReceiver;      //to speed up iThread Id swap on dispatch

public:
	/** The space required for reconstructing the address object.
	It is imagineable that a context within a node context would be required.
	Plus one additional sizeof(TInt) to be on the safe side.
	Size protected by __ASSERT_COMPILE lower down where the
	sizeof(TNodeCtxId) is known.
	*/
	enum { KMaxInlineAddressSize = 20 }; //sizeof(TNodeCtxId) + sizeof(TInt);
	/**
    If this mask is not present in the size of a runtime context id, then the runtime context id is a null address
	and should not be used.
	 */
	enum { KIsNullMask8 = 0x80 };

public:
	/**
	@return the null instance of TRuntimeContextId
	 */
	IMPORT_C static const TRuntimeCtxId& NullId();
	IMPORT_C explicit TRuntimeCtxId();
	/**
	Copy constructor
	@param aRuntimeCtxId runtime context id to copy
	 */
	IMPORT_C TRuntimeCtxId(const TRuntimeCtxId& aRuntimeCtxId); //Copy ctor can not be explicit (?!?)

	/**
	Assignment operator. Copies contents of the passed in runtime context id into this runtime context id.
	@param aRHS runtime ctx id to copy
	*/
	IMPORT_C const TRuntimeCtxId& operator=(const TRuntimeCtxId& aRHS);

	/**
	Compare the contents of this runtime context id to the contents of another.
	Only iSize bytes are compared, so if aRHS is bigger than the runtime context id it is being compared to
	any byte greater than iSize will not be compared.

	@param aRHS runtime context id to compare against
	@return ETrue is the contents of both runtime contexts are the same. EFalse otherwise
	*/
	IMPORT_C TBool operator==(const TRuntimeCtxId& aRHS) const;

	/**
	@see operator==

	@return EFalse if the contents of both runtime context ids are the same, ETrue otherwise
	*/
	inline TBool operator!=(const TRuntimeCtxId& aRHS) const
		{
		return !operator==(aRHS);
		};

	/**
	Compare the first aSize bytes of this runtime context id with aRHS
	@param aRHS runtime context id to compare against
	@param aSize number of bytes to compare
	@return ETrue if the comparison matches, EFalse otherwise
	*/
	IMPORT_C TBool ComparePart(const TRuntimeCtxId& aRHS, TUint8 aSize) const;

	/**
	Serialise into a descriptor
	@param aDes descriptor to serialise into
	@return KErrNone if successful, KErrOverflow if the descriptor is not big enough.
	*/
	IMPORT_C TInt Store(TDes8& aDes) const;

	/**
	@return the size, in bytes, of the id
	*/
	inline TUint8 Size() const {return iSize & ~KIsNullMask8;}
	/**
	@return ETrue if this is a null id, EFalse otherwise
	*/
	inline TBool IsNull() const {return ~iSize & KIsNullMask8;}
	/**
	Make this id a null id
	*/
	inline void SetNull() {iSize &= ~KIsNullMask8;}	//Only one setter allowed by design

	/**
	@return Salt used to prevent reuse of the id
	*/
	inline TUint8 Salt() const {return iSalt;}
	/**
	@return The scope of the address, currently unused
	*/
	inline TUint32 Scope() const {return iScope;}
	/**
	@return thread in which the addressed object was created
	*/
	inline TUint16 Thread() const {return iThread;}

	/**
	Post a message to the addressed object.
	@param aPostFrom address of sender
	@param aMessage message to send
	*/
	IMPORT_C void PostTo(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage) const;

protected:
	/**
	@param aSize size in bytes of the new id
	*/
	IMPORT_C explicit TRuntimeCtxId(TUint8 aSize);

private:
	TUint8 iSize;
	TUint8 iSalt;	//Reuse protection

	TUint16 iThread;
	TUint32 iScope;
	};

class TNodeIdRemainder;
class TNodeId : public TRuntimeCtxId
/**
TNodeId enriches TRuntimeCtxId to enable addressing an object (ANode) 'present'
in the destination thread of execution. The Distributed Object Model (i.e.: this component)
defines a scheme whereby objects (MNodes) running in different runtime contexts can
communicate. It is assumed that an object only ever runs in a single runtime context and
hence its code isn't reentrant. If it were, the object would have potentially as many
identities as there are threads executing the object's code.

@publishedPartner
@released
*/
	{
	friend class TNodeIdOp;
public:
	typedef TNodeIdRemainder TRemainder;

	/**
	@return the null instance of TNodeId
	*/
	IMPORT_C static const TNodeId& NullId();
	IMPORT_C explicit TNodeId();

	/**
	Copy constructor.
	@param aNodeId node id to copy
	*/
	IMPORT_C TNodeId(const TNodeId& aNodeId); //Copy ctor can not be explicit (?!?)

	/**
	Assignment operator. Copies contents of the passed in node id to this node id. aRHS is passed in as a TRuntimeCtxId. The size of this object
	must be greater or equal to the size of the target node id. Otherwise as panic will occur.

	@param aRHS node id to copy
	*/
	IMPORT_C const TNodeId& operator=(const TRuntimeCtxId& aRHS);

	/**
	Is not equal to operator.

	@param aRHS node id to compare to
	@return ETrue if this node id is different to aRHS. EFalse otherwise
	*/
	inline TBool operator!=(const TNodeId& aRHS) const
		{
		return TRuntimeCtxId::operator!=(aRHS);
		}

	/**
	@return the pointer member of this node id
	*/
	inline TAny* Ptr() const {return iPtr;}

	/**
	@return the node this node id addresses
	*/
	inline ANode& Node() const
		{
		__ASSERT_DEBUG(iPtr, User::Panic(KSpecAssert_ElemNodeMessAdrH, 1));
		return *reinterpret_cast<ANode*>(iPtr);
		}

protected:
	IMPORT_C explicit TNodeId(TUint8 aSize);

private:
	TAny* iPtr;
	};

class TNodeIdRemainder
/*
TNodeIdRemainder represents the reminder of TRuntimeCtxId beyond TNodeId.
The split at this level is meaningful through its usefullness. NodeMessages (this component)
defines the concept of a Node (ANode), but also recognizes entities of finer
granularity need representing.
*/
	{
	friend class RNodeInterface;
public:
	/**
	Size of the payload. Defined as the maximum size of an address less the size of a TNodeId.
	*/
	enum
		{
		EPayloadSize = __Align8(TRuntimeCtxId::KMaxInlineAddressSize + sizeof(TUint) - sizeof(TNodeId)),
		};

	/**
	@return the size of the remainder
	*/
	TInt Size() const { return iSize; }
	/**
	@param aFrom TRuntimeCtxId to take payload from.
	*/
	IMPORT_C explicit TNodeIdRemainder(const TRuntimeCtxId& aFrom);
	IMPORT_C explicit TNodeIdRemainder();

	/**
	Assignment operator. Will only copy the bytes from (&aRHS + sizeof(TNodeId)) onwards.
	*/
	IMPORT_C const TNodeIdRemainder& operator=(const TRuntimeCtxId& aRHS);
	/**
	Comparison operator. Will only compare the bytes from (&aRHS + sizeof(TNodeId)) onwards.
	@param aRHS to compare to the payload
	@return ETrue if the bytes match, EFalse otherwise
	*/
	IMPORT_C TBool operator==(const TRuntimeCtxId& aRHS) const;
	/**
	Comparison operator.
	@param aRHS remainder to compare to
	*/
	IMPORT_C TBool operator==(const TNodeIdRemainder& aRHS) const;

	private:
	TUint8 iRemainder[EPayloadSize];
	TInt   iSize;
	};


class TNodeCtxId : public TNodeId
/**
TNodeCtxId enriches TNodeId to describe entities addressable _within_ the nodes
(entities of finer granularity than nodes). It is perceived that a single node
(ANode) can maintain multiple conversations with its peers and hence it's the conversations
that need addressing and not just the node.

@publishedPartner
@released
*/
	{
	friend class TNodeCtxIdOp;
	friend class RNodeCtxInterface;

public:
	/**
	@return the null instance of TNodeCtxId
	*/
	IMPORT_C static const TNodeCtxId& NullId();

	IMPORT_C explicit TNodeCtxId();
	/**
	@param aNodeCtx Value used to identify an entity within a node. Usually an activity id, but not nescessarily.
	@param aNodeId Node to be addressed.
	*/
	IMPORT_C explicit TNodeCtxId(TUint16 aNodeCtx, const TNodeId& aNodeId);

	/**
	Copy constructor.
	*/
	IMPORT_C TNodeCtxId(const TNodeCtxId& aNodeCtxId); //Copy ctor can not be explicit (?!?)

	/**
	Assignment operator. Size of aRHS must be greater than or equal to sizeof(TNodeCtxId)
	@param aRHS object to copy contents from
	*/
	IMPORT_C const TNodeCtxId& operator=(const TRuntimeCtxId& aRHS);

	/**
	Equality operator.
	*/
	inline TBool operator==(const TNodeCtxId& aRHS) const
		{
		return TNodeId::operator==(aRHS);
		}

	/**
	Non-equality operator
	*/
	inline TBool operator!=(const TNodeCtxId& aRHS) const
		{
		return TNodeId::operator!=(aRHS);
		}

	/**
	@return the node ctx value for this address.
	*/
	inline TUint16 NodeCtx() const {return iNodeCtx;}

protected:
	IMPORT_C explicit TNodeCtxId(TUint8 aSize);

private:
	TUint16 iNodeCtx;

	/**
	   Solely here to word align the object, so sizeof() will really return the size.
	   Otherwise, comparision breaks.
	   @internalComponent
	*/
	TUint16 iReserved;
	};


/* Unfortunatelly we can not define this enum using sizeof(TNodeCtxId),
since the size of TNodeCtxId is not known at the base class level.
But this is where we protect the consistency of our definition.
*/
__ASSERT_COMPILE(TRuntimeCtxId::KMaxInlineAddressSize == sizeof(TNodeCtxId) + sizeof(TInt));

/**
Cast an address reference to another type of address reference.
The size of the casted address reference must be greater than or equal to the size of the casted to address type.
*/
template <class ADDRESSTYPE>
ADDRESSTYPE& address_cast(Messages::TRuntimeCtxId& aAddress)
    {
    __ASSERT_DEBUG(aAddress.Size()>=sizeof(ADDRESSTYPE),User::Panic(Messages::KMessagesPanic,Messages::EAddressCastPanic));
    return static_cast<ADDRESSTYPE&>(aAddress);
    }

/**
Cast a constant address reference to another type of constant address reference.
The size of the casted address reference must be greater than or equal to the size of the casted to address type.
*/
template <class ADDRESSTYPE>
const ADDRESSTYPE& address_cast(const Messages::TRuntimeCtxId& aAddress)
    {
	__ASSERT_DEBUG(aAddress.Size()>=sizeof(ADDRESSTYPE),User::Panic(Messages::KMessagesPanic,Messages::EAddressCastPanic));
    return static_cast<const ADDRESSTYPE&>(aAddress);
    }

/**
Cast an address pointer to another type of address pointer.
The size of the casted address pointer must be greater than or equal to the size of the casted to address type.
*/
template <class ADDRESSTYPE>
ADDRESSTYPE* address_cast(Messages::TRuntimeCtxId* aAddress)
    {
    __ASSERT_DEBUG(aAddress,User::Panic(Messages::KMessagesPanic,Messages::EAddressCastPanic));
    return aAddress->Size()>=sizeof(ADDRESSTYPE)? static_cast<ADDRESSTYPE*>(aAddress) : NULL;
    }

/**
Cast a constant address pointer to another type of constant address pointer.
The size of the casted address pointer must be greater than or equal to the size of the casted to address type.
*/
template <class ADDRESSTYPE>
const ADDRESSTYPE* address_cast(const Messages::TRuntimeCtxId* aAddress)
    {
    __ASSERT_DEBUG(aAddress,User::Panic(Messages::KMessagesPanic,Messages::EAddressCastPanic));
    return aAddress->Size()>=sizeof(ADDRESSTYPE)? static_cast<const ADDRESSTYPE*>(aAddress) : NULL;
    }

} //namespace Messages


#endif
//SYMBIAN_NM_ADDRESS_H

