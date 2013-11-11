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

#include "tsu_mmf_devsubtitle_neg.h"
#include "subtitlecommonutils.h"

_LIT(KSrtDecoderMime, "srtdecoder");
const TInt KWindowClipRectTlX(0);
const TInt KWindowClipRectTlY(0);
const TInt KWindowClipRectBrX(640);
const TInt KWindowClipRectBrY(240);
const TInt KWindowIdValid = 100;

//
//RDevSubtitleLanguage
//
RDevSubtitleLanguage* RDevSubtitleLanguage::NewL(const TDesC& aStepName)
	{
	RDevSubtitleLanguage* self = new (ELeave) RDevSubtitleLanguage(aStepName);
	return self;
	}

RDevSubtitleLanguage::RDevSubtitleLanguage(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleLanguage::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid;
	DevSubtitle()->AddSubtitleConfigL(config);
	
	TRAPD(err, DevSubtitle()->SetSubtitleLanguageL(ELangEnglish));
	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguageL left with %d; expecting %d."), err, KErrNotSupported);
		return EFail;
		}
	
	//TLanguage lang = ELangNone;
	TRAP(err, TLanguage lang = DevSubtitle()->SubtitleLanguageL());
	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguageL left with %d; expecting %d."), err, KErrNotSupported);
		return EFail;
		}
	
	RArray<TLanguage> languages;
	TRAP(err, DevSubtitle()->GetSupportedSubtitleLanguagesL(languages));
	TInt count = languages.Count();
	languages.Close();
	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF3(_L("GetSupportedSubtitleLanguagesL left with %d; expected %d"), err, KErrNotSupported);
		return EFail;
		}
	
	if (count != 0)
		{
		ERR_PRINTF3(_L("GetSupportedSubtitleLanguagesL returned %d languages; expected %d"), languages.Count(), 0);
		return EFail;
		}
	
	return EPass;
	}

//
//RDevSubtitleAddConfigTwice
//
RDevSubtitleAddConfigTwice* RDevSubtitleAddConfigTwice::NewL(const TDesC& aStepName)
	{
	RDevSubtitleAddConfigTwice* self = new (ELeave) RDevSubtitleAddConfigTwice(aStepName);
	return self;
	}

RDevSubtitleAddConfigTwice::RDevSubtitleAddConfigTwice(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleAddConfigTwice::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid;
	DevSubtitle()->AddSubtitleConfigL(config);

	// Adding the same subtitle configuration a second time should result in KErrArgument.
	TRAPD(err, DevSubtitle()->AddSubtitleConfigL(config));
	
	if (err != KErrArgument)
		{
		ERR_PRINTF3(_L("AddSubtitleConfigL left with %d; expecting %d."), err, KErrArgument);
		return EFail;
		}
	
	// Adding a different configuration should result in KErrInUse.
	config.iWindowId = 1;
	TRAP(err, DevSubtitle()->AddSubtitleConfigL(config));
	
	if (err != KErrInUse)
		{
		ERR_PRINTF3(_L("AddSubtitleConfigL left with %d; expecting %d."), err, KErrInUse);
		return EFail;
		}
	
	return EPass;
	}

//
//RDevSubtitleGetCrpNegative
//
RDevSubtitleGetCrpNegative* RDevSubtitleGetCrpNegative::NewL(const TDesC& aStepName)
	{
	RDevSubtitleGetCrpNegative* self = new (ELeave) RDevSubtitleGetCrpNegative(aStepName);
	return self;
	}

RDevSubtitleGetCrpNegative::RDevSubtitleGetCrpNegative(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleGetCrpNegative::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;

	// Expect leave because no subtitle configuration has been added yet.
	TRAPD(err, DevSubtitle()->GetCrpParametersL(KWindowIdValid, crpId, region));
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("GetCrpParametersL left with %d; expecting %d."), err, KErrNotReady);
		return EFail;
		}
	
	return EPass;
	}

//
//RDevSubtitleLanguageNoLoad
//
RDevSubtitleLanguageNoLoad* RDevSubtitleLanguageNoLoad::NewL(const TDesC& aStepName)
	{
	RDevSubtitleLanguageNoLoad* self = new (ELeave) RDevSubtitleLanguageNoLoad(aStepName);
	return self;
	}

