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

#ifndef __TC_H__
#define __TC_H__

#include <e32base.h>
#include <e32std.h>
#include <networking/module_if.h>
#include "parameters.h"	// ..for TPacketFilter only

class CNif;
class CPdpContext;
class CFlowData;
class CFlowContext;

class CFlowData : public CBase
	{
public:
	static CFlowData* NewL(CFlowContext& aHandle, CNif& aNif, CFlowData *aNext);
	static CFlowData* Find(const CFlowContext* aHandle, CFlowData *aList);
	static CFlowData* Remove(const CFlowContext* aHandle, CFlowData **aList);

	~CFlowData();

	TInt Send(RMBufChain& aPacket, RMBufSendInfo& aInfo);
	void SetContext(CPdpContext *aContext);
	
	inline CPdpContext* PdpContext() const;
	inline const CFlowContext& FlowContext() const;
	inline CNif& Nif() const;
	inline TPacketFilter& PacketFilter() { return iPacketFilter; };

	TDblQueLink iLink;

private:
	CFlowData(CFlowContext& aHandle, CNif& aNif);
	void ConstructL();
	CFlowData*			iNext;				// Links together all CFlowData instances.

private:
	CNif&				iNif;				// CNif
	const CFlowContext&	iFlowContext;		// CFlowContext
	CPdpContext*		iContext;			// PDP context
	TPacketFilter		iPacketFilter;		// Store packet filter here, because the address in CFlowContext
											// might be changed and that causes packet filter manipulation to fail
	};

#include "tc.inl"

#endif
