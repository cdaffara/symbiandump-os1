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


#if (!defined __T_MMCSDSTACK_DRIVER_DATA_H__)
#define __T_MMCSDSTACK_DRIVER_DATA_H__


//	User Includes
#include "T_MmcSDDriverData.h"

/**
 * Test Active Notification class
 *
 */
class CT_MMCSDStackDriverData : public CT_MmcSDDriverData
	{
public:
	/**
	* Public destructor
	*/
	~CT_MMCSDStackDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_MMCSDStackDriverData*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMCSDStackDriverData();
	void	ConstructL();

private:

	void 	DoCmdAdjustPartialRead(const TDesC& aSection);
	void 	DoCmdAllocSession();
	void 	DoCmdDemandPagingInfo(const TDesC& aSection);
	void 	DoCmdEffectiveModes(const TDesC& aSection);
	void 	DoCmdBufferInfo(const TDesC& aSection);
	void 	DoCmdGetBufferInfo(const TDesC& aSection);
	void 	DoCmdHasCardsPresent(const TDesC& aSection);
	void 	DoCmdHasSessionsQueued(const TDesC& aSection);
	void 	DoCmdInitStackInProgress(const TDesC& aSection);
	void 	DoCmdMMCSocketL(const TDesC& aSection);
	void 	DoCmdMachineInfo(const TDesC& aSection);
	void 	DoCmdMaxCardsInStack(const TDesC& aSection);
	void 	DoCmdReadPasswordStore(const TDesC& aSection);
	void 	DoCmdWritePasswordStore(const TDesC& aSection);
	void 	DoCmdProgramPeriodInMilliSeconds(const TDesC& aSection);
	void 	DoCmdStop(const TDesC& aSection);
	void 	DoCmdInit();
	void 	DoCmdReportPowerUp(const TDesC& aSection);
	void 	DoCmdReportPowerDown(const TDesC& aSection);
	void 	DoCmdPasswordStore(const TDesC& aSection);
	void 	DoCmdCardP(const TDesC& aSection);
	void 	DoCmdAlterEffectiveModes(const TDesC& aSection);
	
private:

	static const TEnumEntryTable 	iEnumEffectiveModesFunctionIdTable[];	
	
	};

#endif /* __T_MMCSDSTACK_DRIVER_DATA_H__ */
