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

#ifndef TSU_MMF_DEVSUBTITLE_NEG_H
#define TSU_MMF_DEVSUBTITLE_NEG_H

#include "tsu_mmf_devsubtitle.h"

/**
 * MM-MMF-DEVSUBTITLE-U-0101-HP
 */
class RDevSubtitleLanguage : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleLanguage* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
RDevSubtitleLanguage(const TDesC& aStepName);
	};

/**
 * MM-MMF-DEVSUBTITLE-U-0102-HP
 */
class RDevSubtitleAddConfigTwice : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleAddConfigTwice* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleAddConfigTwice(const TDesC& aStepName);
	};

/**
 * MM-MMF-DEVSUBTITLE-U-0103-HP
 */
class RDevSubtitleGetCrpNegative : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleGetCrpNegative* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleGetCrpNegative(const TDesC& aStepName);
	};

/** 
 * MM-MMF-DEVSUBTITLE-U-0104-HP
 */
class RDevSubtitleLanguageNoLoad : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleLanguageNoLoad* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleLanguageNoLoad(const TDesC& aStepName);
	};

/** 
 * MM-MMF-DEVSUBTITLE-U-0105-HP
 */
class RDevSubtitleRemoveNoAdd : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleRemoveNoAdd* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleRemoveNoAdd(const TDesC& aStepName);
	};

/** 
 * MM-MMF-DEVSUBTITLE-U-0106-HP
 */
class RDevSubtitleNoSelect : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleNoSelect* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleNoSelect(const TDesC& aStepName);
	};
	
/** 
 * MM-MMF-DEVSUBTITLE-U-0107-HP
 */
class RDevSubtitleBogusDecoder : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleBogusDecoder* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleBogusDecoder(const TDesC& aStepName);
	};

/** 
 * MM-MMF-DEVSUBTITLE-U-0108-HP
 */
class RDevSubtitleInvalidStatePos : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleInvalidStatePos* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleInvalidStatePos(const TDesC& aStepName);
	};

/** 
 * MM-MMF-DEVSUBTITLE-U-0109-HP
 */
class RDevSubtitleInvalidStateStart : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleInvalidStateStart* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleInvalidStateStart(const TDesC& aStepName);
	};
	
/** 
 * MM-MMF-DEVSUBTITLE-U-0110-HP
 */
class RDevSubtitleStartTwice : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleStartTwice* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleStartTwice(const TDesC& aStepName);
	};

/** 
 * MM-MMF-DEVSUBTITLE-U-0111-HP
 */
class RDevSubtitleInvalidStateStop : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleInvalidStateStop* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleInvalidStateStop(const TDesC& aStepName);
	};

/** 
 * MM-MMF-DEVSUBTITLE-U-0112-HP
 */
class RDevSubtitleInvalidConfig : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleInvalidConfig* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleInvalidConfig(const TDesC& aStepName);
	};

/**
 * MM-MMF-DEVSUBTITLE-U-0113-HP
 */
class RDevSubtitleGetCrpAfterRemove : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleGetCrpAfterRemove* NewL(const TDesC& aStepName);
	
	// From RTestStep
    TVerdict DoTestStepL();
    
private:
	RDevSubtitleGetCrpAfterRemove(const TDesC& aStepName);
	};
#endif // TSU_MMF_DEVSUBTITLE_NEG_H
