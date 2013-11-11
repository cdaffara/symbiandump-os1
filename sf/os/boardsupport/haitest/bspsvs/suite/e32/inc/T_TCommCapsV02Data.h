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


#if (!defined __T_TCOMM_CAPS_V02_DATA_H__)
#define __T_TCOMM_CAPS_V02_DATA_H__

//	User Includes
#include "T_TCommCapsV01Data.h"

/**
 * Test Active Notification class
 *
 */
class CT_TCommCapsV02Data : public CT_TCommCapsV01Data
	{
public:
	CT_TCommCapsV02Data();
	~CT_TCommCapsV02Data();

	//	CT_TCommCapsV01Data implementation
	virtual TDes8*	Descriptor();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	//	CT_TCommCapsV01Data implementation
	virtual TCommCapsV01&		GetCommCapsV01();

	virtual TCommCapsV02&		GetCommCapsV02();

private:
	void	DestroyData();

	TBool 	ReadFlowControlCaps(const TDesC& aSection, const TDesC& aKeyName, TUint& aFlowControlCaps);
	TBool 	ReadNotificationCaps(const TDesC& aSection, const TDesC& aKeyName, TUint& aNotificationCaps);
	TBool 	ReadRoleCaps(const TDesC& aSection, const TDesC& aKeyName, TUint& aRoleCaps);

	TInt	DoCmdConstructor();
	void	DoCmdDestructor();
	void	DoCmdiFlowControlCaps(const TDesC& aSection);
	void	DoCmdiNotificationCaps(const TDesC& aSection);
	void	DoCmdiRoleCaps(const TDesC& aSection);

private:
	TCommCaps2*	iCommCaps2;
	static const TEnumEntryTable	iEnumTableFlowControlCaps[];
	static const TEnumEntryTable	iEnumTableNotificationCaps[];
	static const TEnumEntryTable	iEnumTableRoleCaps[];
	};

#endif /* __T_TCOMM_CAPS_V02_DATA_H__ */
