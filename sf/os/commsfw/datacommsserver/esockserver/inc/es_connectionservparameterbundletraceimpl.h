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
 @publishedPartner
 @released
*/

#ifndef CONNECTIONSERVPARAMETERBUNDLETRACEIMPL_H
#define CONNECTIONSERVPARAMETERBUNDLETRACEIMPL_H

#ifdef __CFLOG_ACTIVE

// Implementation of functions declared in es_connectionservparameterbundletrace.h

#define __PULL_IN_BUNDLE_TRACE_CODE


// t_esock specific kludge to prevent duplicated symbols
#ifdef __BUILDING_T_ESOCK
#ifdef SYMBIAN_SS_TIER_MANAGER_H
#undef __PULL_IN_BUNDLE_TRACE_CODE
#endif
#endif



#ifdef __PULL_IN_BUNDLE_TRACE_CODE

#include <comms-infras/ss_log.h>    

using namespace ConnectionServ;

namespace ParameterBundleTrace
{




void ParamSetToString(const XParameterSetBase *baseSet,TDes8& aBuf)
	{
	aBuf.Append(_L8(" {"));
	if(baseSet->IsTypeOf(Meta::STypeId::CreateSTypeId(KParameterSetTypeUid,Meta::KNetMetaTypeAny)))
		{
		const XParameterSet* set = static_cast<const XParameterSet*>(baseSet);
		
		// perform des16->des8 conversion and safe bounds checking...:
		
		RBuf16 tmp;
		// length limitation only in place for users of this legacy method
		const TInt max_supported_length = 256; // characters
		TRAPD(res,tmp.CreateL(max_supported_length));
		if(res == KErrNone)
			{
			if(set->AppendPrintableTo(tmp) == KErrNone)
				{
				if( tmp.Length() <= aBuf.MaxLength()-aBuf.Length() )
					{
					RBuf8 tmp2;
					TRAP(res,tmp2.CreateL(tmp.Length()));
					if(res == KErrNone)
						{
						tmp2.Copy(tmp);
						aBuf.Append(tmp2);
						tmp2.Close();
						}
					}
				}
			tmp.Close();	
			}
		}
	else // no other polymorphic trace..
		{
		aBuf.Append(_L8("????"));
		}
	aBuf.Append(_L8("}"));
	}


// deprecated methods for specific types. use the XParameterSet::AppendPrintableTo method instead.
void GenericParamsToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}
void GenericQueryToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}
void CacheParamsToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}
void CoreCollectorSharedParametersToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}
void IpProtoParamsToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}
void PlaneStatusParamsToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}
void PlaneStatusQueryToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}
void WifiParamsToString(const XParameterSet* aSet,TDes8& aBuf)
	{ParamSetToString(aSet,aBuf);}


void ParamSetCtrToString(const CParameterSetContainer* aCtnr,TDes8& aBuf)
	{
	aBuf.Append(_L8("Ctnr id: "));
	aBuf.AppendNum(aCtnr->Id());
	const XParameterSetBase *set;
	TInt i=0;
	while(1)
		{
		set = aCtnr->GetParameterSet(i++);
		if(set==NULL)break;

		ParamSetToString(set,aBuf);
		}
	aBuf.Append(_L8("\r\n"));
	}

void ParamSetCtrToLog(const TDesC8& aSubsystem, const TDesC8& aComponent, char* aText, const CParameterSetContainer* aCtnr)
	{
	#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((aSubsystem,aComponent,_L8("%s Ctnr id: %d:"), aText, aCtnr->Id()));
		
		TInt i=0;
		while(1)
			{
		const XParameterSetBase* set = aCtnr->GetParameterSet(i++);
			if(set==NULL)break;
		
			TBuf8<512> buf;
			buf.Append(_L8("    "));
			ParamSetToString(set,buf);
			__CFLOG_0(aSubsystem,aComponent,buf);
			}
	#else
		// Preventing unused variable warnings.
		(void)aSubsystem;
		(void)aComponent;
		(void)aText;
		(void)aCtnr;
	#endif
	}

void BundleToString(const CParameterBundleBase* aBundle,TDes8& aBuf)
	{
	if(aBundle==NULL)
		{
		aBuf.Append(_L8("Bundle:null\r\n"));
		return;
		}
	aBuf.Append(_L8("Bundle:\r\n"));
	const CParameterSetContainer* ctnr;
	TInt i=0;
	while(1)
		{
		ctnr = aBundle->GetParamSetContainer(i++);
		if(ctnr==NULL)break;

		ParamSetCtrToString(ctnr,aBuf);
		}
	}
	
void BundleToLog(const TDesC8& aSubsystem, const TDesC8& aComponent, char* aText, const CParameterBundleBase* aBundle)
	{
	#ifdef __CFLOG_ACTIVE
		if(aBundle==NULL)
			{
			__CFLOG_VAR((aSubsystem,aComponent,_L8("%s Bundle:null"), aText));
			return;
			}
		__CFLOG_VAR((aSubsystem,aComponent,_L8("%s Bundle:"), aText));
		const CParameterSetContainer* ctnr;
		TInt i=0;
		while(1)
			{
			ctnr = aBundle->GetParamSetContainer(i++);
			if(ctnr==NULL)break;

			ParamSetCtrToLog(aSubsystem,aComponent," ",ctnr);
			}
	#else
		// Preventing unused variable warnings.
		(void)aSubsystem;
		(void)aComponent;
		(void)aText;
		(void)aBundle;
	#endif
	}
	
	


} // namespace ParameterBundleTrace


#endif
// __PULL_IN_BUNDLE_TRACE_CODE

#endif
// __CFLOG_ACTIVE

#endif
// CONNECTIONSERVPARAMETERBUNDLETRACEIMPL_H

