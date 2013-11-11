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


#if (!defined __T_RSESSION_BASE_DATA_H__)
#define __T_RSESSION_BASE_DATA_H__

//	User Includes
#include "T_RHandleBaseData.h"

//	EPOC includes
#include <e32std.h>

/**
 * Test Active Notification class
 *
 */
class CT_RSessionBaseData : public CT_RHandleBaseData
	{
public:
	CT_RSessionBaseData();
	~CT_RSessionBaseData();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	virtual RSessionBase*	GetSessionBase() = 0;

	TInt	DoCmdShareAuto();
	TInt	DoCmdShareProtected();
	};

#endif /* __T_RSESSION_BASE_DATA_H__ */
