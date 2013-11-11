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
// INCLUDE FILES
//

#include <e32std.h>
#include <a3f/devsoundadaptationinfo.h>
#include "devsoundadaptationinfoimpl.h"
#include "devsoundadaptationinfoimplbody.h"

//-------------------------------------------------------------------
//	CA3FDevSoundAdaptationInfo* CA3FDevSoundAdaptationInfo::NewL
//	Two-phased constructor.
//-------------------------------------------------------------------
//
EXPORT_C CA3FDevSoundAdaptationInfo* CA3FDevSoundAdaptationInfo::NewL(MA3FDevSoundAdaptationInfoObserver& aAdaptationInfoObserver, CFourCCConvertor& aFourCcConvertor)
	{
	DP_STATIC_CONTEXT(CA3FDevSoundAdaptationInfo::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CA3FDevSoundAdaptationInfoImpl* self = new(ELeave) CA3FDevSoundAdaptationInfoImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aAdaptationInfoObserver, aFourCcConvertor);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}
	
//------------------------------------------------------------
// 	CA3FDevSoundAdaptationInfo::CA3FDevSoundAdaptationInfo
//	C++ default constructor.
//------------------------------------------------------------
//
CA3FDevSoundAdaptationInfo::CA3FDevSoundAdaptationInfo()
	{
	}

//--------------------------------------------------------------
//	CA3FDevSoundAdaptationInfo::~CA3FDevSoundAdaptationInfo
//	Destructor
//--------------------------------------------------------------
//
CA3FDevSoundAdaptationInfo::~CA3FDevSoundAdaptationInfo()
	{
	}
	
//------------------------------------------------------------
// 	CA3FDevSoundAdaptationInfoImpl::CA3FDevSoundAdaptationInfo
//	C++ default constructor.
//------------------------------------------------------------
//
CA3FDevSoundAdaptationInfoImpl::CA3FDevSoundAdaptationInfoImpl()
	{
	}

//--------------------------------------------------------------
//	CA3FDevSoundAdaptationInfo::~CA3FDevSoundAdaptationInfo
//	Destructor
//--------------------------------------------------------------
//
CA3FDevSoundAdaptationInfoImpl::~CA3FDevSoundAdaptationInfoImpl()
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfo::~CA3FDevSoundAdaptationInfo *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	delete iBody;
	DP_OUT();
	}
	
//--------------------------------------------------------------
//	CA3FDevSoundAdaptationInfoImpl::ConstructL
//	2nd phase constructor
//--------------------------------------------------------------
//
void CA3FDevSoundAdaptationInfoImpl::ConstructL(MA3FDevSoundAdaptationInfoObserver& aAdaptationInfoObserver, CFourCCConvertor& aFourCcConvertor)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfo::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iBody = CBody::NewL(aAdaptationInfoObserver, aFourCcConvertor);
	DP_OUT();
	}
	
TInt CA3FDevSoundAdaptationInfoImpl::RequestMaxGain(TUid aCodecType)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::RequestMaxGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt result = iBody->RequestMaxGain(aCodecType);
	DP0_RET(result, "%d");
	}
	
TInt CA3FDevSoundAdaptationInfoImpl::RequestSupportedFormats(TUid aCodecType, RArray<TUid>& aSupportedFormats)
	{
	DP_CONTEXT(CA3FDevSoundAdaptationInfoImpl::RequestSupportedFormats *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt result = iBody->RequestSupportedFormats(aCodecType, aSupportedFormats);
	DP0_RET(result, "%d");
	}

TInt CA3FDevSoundAdaptationInfoImpl::Extension_(TUint aExtensionId, TAny*& aExtPtr, TAny*)
	{
	return iBody->Extension_(aExtensionId, aExtPtr, NULL); 
	}

//--------------------------------------------------------------

// End of file
