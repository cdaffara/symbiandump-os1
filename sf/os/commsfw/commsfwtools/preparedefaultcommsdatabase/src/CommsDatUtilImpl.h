/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Comms Data Utilities exported by CommsDat in addition to general database access activities.
* All Data PublishedAll
* 
*
*/



/**
 @file CCommsDatUtilImpl.h
*/
 
#if (!defined CCOMMSDATUTILIMPL_H)
#define CCOMMSDATUTILIMPL_H

#include <dial.h>

const TText KStarChar='*';
const TText KPlusChar='+';
	
NONSHARABLE_CLASS(CCommsDatUtilImpl) : public CBase
	{
	public:

		CCommsDatUtilImpl(){}    
		~CCommsDatUtilImpl();
		
		
		void ResolvePhoneNumberL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId);
		void ResolvePhoneNumberFromDatabaseL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId);
		TInt CheckReadCapability( const TDesC& aField, const RMessagePtr2* aMessage );
		TInt CheckWriteCapability( const TDesC& aField, const RMessagePtr2* aMessage );

	private:
		void PopulateDialLocationL(TUint32 aId, TDialLocation& aLocation);
		void PopulateChargeCardL(TUint32 aId, TChargeCard& aChargeCard);

	private:
		CommsDat::CMDBSession* idbSession;
	};

#endif
