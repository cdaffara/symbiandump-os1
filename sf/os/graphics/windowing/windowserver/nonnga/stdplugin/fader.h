// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CFADER_H__
#define __CFADER_H__

#include "Graphics/WSPLUGIN.H"

struct TFadingParams
    {
    TUint8 blackMap;
    TUint8 whiteMap;
    };

class MWsGraphicDrawerEnvironment;

class CFader : public CWsPlugin, public MWsFader
  {
public:
	enum { EImplUid = 0x2001B70D };

public:
	static CFader* CreateL();
	~CFader();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	const TDesC& PluginName() const;

public: // from MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);
	
public: // from MWsFader
  virtual void SetFadingParameters(const TDesC8& aData);
	virtual void FadeArea(CFbsBitGc* aBitGc,const TRegion * aRegion);
	
private:
	CFader();

private:
  TUint8 iBlackMap;
  TUint8 iWhiteMap;
  };

#endif //__CFADER_H__
