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


#if (!defined __T_RHANDLE_BASE_DATA_H__)
#define __T_RHANDLE_BASE_DATA_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <e32cmn.h>

/**
 * Test Active Notification class
 *
 */
class CT_RHandleBaseData : public CDataWrapperBase
	{
public:
	CT_RHandleBaseData();
	~CT_RHandleBaseData();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	virtual RHandleBase*	GetHandleBase() = 0;

	void	DoCmdClose();
	void	DoCmdFullName(const TDesC& aSection);
	void	DoCmdName(const TDesC& aSection);
	};

#endif /* __T_RHANDLE_BASE_DATA_H__ */
