// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MDF_AUDIOCODECS_RESOLVER_H
#define TSU_MDF_AUDIOCODECS_RESOLVER_H

#include "tsu_mdf_audiocodecs_step.h"
#include <mmf/server/mmfhwdevice.h>

class RTestStepAudioCodecLoadFourCC : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecLoadFourCC();
	virtual TVerdict DoTestStepL();
	};

class RTestStepAudioCodecFindP8InputFourCC : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecFindP8InputFourCC();
	virtual TVerdict DoTestStepL();
	};	

class RTestStepAudioCodecFindPU8OutputFourCC : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecFindPU8OutputFourCC();
	virtual TVerdict DoTestStepL();
	};	
		
class RTestStepAudioCodecFindIncorrectInputFourCC : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecFindIncorrectInputFourCC();
	virtual TVerdict DoTestStepL();
	};	
	
class RTestStepAudioCodecFindIncorrectUid : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecFindIncorrectUid();
	virtual TVerdict DoTestStepL();
	};		

class RTestStepAudioCodecFindIncorrectImpUid : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecFindIncorrectImpUid();
	virtual TVerdict DoTestStepL();
	};	
	
class RTestStepAudioCodecFindIncorrectInterfaceUid : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecFindIncorrectInterfaceUid();
	virtual TVerdict DoTestStepL();
	};		
	

class RTestStepVideoResolverUtils : public RTestStepAudioCodecs
	{
public:
	RTestStepVideoResolverUtils();
	virtual TVerdict DoTestStepL();
	};		
	
class RTestStepResolverUtils : public RTestStepAudioCodecs
	{
public:
	RTestStepResolverUtils();
	virtual TVerdict DoTestStepL();
	};		
	
	
#endif // TSU_MDF_AUDIOCODECS_RESOLVER_H


