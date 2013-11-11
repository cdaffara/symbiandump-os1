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


#if (!defined __T_MMCSD_SESSION_DATA_H__)
#define __T_MMCSD_SESSION_DATA_H__


//	User Includes
#include "T_RBusLogicalChannelData.h"
#include "d_mmcsdif.h"

/**
 * Test Active Notification class
 *
 */


class CT_MmcSDSessionData : public CT_RBusLogicalChannelData
	{
public:
	/**
	* Public destructor
	*/
	~CT_MmcSDSessionData();
	
	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_MmcSDSessionData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MmcSDSessionData();
	void	ConstructL();

	//	CT_RHandleBaseData implementation
	virtual RHandleBase*		GetHandleBase();

	//	CT_RBusLogicalChannelData implementation
	virtual RBusLogicalChannel*	GetBusLogicalChannel();

	void	DoCmdConstructor();
	void	DoCmdLoadDriver(const TDesC& aSection);
	void	DoCmdDriverOpen(const TDesC& aSection);
	void	DoCmdUnLoadDriver(const TDesC& aSection);
	void	DoCmdDestructor();

private:
	void	DestroyData();

protected:
	RMMCSDTestControllerInterface*	iMmcSDController;

private:
	};

#endif /* __T_MMCSD_SESSION_DATA_H__ */
