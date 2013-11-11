/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file lbsnetprotocolstubtest.h
*/
#if (!defined LBSNETPROTOCOLSTUBTEST_H)
#define LBSNETPROTOCOLSTUBTEST_H

#define ENABLE_LBS_DEV_LOGGER 1

#include <e32base.h>
#include <e32cons.h>
#include <e32debug.h>

#include <lbs/test/lbsnetprotocolstub.h>
#include <lbspositioninfo.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistancedatabuilderset.h>

//-------------------------------------------------------------------------------------------------

class CTResponseChannel : public CActive
	{
public:
	static CTResponseChannel* NewL(CConsoleBase* aConsole, CLbsNetworkProtocolBase* aNetProtocol);
	void StartFetch();
	~CTResponseChannel();
protected:
	CTResponseChannel(CConsoleBase* aConsole, CLbsNetworkProtocolBase* aNetProtocol);
	void ConstructL();
	virtual void RunL();
	virtual void DoCancel();
private:
	void HandleKeyPressL(TKeyCode aCode);
	void DisplayKeyPresses();
	CConsoleBase* iConsole;
	TKeyCode iLastCode;
	RArray<TKeyCode> iKeyCodes;
	CLbsNetworkProtocolBase* iProtocol;
	TInt iNextResponse;
	};

//-------------------------------------------------------------------------------------------------

class CNetworkProtocolObserverStub : public MLbsNetworkProtocolObserver, CBase
	{
public:
	static CNetworkProtocolObserverStub* NewL();
	~CNetworkProtocolObserverStub();

	// MLbsNetworkProtocolObserver methods
	void GetCurrentCapabilities(
								TLbsNetPosCapabilities& aCapabilities) const;


	void ProcessStatusUpdate(
								TLbsNetProtocolServiceMask aActiveServiceMask);

	void ProcessPrivacyRequest(
								const TLbsNetSessionId& aSessionId,
								TBool aEmergency, 
								const TLbsNetPosRequestPrivacy& aPrivacy,
								const TLbsExternalRequestInfo& aRequestInfo);
	
	void ProcessLocationRequest(
								const TLbsNetSessionId& aSessionId,
								TBool aEmergency,
								TLbsNetProtocolService aService, 
								const TLbsNetPosRequestQuality& aQuality,
								const TLbsNetPosRequestMethod& aMethod);

	void ProcessSessionComplete(
								const TLbsNetSessionId& aSessionId,
								TInt  aReason);

	void ProcessAssistanceData(
								TLbsAsistanceDataGroup aDataMask,
								const RLbsAssistanceDataBuilderSet& aData,
								TInt aReason);
	
	void ProcessLocationUpdate(
								const TLbsNetSessionId& aSessionId,
								const TPositionInfoBase& aPosInfo);										
private:
	CNetworkProtocolObserverStub();
	void ConstructL();
	};

//-------------------------------------------------------------------------------------------------

#endif // LBSNETPROTOCOLSTUBTEST_H
