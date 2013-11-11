/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LBSNETREGSTATUS_H
#define LBSNETREGSTATUS_H

#include <e32std.h>
#include <e32property.h>

/**
@file
@internalTechnology
@released
*/


/**
*/
NONSHARABLE_CLASS(RLbsNetworkRegistrationStatus)
	{
public:
	/** 
	*/
	enum TLbsNetworkRegistrationStatus
		{
		/** */
		ENetworkRegistrationUnknown,
		
		/** */
		ENotRegistered,
		
		/** */
		ERegisteredHomeNetwork,
		
		/** */
		ERegisteredRoamingNetwork
		};
	
public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();
	
	IMPORT_C void OpenL();
	IMPORT_C void Close();

	IMPORT_C TInt GetNetworkRegistrationStatus(TLbsNetworkRegistrationStatus& aStatus);
	IMPORT_C TInt SetNetworkRegistrationStatus(TLbsNetworkRegistrationStatus aStatus);

	IMPORT_C void NotifyNetworkRegistrationStatusUpdate(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyNetworkRegistrationStatusUpdate();

private:
	static void GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid);
	static void SetCategoryUidL(TUint32 aKey, TUid aCategoryUid);

private:
	RProperty iNetworkRegistrationStatusProperty;
	};

#endif // LBSNETREGSTATUS_H
