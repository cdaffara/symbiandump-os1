/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#if (!defined __T_MMCSDSOCKET_DRIVER_DATA_H__)
#define __T_MMCSDSOCKET_DRIVER_DATA_H__


//	User Includes
#include "T_MmcSDDriverData.h"

/**
 * Test Active Notification class
 *
 */
class CT_MMCSDSocketDriverData : public CT_MmcSDDriverData
	{
public:
	/**
	* Public destructor
	*/
	~CT_MMCSDSocketDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_MMCSDSocketDriverData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMCSDSocketDriverData();
	void	ConstructL();

	void 	DoCmdAdjustPartialRead(const TDesC& aSection);
	void 	DoCmdCardIsPresent(const TDesC& aSection);
	void 	DoCmdGetBufferInfo(const TDesC& aSection);
	void 	DoCmdInit();
	void 	DoCmdInitiatePowerUpSequence();
	void 	DoCmdMachineInfo(const TDesC& aSection);
	void 	DoCmdPrepareStore(const TDesC& aSection);
	void 	DoCmdReset1();
	void 	DoCmdReset2();
	void  	DoCmdResetInactivity(const TDesC& aSection);
	void  	DoCmdStack(const TDesC& aSection);
	void 	DoCmdiState_Get(const TDesC& aSection);
	void 	DoCmdiType_Get(const TDesC& aSection);
	void 	DoCmdiDoorOpened_Get(const TDesC& aSection);
	void 	DoCmdiStandby_Get(const TDesC& aSection);
	void 	DoCmdPowerUpL(const TDesC& aSection, TInt aAsyncErrorIndex);

private:
	static const TEnumEntryTable 	iEnumTRequestIdTable[];	
	static const TEnumEntryTable 	iEnumTPBusStateTable[];	
	static const TEnumEntryTable 	iEnumTPBusTypeTable[];	

	};

#endif /* __T_MMCSDSOCKET_DRIVER_DATA_H__ */
