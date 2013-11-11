// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// FONT related classes and font cache
// 
//

#ifndef __WSFONT_H__
#define __WSFONT_H__

#include <e32std.h>
#include <w32std.h>
#include "w32cmd.h"
#include <bitstd.h>
#include <bitdev.h>
#include <e32svr.h>
#include "OBJECT.H"
#include "CLIENT.H"
#include "server.h"

class CWsAnim;

class CAnimFbsFont : public CFbsBitGcFont
	{
	friend class CWsAnim;
public:
	static CAnimFbsFont* NewL(TInt aHandle,TInt& aError);
	void Open();
	void Close();
	inline TInt Duplicate(TInt aHandle);
private:
	CAnimFbsFont();
	~CAnimFbsFont();
private:
	TInt iAccessCount;
	};

class CWsFontCache;

class CWsFbsFont : public CFbsBitGcFont
	{
public:
	~CWsFbsFont();
	CWsFbsFont();
	inline TInt Duplicate(TInt aHandle);
	inline TInt Handle() const;
	inline void Reset();
public:
	TDblQueLink iLink;
	TInt iCount;
	};

class CWsFontCache : public CBase
	{
	enum {EFontCacheSize=128};
public:
	static void CreateInstanceL();
	static void DestroyInstance();
	static CWsFontCache * Instance() { return iSelf; }
	TBool UseFont(CWsFbsFont *&aFont, TInt aHandle);
	void ReleaseFont(CWsFbsFont *&aFont);
	static TDblQue<CWsFbsFont>& List();
private:
	CWsFontCache();
	~CWsFontCache();
private:
	static CWsFontCache * iSelf;
	static TDblQue<CWsFbsFont> * iList;
	CWsFbsFont iFont[EFontCacheSize];
	};

inline TInt CWsFbsFont::Duplicate(TInt aHandle)
	{return(CFbsBitGcFont::Duplicate(aHandle));}
inline TInt CWsFbsFont::Handle() const
	{return(CFbsBitGcFont::Handle());}
inline void CWsFbsFont::Reset()
	{CFbsBitGcFont::Reset();}
//
inline TInt CAnimFbsFont::Duplicate(TInt aHandle)
	{return(CFbsFont::Duplicate(aHandle));}

#endif
