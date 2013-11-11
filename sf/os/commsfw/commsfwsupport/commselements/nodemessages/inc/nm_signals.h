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
 @internalTechnology
*/

#ifndef SYMBIAN_NM_SIGNALS_H
#define SYMBIAN_NM_SIGNALS_H

#include <elements/h_structures.h>
#include <elements/metavirtctor.h>
#include <elements/nm_common.h>

namespace Messages
{

class TRuntimeCtxId;

//-=========================================================
//
// TSignalBase
//
//-=========================================================
class TSignalBase : public Meta::SMetaDataNetCtor
	{
public:
	/** The space required for reconstructing the message object from the stored data.
	This value is not directly related to the maximum length of a message supported
	by the currently used transport implementation.
	*/
	enum { KMaxInlineMessageSize = 224 }; //KMaxInlineMessageSize + KMaxUnstoredOverhead == 256 (currently used transport supports ~150)

	/** The space required for reconstructing the object from the stored data will always be somewhat larger than that data, due
	to the vtbl pointer, iSender, and most variably the overhead for member objects such as descriptors. This value is allows for
	several descriptors in a maximally-sized (non-blob) message without being lavish - in general derived classes shouldn't have
	unserialised members.
	*/
	enum { KMaxUnstoredOverhead = 32 };

public:
	virtual void DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient) = 0;
	virtual void Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, TInt aError) = 0;

public:
	EXPORT_DATA_VTABLE_AND_FN
	};

} //namespace Messages


#endif
	//SYMBIAN_NM_SIGNALS_H

