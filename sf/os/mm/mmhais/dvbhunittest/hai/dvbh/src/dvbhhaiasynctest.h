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
 @internalComponent
 @prototype
*/

#ifndef DVBHHAIASYNCTEST_H
#define DVBHHAIASYNCTEST_H

#include <testframework.h>
#include "dvbhhaitestutil.h"
#include <e32property.h>
#include <mobiletv/hai/dvbh/dvbhreceiverinfo.h>

class RDvbhHaiAsyncTest : public RAsyncTestStep,
						  public MDvbhHaiTestLoggerInterface,
						  public MDvbhStateObserver,
					 	  public MDvbhSignalQualityObserver,
					 	  public MDvbhPlatformObserver,
					 	  public MDvbhNetworkTimeObserver,
					 	  public MDvbhFrequencyObserver,
					 	  public MDvbhCellIdObserver,
					 	  public MDvbhNetworkIdObserver
	{
private:
	class CAlternativeStateObserver;
	friend class CDvbhTestUtil;
	friend class CAlternativeStateObserver;

public:
	static RDvbhHaiAsyncTest* NewL(TBool aRequireReceiverInfo, const TDesC& aTestName);
	
	//MDvbhStateObserver
	virtual void DvbhStateChange( TDvbhState aNewState );
	
	//MDvbhSignalQualityObserver
	void DvbhSignalQualityChange( const TDvbhSignalQuality& aNewSignalQuality );
	
	//MDvbhPlatformObserver
	void DvbhPlatformChange( const TDvbhPlatform& aNewPlatform, const TIp6Addr& aESGRoot );
	
	//MDvbhNetworkTimeObserver
	void DvbhNetworkTimeUpdate();
	
	//MDvbhFrequencyObserver
	void DvbhFrequencyChange( const TDvbhFrequency& aNewFrequency );
	
	//MDvbhCellIdObserver
	void DvbhCellIdChange( const TDvbhCellId& aNewCellId );
	
	//MDvbhNetworkIdObserver
	void DvbhNetworkIdChange( const TDvbhNetworkId& aNewNetworkId );
protected:
	//From RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	
	//From MDvbhHaiTestLoggerInterface
	void InfoPrintf3(const TDesC16& aString, TInt aInt1, TInt aInt2);
	void InfoPrintf4(const TDesC16& aString, TInt aInt1, TInt aInt2, TInt aInt3);

	//Access to protected data
	
	//CDvbhReceiverInfo Positive Async Test Steps	
	void DoSetStateObserverTestStep(TBool aProperyExists); //111 & 118
	void DoSetSignalQualityObserverTestStep(TBool aProperyExists); //112 & 119
	void DoSetPlatformObserverTestStep(TBool aProperyExists); //113 & 120
	void DoSetNetworkTimeObserverTestStep(TBool aProperyExists); //114 & 121
	void DoSetFrequencyObserverTestStep(TBool aProperyExists); //115 & 122
	void DoSetCellIdObserverTestStep(TBool aProperyExists); //116 & 123
	void DoSetNetworkIdObserverTestStep(TBool aProperyExists); //117 & 124
	void DoSetMultipleStateObserversTestStep(TBool aProperyExists); //125 & 126
	void DoCreateMultipleReceiverInfoTestStep(TBool aProperyExists); //127 & 128
	
	//CDvbhReceiverInfo Negative Async Test Steps
	void DoSetObserversNoMemory(); //401
private:
	/**
	* A second state observer to test setting multiple observers.
	*/
	class CAlternativeStateObserver : public CBase,
									  public MDvbhStateObserver
		{
	public:
		static CAlternativeStateObserver* NewL(RDvbhHaiAsyncTest& aTestStep);
		virtual void DvbhStateChange( TDvbhState aNewState );
	private:
		CAlternativeStateObserver(RDvbhHaiAsyncTest& aTestStep);
		RDvbhHaiAsyncTest& iTestStep;
		};
		
	enum
		{
		E1stStateObserver = 0,
		E2ndStateObserver = 1
		};
		
private:
	RDvbhHaiAsyncTest(TBool aRequireReceiverInfo, const TDesC& aTestName);
	TVerdict DoPreTest();
	void SetupParameterData();
private:
	TTime				iExpectedNetworkTime;
	TTime				iInitialNetworkTime;	
	TDvbhPlatformProperty iInitialPlatform;
	TDvbhPlatformProperty iExpectedPlatform;
	
	TBool				iPreTestRun;
	TBool				iRequireReceiverInfo;
	
	TInt				iStateCallbackVerdicts[2];
	
	CDvbhReceiverInfo*			iRxInfo;
	CDvbhReceiverInfo*			i2ndRxInfo;
	CDvbhTestUtil*				iTestUtils;
	CAlternativeStateObserver*	i2ndStateObserver;
	};


#endif // DVBHHAIASYNCTEST_H
