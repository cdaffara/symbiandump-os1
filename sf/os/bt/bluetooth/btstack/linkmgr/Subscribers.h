// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _SUBSCRIBERS_H
#define _SUBSCRIBERS_H

#include <e32base.h>
#include <e32property.h>

class CLinkMgrProtocol;

NONSHARABLE_CLASS(CSubscriber) : public CActive
	{
public:
	~CSubscriber();
protected:
	CSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void ConstructL(const TUint aKey);
	void Subscribe();
protected:
	CLinkMgrProtocol&	iParent;
	RProperty			iProperty;
private:
	void DoCancel();
	TInt RunError(TInt aError);
	};
	
NONSHARABLE_CLASS(CDiscoverabilitySubscriber) : public CSubscriber
	{
public:
	static CDiscoverabilitySubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CDiscoverabilitySubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};


NONSHARABLE_CLASS(CLimitedSubscriber) : public CSubscriber
	{
public:
	static CLimitedSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CLimitedSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

NONSHARABLE_CLASS(CDeviceClassSubscriber) : public CSubscriber
	{
public:
	static CDeviceClassSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CDeviceClassSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};
	
NONSHARABLE_CLASS(CDeviceNameSubscriber) : public CSubscriber
	{
public:
	static CDeviceNameSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CDeviceNameSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

NONSHARABLE_CLASS(CAFHChannelAssessmentModeSubscriber) : public CSubscriber
	{
public:
	static CAFHChannelAssessmentModeSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CAFHChannelAssessmentModeSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

NONSHARABLE_CLASS(CAFHHostChannelClassificationSubscriber) : public CSubscriber
	{
public:
	static CAFHHostChannelClassificationSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CAFHHostChannelClassificationSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

NONSHARABLE_CLASS(CRegistryRemoteTableSubscriber) : public CSubscriber
	{
public:
	static CRegistryRemoteTableSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CRegistryRemoteTableSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

NONSHARABLE_CLASS(CAcceptPairedOnlySubscriber) : public CSubscriber
	{
public:
	static CAcceptPairedOnlySubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CAcceptPairedOnlySubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

NONSHARABLE_CLASS(CDebugModeSubscriber) : public CSubscriber
	{
public:
	static CDebugModeSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CDebugModeSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

NONSHARABLE_CLASS(CPageScanParametersSubscriber) : public CSubscriber
	{
public:
	static CPageScanParametersSubscriber* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
private:
	CPageScanParametersSubscriber(CLinkMgrProtocol& aLinkMgrProtocol);
	void RunL();
	};

#endif //#ifndef _SUBSCRIBERS_H
