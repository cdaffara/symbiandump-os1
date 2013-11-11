// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Definition of extended processing interface
// 
//

/**
 @file 
 @internalComponent 
*/
#ifndef __CEDDUMPEXTENSIONS_H__
#define __CEDDUMPEXTENSIONS_H__

#ifdef __COMMDB_ROHID_SUPPORT__

#include "filedump.h"

GLDEF_C TInt DoExtendedProcessing(FileDump& aLogger);
GLDEF_C void DoExtendedConnPrefRecordProcessing(CCommsDbConnectionPrefTableView* aDbPrefView, FileDump& oFile, TInt& aFieldCount); 
#endif // __COMMDB_ROHID_SUPPORT__

#endif // __CEDDUMPEXTENSIONS_H__
