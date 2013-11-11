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
 @internalComponent
*/

#ifndef SYMBIAN_NETWORKING_CFTRANSPORTMSG_H
#define SYMBIAN_NETWORKING_CFTRANSPORTMSG_H

#include <cfmsgs.h>
#include <elements/cftransport.h>
#include <elements/nm_address.h>

namespace CommsFW
{

/**
Extends TCFMessage to support cookies and legacy payloads
*/
class TCFMessage2 : public TCFMessage
	{
public:
	explicit TCFMessage2();
	explicit TCFMessage2(const Messages::TRuntimeCtxId& aPostFrom, const Messages::TRuntimeCtxId& aPostTo, const TDesC8& aData);
	explicit TCFMessage2(TPtrC8& aData, const Messages::TRuntimeCtxId& aPostFrom, const Messages::TRuntimeCtxId& aPostTo);

	static void SetChipsFollowing(TUint8 aChipsFollowing, TCFMessage& aMessage);
	static TUint8 ChipsFollowing(const TCFMessage& aMessage);

	//Returns an offset from "this" to the message data
	TUint8 ParseGetOffset(Messages::TRuntimeCtxId*& aPostedFrom, Messages::TRuntimeCtxId*& aPostedTo);
	};

/**
Extends TCFMessage to support cookies and legacy payloads
*/
class TCFLegacyMessagePacker : public TCFMessage
	{
private:
	enum {KLegacyMessageIdOverride = 0xFF};

private:
	union TUnion
		{
		struct TPackage
			{
			TUint8 iChipsFollowing;
			TUint16 iWorkerId;
			};
		struct TLegacyPackage //Needed as the relevant fields of TCFMessage are private
			{
			TCode iCode;
			TCode iSubCode;
			};
		};

public:
	static void PackForPostage(TWorkerId aWorkerId, TCFMessage& aMessage);
	static void UnPack(TCFMessage& aMessage);
	static TWorkerId WorkerId(const TCFMessage& aMessage);
	static TBool IsLegacy(const TCFMessage& aMessage);
	};

/** The largest message that can be sent without using a Blob is KMaxInlineMessageSize, which is controlled by the
maximum number of chips the transport can use for message fragments. Increasing this limit may aid coding by removing
a failure path for the sender, however at the expense of proportionately larger heap and stack footprints
*/
const TInt KMaxNumChips = 3;	// Big enough for a RMessage2 with some supplementary fields. If much increase reqd then consider refactoring to make a template parameter
const TInt KMaxInlineMessageSize = KMaxNumChips * TCFMessage::KMaxRawBlockSize;

#include "elements/cftransportmsg.inl"

} // namespace CommsFW

#endif // SYMBIAN_NETWORKING_CFTRANSPORTMSG_H

