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

#include "fader.h"

_LIT(CFaderName, "wsfader");
    
CFader* CFader::CreateL()
	{
	return new(ELeave) CFader;
	}
	
void CFader::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	//Default bitgdi fading parameters
	iBlackMap=128;
	iWhiteMap=255;
	}
	
CFader::CFader()
	{
	}
	
CFader::~CFader()
	{
	}

//Default fading plugin simply uses bitgdi to perform fading
void CFader::FadeArea(CFbsBitGc* aBitGc,const TRegion * aRegion)
	{
  	aBitGc->Reset();
  	aBitGc->SetFadingParameters(iBlackMap,iWhiteMap);
	aBitGc->FadeArea(aRegion);
	}
	
//Default fading plugin expects two TUint8's describing the black/white map 
//as possible fading parameters
void CFader::SetFadingParameters(const TDesC8& aData)
  	{
	TPckgBuf<TFadingParams> buf;
	buf.Copy(aData);
	TFadingParams parameters = buf();

	iBlackMap = parameters.blackMap;
	iWhiteMap = parameters.whiteMap;
  	}

TAny* CFader::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case MWsFader::EWsObjectInterfaceId:
			return static_cast<MWsFader*>(this);
		}

	return NULL;
	}

const TDesC& CFader::PluginName() const
	{
	return CFaderName;
	}
