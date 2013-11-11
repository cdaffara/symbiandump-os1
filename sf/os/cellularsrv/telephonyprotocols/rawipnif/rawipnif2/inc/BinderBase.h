// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the base class for the protocol interface classes.
// 
//

/**
 @file
*/

#ifndef BINDERBASE_H__
#define BINDERBASE_H__

#include <comms-infras/nifif.h>
#include <comms-infras/commsbufchain.h>
#include <es_mbuf.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/linkprovision.h>
#include "Constants.h"
#include <comms-infras/es_protbinder.h>
#include <networking/bca2.h>

class CRawIP2Flow;

class CBinderBase : public CBase, public ESock::MLowerDataSender, public ESock::MLowerControl,
						BasebandChannelAdaptation2::MUpperControl, BasebandChannelAdaptation2::MUpperDataReceiver
	{
public:
	friend class CRawIP2Flow;
	CBinderBase(CRawIP2Flow& aFlow);
	virtual ~CBinderBase();

public:
	// from ESock::MLowerControl
	virtual TInt BlockFlow(ESock::MLowerControl::TBlockOption aOption);
	virtual void ChangeFlow(CRawIP2Flow& aNewFlow);

	//
	// Utility calls from Flow during binding
	//
	ESock::MLowerDataSender* Bind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);
	void Unbind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);

	// from BasebandChannelAdaptation2::MUpperControl
	virtual void StartSending();
	virtual void Error(TInt aErr);

	virtual void SetProvision(const CIPConfig& aProvision) = 0;

public:
	virtual void UpdateContextConfigL(const TPacketDataConfigBase& aConfig);
	virtual void UpdateConnectionSpeed(TUint aConnectionSpeed);

	virtual TBool WantsProtocol(TUint16 aProtocolCode)=0;
	
	//from ESock::MUpperDataReceiver
	virtual void Process(RMBufChain& aPdu) = 0;
	virtual ESock::MLowerDataSender::TSendResult Send(RMBufChain& aPdu) = 0; 
	
	//from BasebandChannelAdaptation2::MUpperDataReceiver
	virtual void Process(RCommsBufChain& aPdu) = 0; 
	
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	void SetType(TUint16 aType); 
#endif
	void SetSender(BasebandChannelAdaptation2::MLowerDataSender* aLowerDataSender);

public:
	inline CRawIP2Flow& GetFlow();

protected:
	CRawIP2Flow* iFlow; // not owned
	ESock::MUpperControl* iUpperControl;  // not owned
	ESock::MUpperDataReceiver* iUpperReceiver;  // not owned
	BasebandChannelAdaptation2::MLowerDataSender* iLowerDataSender; // not owned
	
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	CIPTagHeader* iIPTagHeader; // owned
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
	
	TBool iStarted:1; 

	};


CRawIP2Flow& CBinderBase::GetFlow()
/**
 * Returns to reference to RawIPFlow2
 *
 * @return reference to CRawIPFlow2 class
 */
	{
	ASSERT(iFlow);
	return *iFlow;
	}

#endif
