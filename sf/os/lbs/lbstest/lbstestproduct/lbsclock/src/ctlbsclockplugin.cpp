// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GPSSetClockTestPlugin.cpp
// 
//

// System includes.
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <e32property.h>

// Lbs includes.
#include <lbs/gpssetclockbase.h>

// Lbs test includes.
#include <lbs/test/tlbsutils.h>


/**
Test clock adjust plugin.
@internalTechnology
*/
NONSHARABLE_CLASS(CGPSSetClockTestPlugin) : public CGPSSetClockBase
	{
public:
	static CGPSSetClockBase* NewL();
	~CGPSSetClockTestPlugin();
	virtual TInt SetUTCTime(const TTime &aUTCTime);
private:
	CGPSSetClockTestPlugin();
	void ConstructL();
	};


CGPSSetClockBase* CGPSSetClockTestPlugin::NewL()
	{
	CGPSSetClockTestPlugin* self = new(ELeave) CGPSSetClockTestPlugin;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CGPSSetClockTestPlugin::~CGPSSetClockTestPlugin()
	{
	}

/**
Test version of the method that performs the product specific means of changing the system clock.

@param aUTCTime  The system clock should be set to this UTC time.
@return KErrNone if the system clock has been successfully changed. Otherwise a 
				 standard Symbian OS error code.
@internalTechnology
*/
TInt CGPSSetClockTestPlugin::SetUTCTime(const TTime& aUTCTime)
	{
	TInt err;
	TTime time = aUTCTime;

	// Send msg to test harness via a test property - this allows the test harness to determine
	// if the plugin was invoked instead of the default behaviour.
	TPckgBuf<TClockPluginDataOut> pluginDataOutBuf;
	TClockPluginDataOut& pluginDataOut = pluginDataOutBuf();

	pluginDataOut.iResponse = TClockPluginDataOut::EClockPluginResponseOk;   

	err = RProperty::Set(KUidSystemCategory, ELbsTestClockPluginOut, pluginDataOutBuf);

	// Set the system clock.
	if (err == KErrNone)
		{
		err = User::SetUTCTime(time);
		}
	
	return err;
	}

	
CGPSSetClockTestPlugin::CGPSSetClockTestPlugin()
	{}


void CGPSSetClockTestPlugin::ConstructL()
	{}


const TImplementationProxy KImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10281D72,  CGPSSetClockTestPlugin::NewL)
	};


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(KImplementationTable) / sizeof(KImplementationTable);
	return KImplementationTable;
	}
