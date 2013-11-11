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
// Contains wrapper helper functions for test code to control the test wrappers 
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef ADAPTATIONCLIWRAPPER_H_
#define ADAPTATIONCLIWRAPPER_H_

class AdaptationCliWrapper
{
public:
	static void SetRSsmAdaptationBaseNextConnectReturn(TInt aRet);
	static TInt GetRSsmAdaptationBaseNextConnectReturn();
	
	static void SetRSsmRtcAdaptationNextValidity(TBool aValidity);
	static TBool GetRSsmRtcAdaptationNextValidity();

	static void SetRSsmRtcAdaptationNextValidityReturn(TInt aRet);
	static TInt GetRSsmRtcAdaptationNextValidityReturn();	
	
	static void SetRSsmEmergencyCallReturnValue(TInt aRet);
	static TInt GetRSsmEmergencyCallReturnValue();
	
	static void SetSimOwnedReturnValue(TInt aRet);
	static TInt GetSimOwnedReturnValue();

	static void SetSimOwned(TBool aOwned);
	static TBool GetSimOwned();
	
	static void SetSimChangedReturnValue(TInt aRet);
	static TInt GetSimChangedReturnValue();	

	static void SetSimChanged(TBool aChanged);
	
	static void SetSimNotifyStatus(TRequestStatus* aStatus);
	static TRequestStatus* GetSimNotifyStatus();
	
	static void SetSimEventPckg(TDes8* aPckg);
	static TDes8* GetSimEventPckg();
	
private:
	static TInt iSimChangedReturn;
	static TInt iSimOwnedReturn;
	static TRequestStatus* iSimNotifyStatus;
	static TBool iSimChanged;
	static TBool iSimOwned;
	static TDes8* iSimEventPckg;
	static TInt iSsmAdaptationBaseNextConnect;
	static TBool iSsmRtcAdaptationNextValidity;
	static TInt iSsmRtcAdaptationNextValidityReturn;
	static TInt iSsmEmergencyCallReturnValue;
};

#endif /*ADAPTATIONCLIWRAPPER_H_*/
