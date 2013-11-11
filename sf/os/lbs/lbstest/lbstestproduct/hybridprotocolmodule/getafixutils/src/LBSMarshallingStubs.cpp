/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// LBSMarshallingStubs.cpp
//
//

#include "LBSMarshallingStubs.h"
#include <s32mem.h>
//-------------------------------------------------------------------------------------------------
EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsNetSessionId& aParam)
	{
	// Only one sub-class defined: ELbsNetSessionIdClass
	__ASSERT_ALWAYS(aParam.ClassType() == ELbsNetSessionIdClass, User::Invariant());
	
	aPipe << (TInt32)ELbsNetSessionIdClass;
	
	TPckgBuf<TLbsNetSessionId> data(aParam);
	aPipe << data;
	}


EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestPrivacy& aParam)
	{
	// Only one sub-class defined: 	ELbsNetPosRequestPrivacyClass
	__ASSERT_ALWAYS(aParam.ClassType() == ELbsNetPosRequestPrivacyClass, User::Invariant());
	
	aPipe << (TInt32)ELbsNetPosRequestPrivacyClass;
	
	TPckgBuf<TLbsNetPosRequestPrivacy> data(aParam);
	aPipe << data;
	}

EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestQuality& aParam)
	{
	// Only one sub-class defined: 	ELbsNetPosRequestQualityClass
	__ASSERT_ALWAYS(aParam.ClassType() == ELbsNetPosRequestQualityClass, User::Invariant());
	
	aPipe << (TInt32)ELbsNetPosRequestQualityClass;
	
	TPckgBuf<TLbsNetPosRequestQuality> data(aParam);
	aPipe << data;
	}
	
EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestOptionsBase& aParam)
	{
	// Has three defined sub-classes: ELbsNetPosRequestOptionsBaseClass,
	//								  ELbsNetPosRequestOptionsClass and 
	//								  ELbsNetPosRequestOptionsAssistanceClass
	
	switch(aParam.ClassType())
		{
		case ELbsNetPosRequestOptionsBaseClass:
			{
			aPipe << (TInt32)ELbsNetPosRequestOptionsBaseClass;
		
			TPckgBuf<TLbsNetPosRequestOptionsBase> data(aParam);
			aPipe << data;
			break;
			}			
		case ELbsNetPosRequestOptionsClass:
			{
			aPipe << (TInt32)ELbsNetPosRequestOptionsClass;
		
			TPckgBuf<TLbsNetPosRequestOptions> data((TLbsNetPosRequestOptions&)aParam);
			aPipe << data;
			break;
			}			
				
		case ELbsNetPosRequestOptionsAssistanceClass:
			{
			aPipe << (TInt32)ELbsNetPosRequestOptionsAssistanceClass;
		
			TPckgBuf<TLbsNetPosRequestOptionsAssistance> data((TLbsNetPosRequestOptionsAssistance&)aParam);
			aPipe << data;
			break;
			}			
		
		default:
			User::Invariant();
			break;
		}
	}

EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsNetPosMethod& aParam)
	{
	// Only one sub-class defined: ELbsNetPosMethodClass
	__ASSERT_ALWAYS(aParam.ClassType() == ELbsNetPosMethodClass, User::Invariant());
	
	aPipe << (TInt32)ELbsNetPosMethodClass;
	
	TPckgBuf<TLbsNetPosMethod> data(aParam);
	aPipe << data;
	}
	
EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsNetPosCapabilities& aParam)
	{
	// Only one sub-class defined: ELbsNetPosCapabilitiesClass
	__ASSERT_ALWAYS(aParam.ClassType() == ELbsNetPosCapabilitiesClass, User::Invariant());
	
	aPipe << (TInt32)ELbsNetPosCapabilitiesClass;
	
	TPckgBuf<TLbsNetPosCapabilities> data(aParam);
	aPipe << data;
	}
	
EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestMethod& aParam)
	{
	// Only one sub-class defined: TLbsNetPosRequestMethodClassType
	__ASSERT_ALWAYS(aParam.ClassType() == ELbsNetPosRequestMethodClass, User::Invariant());
	
	aPipe << (TInt32)ELbsNetPosRequestMethodClass;
	
	TPckgBuf<TLbsNetPosRequestMethod> data(aParam);
	aPipe << data;
	}

EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TLbsExternalRequestInfo& aParam)
	{
	// Unknown if and where any sub-classes are defined at this time - TTD CHECK!
//???	__ASSERT_ALWAYS(aParam.ClassType() == TLbsNetPosRequestMethodClassType, User::Invariant());
	
//???	aPipe << (TInt32)TLbsNetPosRequestMethodClassType;
	
	TPckgBuf<TLbsExternalRequestInfo> data(aParam);
	aPipe << data;
	}
	
EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TInt& aParam)
	{
	aPipe << aParam;
	}
	
EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TUint32& aParam)
	{
	aPipe << (TInt32)aParam;
	}

EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TPositionInfoBase& aParam)
	{
	aPipe << (TInt32)aParam.PositionClassType();
	switch(aParam.PositionClassType())
		{
		case EPositionInfoClass:
			{
			TPckgBuf<TPositionInfo > data((TPositionInfo &)aParam);
			aPipe << data;
			break;
			}
		case EPositionGenericInfoClass: // Not used?
			User::Invariant(); 
			break;
		case (EPositionInfoClass|EPositionCourseInfoClass):
			{	
			TPckgBuf<TPositionCourseInfo > data((TPositionCourseInfo &)aParam);
			aPipe << data;
			break;
			}
		case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass):
			{
			TPckgBuf<TPositionSatelliteInfo > data((TPositionSatelliteInfo &)aParam);
			aPipe << data;
			break;
			}
		case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass):
            {
            TPckgBuf<TPositionExtendedSatelliteInfo > data((TPositionExtendedSatelliteInfo &)aParam);
            aPipe << data;
            break;
            }
		case EPositionGpsMeasurementInfoClass:
			{
			TPckgBuf<TPositionGpsMeasurementInfo> data((TPositionGpsMeasurementInfo &)aParam);
			aPipe << data;
			break;
			}
		default:
			User::Invariant();
			break;
		}
	}
	
EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, RLbsAssistanceDataBuilderSet& aParam)
	{
	TUint32 len = aParam.MaxExternalizedBufferSize();
	
	HBufC8* pBuf = HBufC8::NewMaxLC(len);
	TPtr8 ptr = pBuf->Des();
	
	RDesWriteStream stream(ptr);
	CleanupClosePushL(stream);

	aParam.ExternalizeL(stream);

	CleanupStack::PopAndDestroy();	// stream
	
	__ASSERT_DEBUG(pBuf->Length() > 0, User::Invariant());	// Check the buffer actually has some data in it...
	
	aPipe << len << *pBuf; // This writes the length twice, so that we can read it correctly!
	
	CleanupStack::PopAndDestroy(pBuf);
	}

EXPORT_C void LBSTestMarshaller::WriteL(CWritePropPipe& aPipe, const TDesC16& aParam)
	{
	aPipe << aParam.Length() << aParam;	// Writes the length twice to make reading easier
	}
			
//-------------------------------------------------------------------------------------------------

EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsNetSessionId* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = ELbsNetSessionIdUnknownClass;
	aPipe >> classType;
	
	// Only one sub-class defined; ELbsNetSessionIdClass
	__ASSERT_ALWAYS(classType == ELbsNetSessionIdClass, User::Invariant());
	
	TPckgBuf<TLbsNetSessionId> data;
	aPipe >> data;
	TLbsNetSessionId *pNetSessionId = iMemManager->LbsNetSessionId();
	*pNetSessionId = data();
	aParam = pNetSessionId;
	}
	
EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsNetPosRequestPrivacy* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = ELbsNetPosRequestPrivacyUnknownClass;
	aPipe >> classType;
	
	// Only one sub-class defined; ELbsNetPosRequestPrivacyClass
	__ASSERT_ALWAYS(classType == ELbsNetPosRequestPrivacyClass, User::Invariant());
	
	TPckgBuf<TLbsNetPosRequestPrivacy> data;
	aPipe >> data;
	TLbsNetPosRequestPrivacy *pNetPosRequestPrivacy = iMemManager->LbsNetPosRequestPrivacy();
	*pNetPosRequestPrivacy = data();
	aParam = pNetPosRequestPrivacy;
	}
	
EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsNetPosRequestQuality* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = ELbsNetPosRequestPrivacyUnknownClass;
	aPipe >> classType;
	
	// Only one sub-class defined; ELbsNetPosRequestQualityClass
	__ASSERT_ALWAYS(classType == ELbsNetPosRequestQualityClass, User::Invariant());
	
	TPckgBuf<TLbsNetPosRequestQuality> data;
	aPipe >> data;
	TLbsNetPosRequestQuality *pNetPosRequestQuality = iMemManager->LbsNetPosRequestQuality();
	*pNetPosRequestQuality = data();
	aParam = pNetPosRequestQuality;
	}

EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsNetPosRequestOptionsBase* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = ELbsNetPosRequestOptionsUnknownClass;
	aPipe >> classType;
	
	// Has three defined sub-classes: ELbsNetPosRequestOptionsBaseClass,
	//								  ELbsNetPosRequestOptionsClass and 
	//								  ELbsNetPosRequestOptionsAssistanceClass
	
	switch(classType)
		{
		case ELbsNetPosRequestOptionsBaseClass:
			{
			TPckgBuf<TLbsNetPosRequestOptionsBase> data;
			aPipe >> data;
			TLbsNetPosRequestOptionsBase *pNetPosRequestOptionsBase = iMemManager->LbsNetPosRequestOptionsBase();
			*pNetPosRequestOptionsBase = data();
			aParam = pNetPosRequestOptionsBase;
			break;
			}			
		case ELbsNetPosRequestOptionsClass:
			{
			TPckgBuf<TLbsNetPosRequestOptions> data;
			aPipe >> data;
			TLbsNetPosRequestOptions *pNetPosRequestOptions = iMemManager->LbsNetPosRequestOptions();
			*pNetPosRequestOptions = data();
			aParam = pNetPosRequestOptions;
			break;
			}			
		case ELbsNetPosRequestOptionsAssistanceClass:
			{
			TPckgBuf<TLbsNetPosRequestOptionsAssistance> data;
			aPipe >> data;
			TLbsNetPosRequestOptionsAssistance *pNetPosRequestOptionsAssistance = iMemManager->LbsNetPosRequestOptionsAssistance();
			*pNetPosRequestOptionsAssistance = data();
			aParam = pNetPosRequestOptionsAssistance;
			break;
			}			
		
		default:
			User::Invariant();
			break;
		}
	}

EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsNetPosMethod* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = ELbsNetPosMethodUnknownClass;
	aPipe >> classType;
	
	// Only one sub-class defined; ELbsNetPosMethodClass
	__ASSERT_ALWAYS(classType == ELbsNetPosMethodClass, User::Invariant());
	
	TPckgBuf<TLbsNetPosMethod> data;
	aPipe >> data;
	TLbsNetPosMethod *pNetPosMethod = iMemManager->LbsNetPosMethod();
	*pNetPosMethod = data();
	aParam = pNetPosMethod;
	}

EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsNetPosCapabilities* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = ELbsNetPosCapabilitiesUnknownClass;
	aPipe >> classType;
	
	// Only one sub-class defined; ELbsNetPosCapabilitiesClass
	__ASSERT_ALWAYS(classType == ELbsNetPosCapabilitiesClass, User::Invariant());
	
	TPckgBuf<TLbsNetPosCapabilities> data;
	aPipe >> data;
	TLbsNetPosCapabilities *pNetPosCapabilities = iMemManager->LbsNetPosCapabilities();
	*pNetPosCapabilities = data();
	aParam = pNetPosCapabilities;
	}

EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsNetPosRequestMethod* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = ELbsNetPosRequestMethodUnknownClass;
	aPipe >> classType;
	
	// Only one sub-class defined; ELbsNetPosRequestMethodClass
	__ASSERT_ALWAYS(classType == ELbsNetPosRequestMethodClass, User::Invariant());
	
	TPckgBuf<TLbsNetPosRequestMethod> data;
	aPipe >> data;
	TLbsNetPosRequestMethod *pNetPosRequestMethod = iMemManager->LbsNetPosRequestMethod();
	*pNetPosRequestMethod = data();
	aParam = pNetPosRequestMethod;
	}
	
EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TLbsExternalRequestInfo* &aParam ,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
// Unknown if and where any sub-classes are defined at this time - TTD CHECK!
	
	TPckgBuf<TLbsExternalRequestInfo> data;
	aPipe >> data;
	TLbsExternalRequestInfo * pExtReqInf = iMemManager->LbsExternalRequestInfo();
	*pExtReqInf = data();
	aParam = pExtReqInf;
	}
	
EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, HBufC16* &aParam)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 len = 0;
	
	aPipe >> len;
	HBufC16 *p = HBufC16::NewMaxLC(len);
	TPtr16 ptr = p->Des();
	aPipe >> ptr;
	CleanupStack::Pop(p);
	aParam = p;
	}
	
EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TInt& aParam)
	{
	TInt32 a = 0;
	aPipe >> a;
	aParam = (TInt)a;
	}

EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TUint32& aParam)
	{
	TInt32 a = 0;
	aPipe >> a;
	aParam = (TUint32)a;
	}
	
EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, TPositionInfoBase* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 classType = EPositionInfoUnknownClass;
	aPipe >> classType;
	
	switch(classType)
		{
		case EPositionInfoClass:
			{
			TPckgBuf<TPositionInfo> data;
			aPipe >> data;
			TPositionInfo *pPositionInfo = iMemManager->LbsPositionInfo();
			*pPositionInfo = data();
			aParam = pPositionInfo;
			break;
			}
		case EPositionGenericInfoClass: // Not used?
			User::Invariant(); 
			break;
		case (EPositionInfoClass|EPositionCourseInfoClass):
			{
			TPckgBuf<TPositionCourseInfo> data;
			aPipe >> data;
			TPositionCourseInfo *pPositionCourseInfo = iMemManager->LbsPositionCourseInfo();
			*pPositionCourseInfo = data();
			aParam = pPositionCourseInfo;
			break;
			}
		case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass):
			{
			TPckgBuf<TPositionSatelliteInfo> data;
			aPipe >> data;
			TPositionSatelliteInfo *pPositionSatelliteInfo = iMemManager->LbsPositionSatelliteInfo();
			*pPositionSatelliteInfo = data();
			aParam = pPositionSatelliteInfo;
			break;
			}
		
		case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass):
            {
            TPckgBuf<TPositionExtendedSatelliteInfo> data;
            aPipe >> data;
            TPositionExtendedSatelliteInfo *pPositionExtendedSatelliteInfo = iMemManager->LbsPositionExtendedSatelliteInfo();
            *pPositionExtendedSatelliteInfo = data();
            aParam = pPositionExtendedSatelliteInfo;
            break;
            }
		case EPositionGpsMeasurementInfoClass:
			{
			TPckgBuf<TPositionGpsMeasurementInfo> data;
			aPipe >> data;
			TPositionGpsMeasurementInfo *pPositionGpsMeasurementInfo = iMemManager->LbsPositionGpsMeasurementInfo();
			*pPositionGpsMeasurementInfo = data();
			aParam = pPositionGpsMeasurementInfo;
			break;
			}
		default:
			User::Invariant();
			break;
		}
	}
	
EXPORT_C void LBSTestMarshaller::ReadL(CReadPropPipe& aPipe, RLbsAssistanceDataBuilderSet* &aParam,CMemoryManager* iMemManager)
	{
	__ASSERT_ALWAYS(NULL == aParam, User::Invariant());
	
	TInt32 len = 0;
	aPipe >> len;
	HBufC8* pBuf1 = iMemManager->MaxBuf();
	TPtr8 ptr = pBuf1->Des();
	aPipe >> ptr;
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	RLbsAssistanceDataBuilderSet *p = iMemManager->LbsAssistanceDataBuilderSet();
	TRAPD(e,p->InternalizeL(stream));
	if(KErrNone == e)
		{
		aParam = p;
		}
	else
		{
		p->Close();	
		}
	CleanupStack::PopAndDestroy();	// stream
	}
	

