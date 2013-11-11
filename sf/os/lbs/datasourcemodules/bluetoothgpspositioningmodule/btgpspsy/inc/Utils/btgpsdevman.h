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
// btengdevman.h
// 
//

#ifndef BTGPSDEVMANOBSERVER_H
#define BTGPSDEVMANOBSERVER_H

//  INCLUDES

#include <e32base.h> //For CTimer
#include <btdevice.h>
#include <btmanclient.h>

/**
 * Observer for DevMan queries
 */
class MBTGPSDevManObserver
    {
	public:
      virtual void HandleGetDevicesComplete(TInt aErr, RBTDeviceArray* aDeviceArray) = 0;
      virtual void HandleAddDeviceComplete(TInt aErr) = 0;
      virtual void HandleDeleteDevicesComplete(TInt aErr) = 0;
    };
    
/**
 * Wrapper class for performing a BTRegistry search
 * @internalTechnology
 */
class CBTGPSDevMan : public CActive
    {
    public:
    	virtual TInt GetDevices(TBTRegistrySearch& aSearchPattern, RBTDeviceArray* aDeviceArray);
    	static CBTGPSDevMan* NewL(MBTGPSDevManObserver* aObserver);
    	~CBTGPSDevMan();
    	
    private:
    	void ConstructL(MBTGPSDevManObserver* aObserver);
    	CBTGPSDevMan();
    	void Find(TBTRegistrySearch aSearchPattern);
    	void RunL();
    	TInt RunError(TInt aError);
    	void DoCancel();
    	
    private:
    	enum TRegistryState
    	{
    		EFinding = 0,
    		EGetting
    	};
    	TBTRegistrySearch* iSearch;
    	CBTRegistryResponse*    iResponse;
    	RBTRegistry iRegistryView;
    	RBTRegServ iRegServer;
    	RBTDeviceArray* iDeviceArray;
    	TRegistryState iRegistryState;
    	MBTGPSDevManObserver* iObserver;
    };
    
#endif
// End of File

