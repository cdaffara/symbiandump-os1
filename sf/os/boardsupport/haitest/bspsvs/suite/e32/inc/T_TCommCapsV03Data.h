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


#if (!defined __T_TCOMM_CAPS_V03_DATA_H__)
#define __T_TCOMM_CAPS_V03_DATA_H__

//	User Includes
#include "T_TCommCapsV02Data.h"

/**
 * Test Active Notification class
 *
 */
class CT_TCommCapsV03Data : public CT_TCommCapsV02Data
	{
public:
	CT_TCommCapsV03Data();
	~CT_TCommCapsV03Data();

	//	CT_TCommCapsV01Data implementation
	virtual TDes8*	Descriptor();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	//	CT_TCommCapsV01Data implementation
	virtual TCommCapsV01&		GetCommCapsV01();

	//	CT_TCommCapsV02Data implementation
	virtual TCommCapsV02&		GetCommCapsV02();

	virtual TCommCapsV03&		GetCommCapsV03();

private:
	void	DestroyData();

	TInt	DoCmdConstructor();
	void	DoCmdDestructor();
	void	DoCmdiBreakSupported(const TDesC& aSection);

private:
	TCommCaps3*	iCommCaps3;
	};

#endif /* __T_TCOMM_CAPS_V03_DATA_H__ */
