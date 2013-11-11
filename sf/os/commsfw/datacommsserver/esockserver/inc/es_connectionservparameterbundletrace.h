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
// This is a debug-only trace library for ConnectionServParameterBundle.
// To avoid symbol export issues it doesn't rely on any exported symbols.
// So its implementation (es_connectionservparameterbundletraceimpl.h)
// must be brought in *once* into each DLL / EXE it is to be used in.
// This small debug-only code duplication cost is less of a problem than trying
// to manage udeb-only exports, by having debug variables hanging around
// in urel binaries
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef ES_CONNECTIONSERVPARAMETERBUNDLETRACE
#define ES_CONNECTIONSERVPARAMETERBUNDLETRACE

#include <comms-infras/ss_log.h>
#include <comms-infras/es_parameterbundle.h>



// Macro definitions for each of the logging systems in turn.
//   1. binary logging. For high performance instrumentation only-
//      on by default in UDEB, but 
//      should be turn-on-able in UREL without killing performance.
//      i.e. not for formatted strings!
#ifdef SYMBIAN_TRACE_ENABLE

_LIT8(KCoreTMgrSubTag, "coretiermgr");
#define _TIER_LOG_SVG(src,dest,fn) \
    NM_LOG_START_BLOCK(KCoreTMgrSubTag, _L8("TierLogSvg")); \
    NM_LOG((KCoreTMgrSubTag, _L8(fn))); \
    NM_LOG_NODE_EXT(KCoreTMgrSubTag, src, _L8("Src: ")); \
    NM_LOG_NODE_EXT(KCoreTMgrSubTag, dest, _L8("Dest: ")); \
    NM_LOG_END_BLOCK(KCoreTMgrSubTag, _L8("TierLogSvg"));

#else // SYMBIAN_TRACE_ENABLE

#define _TIER_LOG_SVG(src,dest,fn)

#endif SYMBIAN_TRACE_ENABLE


//   2. rich logging with CommsDebugUtility- generally only
//      enabled in UDEB and full of formatted strings etc.
#ifdef __CFLOG_ACTIVE

#define KTierTag KESockTierTag

#ifndef SYMBIAN_TRACE_ENABLE
_LIT8(KCoreTMgrSubTag, "coretiermgr");
#endif
#define _TIER_LOG_PSC(text,paramset) {ParameterBundleTrace::ParamSetCtrToLog(KTierTag, KCoreTMgrSubTag,text,paramset);}

#define _TIER_LOG(x)                LOG(ESockLogExternal::Printf(KTierTag, KCoreTMgrSubTag, x));
#define _TIER_LOG_2(x,y)            LOG(ESockLogExternal::Printf(KTierTag, KCoreTMgrSubTag, x,y));
#define _TIER_LOG_3(x,y,z)          LOG(ESockLogExternal::Printf(KTierTag, KCoreTMgrSubTag, x,y,z));
#define _TIER_LOG_4(x,y,z,a1)       LOG(ESockLogExternal::Printf(KTierTag, KCoreTMgrSubTag, x,y,z,a1));
#define _TIER_LOG_5(x,y,z,a1,a2)    LOG(ESockLogExternal::Printf(KTierTag, KCoreTMgrSubTag, x,y,z,a1,a2));


#define _TIER_LOG_BUNDLE(text,paramset) {ParameterBundleTrace::BundleToLog(KTierTag, KCoreTMgrSubTag,text,paramset);}


_LIT8(KConnServSubTag, "connserv");
#define _LOG_BUNDLE(text,bundle) {ParameterBundleTrace::BundleToLog(KTierTag,KConnServSubTag,text,bundle);}



namespace ParameterBundleTrace
{
void BundleToString(const CParameterBundleBase* aBundle,TDes8& aBuf);
void BundleToLog(const TDesC8& aSubsystem, const TDesC8& aComponent, char* aText, const CParameterBundleBase* aBundle);

void ParamSetCtrToString(const CParameterSetContainer* aCtnr,TDes8& aBuf);
void ParamSetCtrToLog(const TDesC8& aSubsystem, const TDesC8& aComponent, char* aText, const CParameterSetContainer* aCtnr);

// Deprecated methods for appending a string representation of parameter sets to a buffer. Use aSet->AppendPrintableTo() instead
void GenericParamsToString(const XParameterSet* aSet,TDes8& aBuf);
void GenericQueryToString(const XParameterSet* aSet,TDes8& aBuf);
void CacheParamsToString(const XParameterSet* aSet,TDes8& aBuf);
void CoreCollectorSharedParametersToString(const XParameterSet* aSet,TDes8& aBuf);
void IpProtoParamsToString(const XParameterSet* aSet,TDes8& aBuf);
void WifiParamsToString(const XParameterSet* aSet,TDes8& aBuf);
}

#else // __CFLOG_ACTIVE

#define _TIER_LOG_PSC(output,pset)
#define _TIER_LOG(x)
#define _TIER_LOG_2(x,y)
#define _TIER_LOG_3(x,y,z)
#define _TIER_LOG_4(x,y,z,a1)
#define _TIER_LOG_5(x,y,z,a1,a2)
#define _TIER_LOG_BUNDLE(text,bundle)

#define _LOG_BUNDLE(text,bundle)

#endif
// __CFLOG_ACTIVE


#endif
// ES_CONNECTIONSERVPARAMETERBUNDLETRACE

