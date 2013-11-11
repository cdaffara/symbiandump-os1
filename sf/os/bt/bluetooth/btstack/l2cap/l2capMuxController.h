// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Holds a list of all Muxers and listening SAP signal handlers.
// 
//

#ifndef L2CAPMUXCONTROLLER_H
#define L2CAPMUXCONTROLLER_H

#include <e32base.h>
#include <e32std.h>

class CL2CAPMux;
class CL2CapSAPSignalHandler;
class CServProviderBase;
class TBTDevAddr;
class CL2CAPProtocol;

NONSHARABLE_CLASS(CL2CAPMuxController) : public CBase
	{
public:
	 CL2CAPMuxController(CL2CAPProtocol& aProtocol);
	~CL2CAPMuxController();
	
	TInt BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSAP);
	TInt AttachPassiveSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler,const TBTDevAddr& aAddr);
	void AttachListeningSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler);
	void AttachBoundSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler);
	TInt AttachActiveSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler,const TBTDevAddr& aAddr);
	CL2CapSAPSignalHandler* FindListeningSignalHandler(TL2CAPPort aPort);
	CL2CapSAPSignalHandler* FindIdleSignalHandler(TL2CAPPort aPort);
	inline CL2CAPProtocol& Protocol() const;

	void UpdateMuxerPriorities();
	TInt FindFreeUserPSM(TL2CAPPort& aPSM);

	inline TBool CanProtocolClose() const;

private:
	CL2CAPMux* FindMuxer(const TBTDevAddr& aAddr);
	CL2CAPMux* CreateMuxerL(const TBTDevAddr& aAddr);

protected:

private:
	enum
		{
		EBaseMuxerPriority	= CActive::EPriorityStandard,
		};

	// Dbl link list of Muxers
	TDblQue<CL2CAPMux> iMuxers;

	// Dbl link list of sap signal handlers that are only bound as yet
	TDblQue<CL2CapSAPSignalHandler> iBoundSapSignalHandlers;
	
	// Dbl link list of listening sap signal handlers
	TDblQue<CL2CapSAPSignalHandler> iListeningSapSignalHandlers;

	CL2CAPProtocol& iProtocol;

	};

inline CL2CAPProtocol& CL2CAPMuxController::Protocol() const
	{
	return iProtocol;
	}

inline TBool CL2CAPMuxController::CanProtocolClose() const
	{
	// Only close if all of the containers are empty.
	return (iMuxers.IsEmpty() && iBoundSapSignalHandlers.IsEmpty() && iListeningSapSignalHandlers.IsEmpty());
	}

#endif
