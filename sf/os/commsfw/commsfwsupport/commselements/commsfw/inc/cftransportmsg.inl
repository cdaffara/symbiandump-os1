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

/*static*/ inline void TCFMessage2::SetChipsFollowing(TUint8 aChipsFollowing, TCFMessage& aMessage)
	{
	*(aMessage.RawBlock()) = aChipsFollowing;
	}

/*static*/ inline TUint8 TCFMessage2::ChipsFollowing(const TCFMessage& aMessage)
	{
	return *(aMessage.RawBlock());
	}

inline TCFMessage2::TCFMessage2()
	{
	}

/*static*/ inline void TCFLegacyMessagePacker::UnPack(TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(IsLegacy(aMessage), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	TUnion::TLegacyPackage* lp = reinterpret_cast<TUnion::TLegacyPackage*>(aMessage.RawBlock());
	lp->iCode = lp->iSubCode;
	}

/*static*/ inline TWorkerId TCFLegacyMessagePacker::WorkerId(const TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(IsLegacy(aMessage), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	return reinterpret_cast<const TUnion::TPackage*>(aMessage.RawBlock())->iWorkerId;
	}
	
/*static*/ inline TBool TCFLegacyMessagePacker::IsLegacy(const TCFMessage& aMessage)
	{
	return reinterpret_cast<const TUnion::TPackage*>(aMessage.RawBlock())->iChipsFollowing == KLegacyMessageIdOverride;
	}

/*static*/ inline void TCFLegacyMessagePacker::PackForPostage(TWorkerId aWorkerId, TCFMessage& aMessage)
	{
	__ASSERT_DEBUG(!IsLegacy(aMessage), User::Panic(KCFChannelPanic, ECFChanMsgTooBig));
	TUnion::TLegacyPackage* lp = reinterpret_cast<TUnion::TLegacyPackage*>(aMessage.RawBlock());
	lp->iSubCode = lp->iCode;
	TUnion::TPackage* p = reinterpret_cast<TUnion::TPackage*>(aMessage.RawBlock());
	p->iChipsFollowing = KLegacyMessageIdOverride;
	p->iWorkerId = aWorkerId;
	}


