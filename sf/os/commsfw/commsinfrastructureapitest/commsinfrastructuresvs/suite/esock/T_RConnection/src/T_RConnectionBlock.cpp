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


#include "T_RConnectionBlock.h"

CDataWrapper* CT_RConnectionBlock::CreateDataL( const TDesC& aData )
	{
	CDataWrapper* wrapper = NULL;
	if( KDataRConnection() == aData )
		{
		wrapper = CT_DataRConnection::NewL();
		}
	else if(KDataRSocket() == aData)
		{
		wrapper =CT_DataRSocket::NewL();
		}
	else if(KDataRSocketServ() == aData)
		{
		wrapper = CT_DataRSocketServ::NewL();
		}
	return wrapper;
	}