RDevSubtitleLanguageNoLoad::RDevSubtitleLanguageNoLoad(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleLanguageNoLoad::DoTestStepL()
	{
	TRAPD(err, DevSubtitle()->SetSubtitleLanguageL(ELangEnglish));
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguageL left with %d; expecting %d."), err, KErrNotReady);
		return EFail;
		}
	
	TRAP(err, TLanguage lang = DevSubtitle()->SubtitleLanguageL());
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguageL left with %d; expecting %d."), err, KErrNotReady);
		return EFail;
		}
	
	RArray<TLanguage> languages;
	TRAP(err, DevSubtitle()->GetSupportedSubtitleLanguagesL(languages));
	languages.Close();
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("GetSupportedSubtitleLanguagesL left with %d; expected %d"), err, KErrNotReady);
		return EFail;
		}
	
	return EPass;
	}

//
//RDevSubtitleRemoveNoAdd
//
RDevSubtitleRemoveNoAdd* RDevSubtitleRemoveNoAdd::NewL(const TDesC& aStepName)
	{
	RDevSubtitleRemoveNoAdd* self = new (ELeave) RDevSubtitleRemoveNoAdd(aStepName);
	return self;
	}

RDevSubtitleRemoveNoAdd::RDevSubtitleRemoveNoAdd(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleRemoveNoAdd::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	// Remove the subtitle configuration before one has been added.
	TRAPD(err, DevSubtitle()->RemoveSubtitleConfigL(KWindowIdValid));
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("RemoveSubtitleConfigL left with %d; expecting %d."), err, KErrNotReady);
		return EFail;
		}
			
	return EPass;
	}

//
//RDevSubtitleNoSelect
//
RDevSubtitleNoSelect* RDevSubtitleNoSelect::NewL(const TDesC& aStepName)
	{
	RDevSubtitleNoSelect* self = new (ELeave) RDevSubtitleNoSelect(aStepName);;
	return self;
	}

RDevSubtitleNoSelect::RDevSubtitleNoSelect(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleNoSelect::DoTestStepL()
	{
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid;
	
	// Add the subtitle configuration before decoder has been selected.
	TRAPD(err, DevSubtitle()->AddSubtitleConfigL(config));
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("AddSubtitleConfigL left with %d; expected %d"), err, KErrNotReady);
		return EFail;
		}
	
	return EPass;
	}

//
//RDevSubtitleBogusDecoder
//
RDevSubtitleBogusDecoder* RDevSubtitleBogusDecoder::NewL(const TDesC& aStepName)
	{
	RDevSubtitleBogusDecoder* self = new (ELeave) RDevSubtitleBogusDecoder(aStepName);
	return self;
	}

RDevSubtitleBogusDecoder::RDevSubtitleBogusDecoder(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleBogusDecoder::DoTestStepL()
	{
	_LIT(KSrtDecoderMime, "BOGUS");
	TRAPD(err, DevSubtitle()->SelectDecoderL(KSrtDecoderMime));
	
	if (KErrNotFound != err)
		{
		ERR_PRINTF3(_L("SelectDecoderL left with %d; expected %d"), err, KErrNotFound);
		return EFail;
		}
	
	return EPass;
	}

//
//RDevSubtitleInvalidStatePos
//
RDevSubtitleInvalidStatePos* RDevSubtitleInvalidStatePos::NewL(const TDesC& aStepName)
	{
	RDevSubtitleInvalidStatePos* self = new (ELeave) RDevSubtitleInvalidStatePos(aStepName);
	return self;
	}

RDevSubtitleInvalidStatePos::RDevSubtitleInvalidStatePos(const TDesC& aStepName)
	: RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleInvalidStatePos::DoTestStepL()
	{
	DevSubtitle()->SetVideoPositionL(0);
	ERR_PRINTF1(_L("SetVideoPosition() should have panicked, so test result is fail"));
	return EFail;
	}

//
//RDevSubtitleInvalidStateStart
//
RDevSubtitleInvalidStateStart* RDevSubtitleInvalidStateStart::NewL(const TDesC& aStepName)
	{
	RDevSubtitleInvalidStateStart* self = new (ELeave) RDevSubtitleInvalidStateStart(aStepName);
	return self;
	}

RDevSubtitleInvalidStateStart::RDevSubtitleInvalidStateStart(const TDesC& aStepName)
	: RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleInvalidStateStart::DoTestStepL()
	{
	DevSubtitle()->Start();
	ERR_PRINTF1(_L("Start() should have panicked, so test result is fail"));
	return EFail;
	}

//
//RDevSubtitleStartTwice
//
RDevSubtitleStartTwice* RDevSubtitleStartTwice::NewL(const TDesC& aStepName)
	{
	RDevSubtitleStartTwice* self = new (ELeave) RDevSubtitleStartTwice(aStepName);
	return self;
	}

RDevSubtitleStartTwice::RDevSubtitleStartTwice(const TDesC& aStepName)
	: RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleStartTwice::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid;
	DevSubtitle()->AddSubtitleConfigL(config);
	
	INFO_PRINTF1(_L("Calling Start()"));
	DevSubtitle()->Start();
	
	INFO_PRINTF1(_L("Calling Start() again"));
	DevSubtitle()->Start();
	
	ERR_PRINTF1(_L("Expected a panic, so test result is fail"));
	return EFail;
	}

