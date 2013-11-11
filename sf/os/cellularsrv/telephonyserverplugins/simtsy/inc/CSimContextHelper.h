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
// CSimContextHelper.cpp
// Useful routines for using and related PDP contexts, that may used in multiple files. 
// 
//

/**
 @file
*/
 
 
#ifndef __CSIMCONTEXTHELPER_H__
#define __CSIMCONTEXTHELPER_H__

#include <etelpckt.h>
#include "CSimPacketContext.h"
 
class CTestConfigSection;
 
/**
Useful routines for helping with PDP contexts.
*/    
class CSimContextHelper
{
 	
public :

	static void SetTContextConfigParamFromTContextConfigGPRS( TContextConfigParam& aContextConfigToSet , const RPacketContext::TContextConfigGPRS&  aContextConfigV1 );
	static void SetTContextConfigParamFromTContextConfigR99_R4( TContextConfigParam& aContextConfigToSet, const RPacketContext::TContextConfigR99_R4& aContextConfigV1 );
	static void SetTContextConfigParamFromTContextConfig_R5( TContextConfigParam& aContextConfigToSet, const RPacketContext::TContextConfig_R5& aContextConfigV1 );
	static TBool EqualTContextConfigParam(const TContextConfigParam& aContextConfigLHS, const TContextConfigParam& aContextConfigRHS );
	static void GetContextConfigParamSettingsL( const CTestConfigSection* const aCfgFile, const TPtrC8 aTag, 
	   CArrayFixFlat<TContextConfigParam>* aContextConfigParams );														   
	static void ConvertConfigParams(const TContextConfigParam& aInput, RPacketContext::TContextConfigGPRS& aOutput);
	static void ConvertConfigParams(const TContextConfigParam& aInput, RPacketContext::TContextConfigR99_R4& aOutput);
	static void ConvertConfigParams(const TContextConfigParam& aInput, RPacketContext::TContextConfig_R5& aOutput);
														   
 	
};
 
#endif	//__CSIMCONTEXTHELPER_H__
