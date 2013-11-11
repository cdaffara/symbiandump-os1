// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LBS BT GPS Positioner Class, which is used by the tests to make location 
// requests and get called back with location updates
//

#ifndef __CT_LBSBTGPSPOSITIONER_H__
#define __CT_LBSBTGPSPOSITIONER_H__

#include <lbs.h>


/** Positioner observer interface 
*/
class MBtGpsPositionerObserver
	{
public:
	virtual void OnPositionUpdate(const TPosition& aPosition) = 0;
	virtual void OnPositionError(TInt aError) = 0;
	};


/** Positioner class used to make BT GPS location updates for the test
*/
class CBtGpsPositioner : public CActive
	{
public:
	static CBtGpsPositioner* NewL(MBtGpsPositionerObserver& aObserver);
	~CBtGpsPositioner();

	void StartLocationRequest();
	
	void OpenL();
	void EnableL();
	RPositioner& Positioner();
	
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
		
private:
	CBtGpsPositioner(MBtGpsPositionerObserver& aObserver);
	void ConstructL();
	
private:
	RPositionServer iPositionServer;
	RPositioner iPositioner;
	MBtGpsPositionerObserver& iObserver;
	TPositionInfo iPosition;
	};


#endif	//__CT_LBSBTGPSPOSITIONER_H__
