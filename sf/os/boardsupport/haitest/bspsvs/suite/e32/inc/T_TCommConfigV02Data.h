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


#if (!defined __T_TCOMM_CONFIG_V02_DATA_H__)
#define __T_TCOMM_CONFIG_V02_DATA_H__

//	User Includes
#include "T_TCommConfigV01Data.h"

/**
 * Test Active Notification class
 *
 */
class CT_TCommConfigV02Data : public CT_TCommConfigV01Data
	{
public:
	CT_TCommConfigV02Data();
	~CT_TCommConfigV02Data();

	//	CT_TCommConfigV01Data implementation
	virtual TDes8*	Descriptor();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	//	CT_TCommConfigV01Data implementation
	virtual TCommConfigV01&		GetCommConfigV01();

	virtual TCommConfigV02&		GetCommConfigV02();

private:
	void	DestroyData();

	TInt	DoCmdConstructor();
	void	DoCmdDestructor();
	void	DoCmdiTxShutdownTimeout(const TDesC& aSection);

private:
	TCommConfig2*	iCommConfig2;
	};

#endif /* __T_TCOMM_CONFIG_V02_DATA_H__ */
