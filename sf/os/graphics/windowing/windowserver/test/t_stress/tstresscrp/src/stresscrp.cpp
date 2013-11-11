// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecom/implementationproxy.h>
#include <graphics/wsgraphicdrawer.h>
#include <graphics/wsgraphicdrawerinterface.h>
#include <graphics/wsgraphicscontext.h>
#include "stresscrp.h"

NONSHARABLE_CLASS(CStressCRP): public CWsGraphicDrawer
	{
public:
	static CStressCRP* CreateL();
	enum
		{
		EImplUid = 0xa0005923
		};
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData);
	void HandleMessage(const TDesC8& aData);
private:
	void DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const;
	mutable MWsGraphicDrawerEnvironment* iEnv;
	};

CStressCRP* CStressCRP::CreateL()
	{
	return new(ELeave) CStressCRP;
	}
	
void CStressCRP::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv,aId,aOwner);
	iEnv = &aEnv;
	}
	
void CStressCRP::HandleMessage(const TDesC8&/* aData*/)
	{
	}
	
void CStressCRP::DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& /*aData*/) const
	{
	MWsGraphicsContext* context = static_cast<MWsGraphicsContext*>(aGc.ResolveObjectInterface(KMWsGraphicsContext));
	if(context) //NGA
		StressCrpNgaContext::Draw(aGc, aRect);
	else //NON NGA
		StressCrpNonNgaContext::Draw(aGc, aRect);
	}	
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)  //lint -e714 Suppress 'not referenced' 
	{
	static const TImplementationProxy KImplementationTable[] = 
		{
		IMPLEMENTATION_PROXY_ENTRY(CStressCRP::EImplUid,CStressCRP::CreateL)	//lint !e611 Suspicious cast
		};
	aTableCount = (sizeof(KImplementationTable) / sizeof(TImplementationProxy));
	return KImplementationTable;
	}
