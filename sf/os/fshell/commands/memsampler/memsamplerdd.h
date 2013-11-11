// memsamplerdd.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __MEMSAMPLERDD_H__
#define __MEMSAMPLERDD_H__

#include <e32cmn.h>
#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

//Uncomment this line to generate debug logging of the profiler and sampler.
#define DEBUG_MEMSAMPLER(f) f
//#define DEBUG_MEMSAMPLER(f)


/**
 * The user device driver class for controlling the memory usage sampler.
 */
class RMemSampler : public RBusLogicalChannel
	{
public:
	enum
		{
		EBtraceCategory = 253
		};
	enum
		{
		ENewChunk,
		EChangedChunk,
		EDeletedChunk,
		ENewSample
		};
	friend class DMemSamplerChannel;
private:
	enum TControl
		{
		EControlStartProfile,
		EControlStopProfile,
		EControlResetProfile
		};
	enum TRequest
		{
		ERequestRead
		};
	static inline TInt CancelRequest(TRequest aRequest);
public:
#ifndef __KERNEL_MODE__
	inline TInt Open();
	inline void Start(TInt aRate);
	inline void Stop();
	inline void Reset();
#endif
	};

_LIT(KMemSamplerName,"memsamplerdd");

#ifndef __KERNEL_MODE__

inline TInt RMemSampler::Open()
	{return DoCreate(KMemSamplerName,TVersion(1,0,0),KNullUnit,NULL,NULL);}

inline void RMemSampler::Start(TInt aRate)
	{DoControl(EControlStartProfile, reinterpret_cast<TAny*>(aRate));}

inline void RMemSampler::Stop()
	{DoControl(EControlStopProfile);}

inline void RMemSampler::Reset()
	{DoControl(EControlResetProfile);}


#endif

#endif
