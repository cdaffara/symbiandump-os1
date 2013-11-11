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


#if (!defined __T_MMCSDPSUBASE_DRIVER_DATA_H__)
#define __T_MMCSDPSUBASE_DRIVER_DATA_H__

//	User Includes
#include "T_MmcSDDriverData.h"

/**
 * Test MMCSD PowerSupply class
 *
 */
class CT_MMCSDPsuBaseDriverData : public CT_MmcSDDriverData
	{
public:
	/**
	* Public destructor
	*/
	~CT_MMCSDPsuBaseDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_MMCSDPsuBaseDriverData*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMCSDPsuBaseDriverData();
	void	ConstructL();

	void	DoCmdIsLocked(const TDesC& aSection);
	void 	DoCmdIsOff(const TDesC& aSection);
	void	DoCmdMaxCurrentInMicroAmps(const TDesC& aSection);
	void	DoCmdResetInactivityTimer();
	void 	DoCmdVoltageSupported(const TDesC& aSection);
	void 	DoCmdInactivityCount(const TDesC& aSection);
	void 	DoCmdNotLockedCount(const TDesC& aSection);
	void 	DoCmdInactivityTimeout(const TDesC& aSection);
	void 	DoCmdNotLockedTimeout(const TDesC& aSection);
	void 	DoCmdMemVoltageSupported(const TDesC& aSection);
	void 	DoCmdMemMaxCurrentInMicroAmps(const TDesC& aSection);
	void 	DoCmdVoltCheckInterval(const TDesC& aSection);

private:
	TInt	iAsyncErrorIndex;
	};

#endif /* __T_MMCSDPSUBASE_DRIVER_DATA_H__ */
