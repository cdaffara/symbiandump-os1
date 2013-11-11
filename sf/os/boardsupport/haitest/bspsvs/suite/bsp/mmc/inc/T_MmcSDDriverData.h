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


#if (!defined __T_MMCSD_DRIVER_DATA_H__)
#define __T_MMCSD_DRIVER_DATA_H__


//	User Includes
#include "T_RBusLogicalChannelData.h"
#include "d_mmcsdif.h"
#include "ActiveCallbackWrap.h"

/**
 * Test Active Notification class
 *
 */


class CT_MmcSDDriverData : public CDataWrapperBase
	{
public:
	/**
	* Public destructor
	*/
	~CT_MmcSDDriverData();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MmcSDDriverData();
	void	ConstructL();

	void	DoCmdConstructorL(const TDesC& aSection);
	void	DoCmdDestructor();

	TInt 	GetExpectedAsyncError(const TDesC& aSection);
	void 	DoCmdReadSectorL(const TDesC& aSection, TInt aAsyncErrorIndex);
	void 	DoCmdWriteSectorL(const TDesC& aSection, TInt aAsyncErrorIndex);
	void 	DoCmdReadCSD();
	void 	DoCmdReadExtCSD();
	void 	DoCmdReadCSDL(const TDesC& aSection, TInt aAsyncErrorIndex);
	void 	DoCmdReadExtCSDL(const TDesC& aSection, TInt aAsyncErrorIndex);
	void 	DoCmdCardInfo(const TDesC& aSection);
	void 	DoCmdCardIsPresent(const TDesC& aSection);
	void 	DoCmdCardIsReady(const TDesC& aSection);
	void 	DoCmdCardIsLocked(const TDesC& aSection);
	void 	PrintCSD(const TCSDInfo& aCSDInfo);
	void 	PrintExtendedCSD(const TExtendedCSDInfo& aExtendedCSDInfo);

	
protected:
	void RunL(CActive* aActive, TInt aIndex);
	void DoCancel(CActive* aActive);
	CActiveCallbackWrap* CreateActiveCallbackL(TInt aFunctionId, TInt aExpectedAsyncError);
	void DeleteActiveCallback(CActiveCallback* activeCallbackWrap);

protected:
	RMMCSDTestControllerInterface*	iMmcSDController;
	RPointerArray<CActiveCallbackWrap>	iActiveCallbacks;

private:
	TInt							iStackNum;
	TInt							iAsyncErrorIndex;
	RFs								iFs;
	};

#endif /* __T_MMCSD_DRIVER_DATA_H__ */
