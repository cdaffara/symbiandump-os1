// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the AVDTP configurators. These assist in the configuration process
// between the GAVDP client and AVDTP.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPCONFIGURATORS_H	
#define AVDTPCONFIGURATORS_H

#include "avdtpSignallingSession.h"

const TInt KAvdtpMinSetConfigLength = 4; //header + media cap

NONSHARABLE_CLASS(CSEPConfigurator) : public CBase
	{
public:
	virtual TInt AddCapability(const TDesC8& aOption) =0;
	virtual TInt Finalise() =0;
protected:
	CSEPConfigurator(CSignallingSession& aSignallingSession, CAvdtpProtocol& aProtocol);
	inline CSignallingSession& SignallingSession() const 
		{
		return iSignallingSession;	
		};
	inline CAvdtpProtocol& Protocol() const 
		{
		return iProtocol;	
		};

protected:
	CSignallingSession& iSignallingSession;
	CAvdtpProtocol&		iProtocol;
	};
	
	
NONSHARABLE_CLASS(CLocalSEPConfigurator) : public CSEPConfigurator
	{
public:
	static CLocalSEPConfigurator* NewL(CSignallingSession& aSignallingSession,
									   CAvdtpProtocol& aProtocol,
									   TSEID aSEID);
	virtual TInt AddCapability(const TDesC8& aOption);
	virtual TInt Finalise();
	~CLocalSEPConfigurator();
private:
	CLocalSEPConfigurator(CSignallingSession& aSignallingSession,
						   CAvdtpProtocol& aProtocol);
	void ConstructL(TSEID aSEID);
private:
	CLocalSEP*	iLocalSEP; //non-owned
	CCapabilityParseVisitor* iCapabilityParseVisitor;
	};
	
	
NONSHARABLE_CLASS(CRemoteSEPConfigurator) : public CSEPConfigurator
	{
public:
	static CRemoteSEPConfigurator* NewL(CSignallingSession& aSignallingSession,
										CAvdtpProtocol& aProtocol,
										TSEID aLocalSEID,
										TSEID aRemoteSEID,
										TBool aIsReconfigure);
	~CRemoteSEPConfigurator();
	virtual TInt AddCapability(const TDesC8& aOption);
	virtual TInt Finalise();
private:
	CRemoteSEPConfigurator(CSignallingSession& aSignallingSession,
						   CAvdtpProtocol& aProtocol,
							TBool aIsReconfigure);
	void ConstructL(TSEID aLocalSEID, TSEID aRemoteSEID);
	void DoFinaliseL();

private:
	CLocalSEP*			iLocalSEP;  //non-owned
	TSEID				iRemoteSEID;
	RBuf8				iSetConfigBuffer; // ownership passed
	TBool				iIsReconfigure;
	TBool				iRecoveryConfigured;
	TBool				iReportingConfigured;
	};

#endif //AVDTPCONFIGURATORS_H
