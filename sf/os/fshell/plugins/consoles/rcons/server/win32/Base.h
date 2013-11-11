// Base.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// Description:
// Base.h: interface for the CBase class.

#if !defined(AFX_BASE_H__531B81D8_F017_4998_92BA_35CB284A9B50__INCLUDED_)
#define AFX_BASE_H__531B81D8_F017_4998_92BA_35CB284A9B50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CBase  
	{
public:
	virtual ~CBase();
	void SetName(LPCTSTR aName);
protected:
	CBase();
private:
	LPCTSTR iName;
	};

#endif // !defined(AFX_BASE_H__531B81D8_F017_4998_92BA_35CB284A9B50__INCLUDED_)
