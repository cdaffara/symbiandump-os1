// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent.
*/

#if !defined(AFX_SERIALCONNECTION_H__4888156F_F2B4_11D4_8077_DC3329DB2D48__INCLUDED_)
#define AFX_SERIALCONNECTION_H__4888156F_F2B4_11D4_8077_DC3329DB2D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class CSerialConnection  
{
public:

	CSerialConnection();
	~CSerialConnection();

	BOOL PortInitialize (CString strPort);

protected:
	static UINT PortReadThread(LPVOID lpvoid);
};

#endif // !defined(AFX_SERIALCONNECTION_H__4888156F_F2B4_11D4_8077_DC3329DB2D48__INCLUDED_)