//
//RDevSubtitleInvalidStateStop
//
RDevSubtitleInvalidStateStop* RDevSubtitleInvalidStateStop::NewL(const TDesC& aStepName)
	{
	RDevSubtitleInvalidStateStop* self = new (ELeave) RDevSubtitleInvalidStateStop(aStepName);
	return self;
	}

RDevSubtitleInvalidStateStop::RDevSubtitleInvalidStateStop(const TDesC& aStepName)
	: RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleInvalidStateStop::DoTestStepL()
	{
	DevSubtitle()->Stop();
	ERR_PRINTF1(_L("Stop() did not panic, so test result is pass"));
	return EPass;
	}

//
//RDevSubtitleInvalidConfig
//
RDevSubtitleInvalidConfig* RDevSubtitleInvalidConfig::NewL(const TDesC& aStepName)
	{
	RDevSubtitleInvalidConfig* self = new (ELeave) RDevSubtitleInvalidConfig(aStepName);
	return self;
	}

RDevSubtitleInvalidConfig::RDevSubtitleInvalidConfig(const TDesC& aStepName)
	: RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleInvalidConfig::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowId = KWindowIdValid;
	
	// Zero height and width
	config.iWindowClipRect = TRect(100,100,100,100);
	TRAPD(err, DevSubtitle()->AddSubtitleConfigL(config));	
	if (err != KErrArgument)
		{
		ERR_PRINTF3(_L("AddSubtitleConfigL left with %d; expected %d"), err, KErrArgument);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	// Negative height and width.
	config.iWindowClipRect = TRect(100,100,0,0);
	TRAP(err, DevSubtitle()->AddSubtitleConfigL(config));
	if (err != KErrArgument)
		{
		ERR_PRINTF3(_L("AddSubtitleConfigL left with %d; expected %d"), err, KErrArgument);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	// Invalid display mode
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY); // Reset clip rect to valid value
	config.iDisplayMode = EGray2;
	TRAP(err, DevSubtitle()->AddSubtitleConfigL(config));
	if (err != KErrArgument)
		{
		ERR_PRINTF3(_L("AddSubtitleConfigL left with %d; expected %d"), err, KErrArgument);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	return EPass;
	}

//
//RDevSubtitleGetCrpAfterRemove
//
RDevSubtitleGetCrpAfterRemove* RDevSubtitleGetCrpAfterRemove::NewL(const TDesC& aStepName)
	{
	RDevSubtitleGetCrpAfterRemove* self = new (ELeave) RDevSubtitleGetCrpAfterRemove(aStepName);
	return self;
	}

RDevSubtitleGetCrpAfterRemove::RDevSubtitleGetCrpAfterRemove(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{
	}

TVerdict RDevSubtitleGetCrpAfterRemove::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid;
	
	// Add and remove a subtitle configuration.
	DevSubtitle()->AddSubtitleConfigL(config);
	DevSubtitle()->RemoveSubtitleConfigL(KWindowIdValid);
	
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;

	// Expect to leave because the subtitle configuration has already been removed.
	TRAPD(err, DevSubtitle()->GetCrpParametersL(KWindowIdValid, crpId, region));
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("GetCrpParametersL left with %d; expected %d"), err, KErrNotReady);
		return EFail;
		}
	
	return EPass;
	}
