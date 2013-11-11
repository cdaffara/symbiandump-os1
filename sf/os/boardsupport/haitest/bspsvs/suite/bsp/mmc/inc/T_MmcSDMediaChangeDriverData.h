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


#if (!defined __T_MMCSDMEDIACHANGE_DRIVER_DATA_H__)
#define __T_MMCSDMEDIACHANGE_DRIVER_DATA_H__


//	User Includes
#include "T_MmcSDDriverData.h"

/**
 * Test Active Notification class
 *
 */
class CT_MMCSDMediaChangeDriverData : public CT_MmcSDDriverData
	{
public:
	/**
	* Public destructor
	*/
	~CT_MMCSDMediaChangeDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_MMCSDMediaChangeDriverData*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMCSDMediaChangeDriverData();
	void	ConstructL();

	void	DoCmdMediaState(const TDesC& aSection);
	void	DoCmdForceMediaChange();
	void	DoCmdReplayCount(const TDesC& aSection);

private:
	static const TEnumEntryTable	iEnumMediaState[];
	TInt							iAsyncErrorIndex;
	CConsoleBase* 					iCons;
	};

#endif /* __T_MMCSDMEDIACHANGE_DRIVER_DATA_H__ */

