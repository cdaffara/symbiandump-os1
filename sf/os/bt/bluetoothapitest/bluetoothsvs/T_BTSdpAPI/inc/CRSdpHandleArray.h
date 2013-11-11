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


#if	(!defined __C_RSDP_HANDLE_ARRAY_H__ )
#define	__C_RSDP_HANDLE_ARRAY_H__

//	User Includes
#include <btsdp.h>


class CRSdpHandleArray :	public CBase
	{
public:
	RArray<TUint>		iRecHandleArray;
	RSdpDatabase		iDatabase;
	};

#endif /* __C_RSDP_HANDLE_ARRAY_H__ */
