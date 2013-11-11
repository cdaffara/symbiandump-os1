// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration of the RPdpFsmInterface for SPUD TEL unit tests
// 
//

/**
 @file
 @internalComponent
*/

#ifndef PDPFSMINTERFACETEST_H
#define PDPFSMINTERFACETEST_H

#include "CPdpFsmInterface.h"

// forward declarations
class CInputRequestListener;

/** Implementation of CPdpFsmInterface used to test the Etel driver */
class CPdpFsmInterfaceTest : public CPdpFsmInterface
	{
public:
	CPdpFsmInterfaceTest(CInputRequestListener& aListener);
	
	virtual TInt Input(TContextId aPdpId, const TInt aOperation, const TInt aParam);

	
#ifdef SYMBIAN_NETWORKING_UMTSR5
	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR5Requested& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR5Negotiated& aParam);
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR5Requested& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR5Negotiated& aParam) const;

#else
// !SYMBIAN_NETWORKING_UMTSR5 

	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Requested& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Negotiated& aParam);
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR99_R4Requested& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR99_R4Negotiated& aParam) const;

#endif 
// SYMBIAN_NETWORKING_UMTSR5 

	virtual TInt Set(TContextId aPdpId, const TTFTInfo& aParam);
	virtual TInt Set(TContextId aPdpId, const TTFTOperationCode& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketContext::TDataChannelV2& aParam);
	virtual TInt Set(TContextId aPdpId, const TPacketDataConfigBase& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketContext::TContextStatus& aParam);

	
	virtual TInt Get(TContextId aPdpId, TTFTInfo& aParam) const;
	virtual TInt Get(TContextId aPdpId, TTFTOperationCode& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketContext::TDataChannelV2& aParam) const;
	virtual TInt Get(TContextId aPdpId, TPacketDataConfigBase& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketContext::TContextStatus& aParam) const;

	virtual void Set(const RPacketService::TStatus aParam); // applies to all contexts
	virtual void Get(RPacketService::TStatus& aParam); // applies to all contexts

	virtual const TName& TsyName();

private:
	/** Used as the interface between the thread running the active objects and the test execute thread.
		Contains the data passed to/from Input requests */
	CInputRequestListener& iListener;
	/** Holds the next TFT operation code and data to be used for TFT operations */
	TTFTOperationCode iTftOperationCode;
	TTFTInfo iTftInfo;

	};
	
#endif
// PDPFSMINTERFACETEST_H

