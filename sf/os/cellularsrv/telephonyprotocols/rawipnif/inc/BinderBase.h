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
#include <es_mbuf.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/linkprovision.h>
#include "Constants.h"
#include <comms-infras/es_protbinder.h>

class CRawIPFlow;
class CRawIpProvision;
class CRawIPFlow;

class CBinderBase : public CBase, public ESock::MLowerDataSender, public ESock::MLowerControl
	{
public:
	friend class CRawIPFlow;
	CBinderBase(CRawIPFlow& aFlow);
	virtual ~CBinderBase();

public:
	// from ESock::MLowerControl
	virtual TInt BlockFlow(ESock::MLowerControl::TBlockOption aOption);
	virtual void ChangeFlow(CRawIPFlow& aNewFlow);

	//
	// Utility calls from Flow during binding
	//
	ESock::MLowerDataSender* Bind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);
	void Unbind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);

	//
	virtual void StartSending();
	virtual void SetProvision(const CIPConfig& aProvision) = 0;

public:
	virtual void UpdateContextConfigL(const TPacketDataConfigBase& aConfig);
	virtual void UpdateConnectionSpeed(TUint aConnectionSpeed);

	virtual TBool WantsProtocol(TUint16 aProtocolCode)=0;
	virtual void Process(RMBufChain& aPdu) = 0; 

public:
	inline CRawIPFlow& GetFlow();

protected:
	CRawIPFlow* iFlow;
	ESock::MUpperControl* iUpperControl;
	ESock::MUpperDataReceiver* iUpperReceiver;

	};


CRawIPFlow& CBinderBase::GetFlow()
/**
 * Returns to reference to RawIPFlow
 *
 * @return reference to CRawIPFlow class
 */
	{
	ASSERT(iFlow);
	return *iFlow;
	}

#endif
