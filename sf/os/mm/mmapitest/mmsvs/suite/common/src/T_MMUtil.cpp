/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32std.h>
#include <mmf/common/mmffourcc.h>

//	User Includes
#include "T_MMUtil.h"
#include "DataWrapperBase.h"

/*@{*/
_LIT(KPriority,								"priority");

_LIT(KMdaPriorityMin,						"EMdaPriorityMin");
_LIT(KMdaPriorityNormal,					"EMdaPriorityNormal");
_LIT(KMdaPriorityMax,						"EMdaPriorityMax");

_LIT(KPriorityPreference,					"prioritypreference");

_LIT(KMdaPriorityPreferenceNone,			"None");
_LIT(KMdaPriorityPreferenceTime,			"Time");
_LIT(KMdaPriorityPreferenceQuality,			"Quality");
_LIT(KMdaPriorityPreferenceTimeAndQuality,	"TimeAndQuality");

_LIT(KThreadPriority,						"threadpriority");

_LIT(KEPriorityNull,						"EPriorityNull");
_LIT(KEPriorityMuchLess,					"EPriorityMuchLess");
_LIT(KEPriorityLess,						"EPriorityLess");
_LIT(KEPriorityNormal,						"EPriorityNormal");
_LIT(KEPriorityMore,						"EPriorityMore");
_LIT(KEPriorityMuchMore,					"EPriorityMuchMore");
_LIT(KEPriorityRealTime,					"EPriorityRealTime");
_LIT(KEPriorityAbsoluteVeryLow,				"EPriorityAbsoluteVeryLow");
_LIT(KEPriorityAbsoluteLow,					"EPriorityAbsoluteLow");
_LIT(KEPriorityAbsoluteBackground,			"EPriorityAbsoluteBackground");
_LIT(KEPriorityAbsoluteForeground,			"EPriorityAbsoluteForeground");
_LIT(KEPriorityAbsoluteHigh,				"EPriorityAbsoluteHigh");

_LIT(KFourCC,								"fourcc");

_LIT(KDesMMFFourCCCodePCM8, 				"KMMFFourCCCodePCM8");
_LIT(KDesMMFFourCCCodePCMU8,		 		"KMMFFourCCCodePCMU8");
_LIT(KDesMMFFourCCCodePCM16, 				"KMMFFourCCCodePCM16");
_LIT(KDesMMFFourCCCodePCMU16,		 		"KMMFFourCCCodePCMU16");
_LIT(KDesMMFFourCCCodePCM16B, 				"KMMFFourCCCodePCM16B");
_LIT(KDesMMFFourCCCodePCMU16B,		 		"KMMFFourCCCodePCMU16B");
_LIT(KDesMMFFourCCCodeIMAD, 				"KMMFFourCCCodeIMAD");
_LIT(KDesMMFFourCCCodeIMAS,		 			"KMMFFourCCCodeIMAS");
_LIT(KDesMMFFourCCCodeALAW,					"KMMFFourCCCodeALAW");
_LIT(KDesMMFFourCCCodeMuLAW,		 		"KMMFFourCCCodeMuLAW");
_LIT(KDesMMFFourCCCodeGSM610,				"KMMFFourCCCodeGSM610");
_LIT(KDesFourCCCodeNULL,					"KMMFFourCCCodeNULL");
_LIT(KDesMMFFourCCCodeAMR,					"KMMFFourCCCodeAMR");
_LIT(KDesMMFFourCCCodeAAC,					"KMMFFourCCCodeAAC");
_LIT(KDesMMFFourCCCodeAWB,					"KMMFFourCCCodeAWB");
_LIT(KDesMMFFourCCCodeMP3,					"KMMFFourCCCodeMP3");
_LIT(KDesMMFFourCCCodeATRAC3, 				"KMMFFourCCCodeATRAC3");
_LIT(KDesMMFFourCCCodeSBC,					"KMMFFourCCCodeSBC");
_LIT(KDesMMFFourCCCodeAMRW,					"KMMFFourCCCodeAMRW");
_LIT(KDesMMFFourCCCodeADTS,					"KMMFFourCCCodeADTS");
_LIT(KDesMMFFourCCCodeUnknown,				"KMMFFourCCCodeUnknown");

_LIT(KImageType,							"imagetype");

_LIT(KImageTypeThumbnail,					"EImageTypeThumbnail");
_LIT(KImageTypeMain,						"EImageTypeMain");

_LIT(KIntent,								"intent");

_LIT(KIntentPeek,							"EPeek");
_LIT(KIntentPlay,							"EPlay");
_LIT(KIntentView,							"EView");
_LIT(KIntentExecute,						"EExecute");
_LIT(KIntentPrint,							"EPrint");
_LIT(KIntentPause,							"EPause");
_LIT(KIntentContinue,						"EContinue");
_LIT(KIntentStop,							"EStop");
_LIT(KIntentUnknown,						"EUnknown");
_LIT(KIntentInstall,						"EInstall");

_LIT(KMidiStateClosedDisengaged,			"EMidiStateClosedDisengaged");
_LIT(KMidiStateOpenDisengaged,				"EMidiStateOpenDisengaged");
_LIT(KMidiStateOpenPlaying,					"EMidiStateOpenPlaying");
_LIT(KMidiStateClosedEngaged,				"EMidiStateClosedEngaged");
_LIT(KMidiStateOpenEngaged,					"EMidiStateOpenEngaged");

_LIT(KSampleRateFixed,						"SampleRateFixed");
_LIT(KSampleRateAnyInRange,					"SampleRateAnyInRange");
_LIT(KSampleRate8000Hz,						"SampleRate8000Hz");
_LIT(KSampleRate11025Hz,					"SampleRate11025Hz");
_LIT(KSampleRate12000Hz,					"SampleRate12000Hz");
_LIT(KSampleRate16000Hz,					"SampleRate16000Hz");
_LIT(KSampleRate22050Hz,					"SampleRate22050Hz");
_LIT(KSampleRate24000Hz,					"SampleRate24000Hz");
_LIT(KSampleRate32000Hz,					"SampleRate32000Hz");
_LIT(KSampleRate44100Hz,					"SampleRate44100Hz");
_LIT(KSampleRate48000Hz,					"SampleRate48000Hz");
_LIT(KSampleRate96000Hz,					"SampleRate96000Hz");
_LIT(KSampleRate64000Hz,					"SampleRate64000Hz");
_LIT(KChannelsMono,							"ChannelsMono");
_LIT(KChannelsStereo,						"ChannelsStereo");
_LIT(KRoutingControl,						"RoutingControl");
_LIT(KRealTime,								"RealTime");

_LIT(KNoLocalRouting,						"NoLocalRouting");
_LIT(KNoNetworkRouting,						"NoNetworkRouting");

_LIT(KOptionNone,							"OptionNone");
_LIT(KOptionNoDither,						"OptionNoDither");
_LIT(KOptionAlwaysThread,					"OptionAlwaysThread");
_LIT(KOptionAllowZeroFrameOpen,				"OptionAllowZeroFrameOpen");
_LIT(KAllowGeneratedMask,					"AllowGeneratedMask");

_LIT(KAgentPropertyBufferSize,				"AgentPropertyBufferSize");
_LIT(KAgentPropertyAgentUI,					"AgentPropertyAgentUI");
_LIT(KAgentPropertyMultipleSequence,		"AgentPropertyMultipleSequence");

_LIT(KCmdCapabilty,							"capabilty%d");
_LIT(KCmdChannels,							"channels");
_LIT(KCmdFlags,								"flags");
_LIT(KCmdMaxVolume,							"maxvolume");
_LIT(KCmdSampleRate,						"samplerate");
_LIT(KCmdVolume,							"volume");

_LIT(KDModeNone,							"None");
_LIT(KDModeGray2,							"Gray2");
_LIT(KDModeGray4,							"Gray4");
_LIT(KDModeGray16,							"Gray16");
_LIT(KDModeGray256,							"Gray256");
_LIT(KDModeColor16,							"Color16");
_LIT(KDModeColor256,						"Color256");
_LIT(KDModeColor64K,						"Color64K");
_LIT(KDModeColor16M,						"Color16M");
_LIT(KDModeRgb,								"Rgb");
_LIT(KDModeColor4K,							"Color4K");
_LIT(KDModeColor16MU,						"Color16MU");
_LIT(KDModeColor16MA,						"Color16MA");
_LIT(KDModeColorLast,						"ColorLast");

_LIT(KEVideoRotationNone,					"None");
_LIT(KEVideoRotationClockwise90,			"Clockwise90");
_LIT(KEVideoRotationClockwise180,			"Clockwise180");
_LIT(KEVideoRotationClockwise270,			"Clockwise270");
_LIT(KEVideoRotationUnknown,				"Unknown");



_LIT(KCount,								"count");
/*@}*/

TBool CT_MMUtil::ReadMdaPriority(CDataWrapper& aDataWrapper, const TDesC& aSection, TInt& aPriority)
	{
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, KPriority(), str);
	if ( ret )
		{
		if ( str==KMdaPriorityMin )
			{
			aPriority=EMdaPriorityMin;
			}
		else if ( str==KMdaPriorityNormal )
			{
			aPriority=EMdaPriorityNormal;
			}
		else if ( str==KMdaPriorityMax )
			{
			aPriority=EMdaPriorityMax;
			}
		else
			{
			if ( !aDataWrapper.GetIntFromConfig(aSection, KPriority(), aPriority) )
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal priority %S"), &KPriority());
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	aDataWrapper.INFO_PRINTF3(_L("ReadMdaPriority %S = %d"), &KPriority(), ret);

	return ret;
	}

TBool CT_MMUtil::ReadMdaPriorityPreference(CDataWrapper& aDataWrapper, const TDesC& aSection, TMdaPriorityPreference& aPreference)
	{
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, KPriorityPreference(), str);
	if ( ret )
		{
		if ( str==KMdaPriorityPreferenceNone )
			{
			aPreference=EMdaPriorityPreferenceNone;
			}
		else if ( str==KMdaPriorityPreferenceTime )
			{
			aPreference=EMdaPriorityPreferenceTime;
			}
		else if ( str==KMdaPriorityPreferenceQuality )
			{
			aPreference=EMdaPriorityPreferenceQuality;
			}
		else if ( str==KMdaPriorityPreferenceTimeAndQuality )
			{
			aPreference=EMdaPriorityPreferenceTimeAndQuality;
			}
		else
			{
			TInt	intVal;
			if ( aDataWrapper.GetIntFromConfig(aSection, KPriorityPreference(), intVal) )
				{
				aPreference=(TMdaPriorityPreference)intVal;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TMdaPriorityPreference %S"), &KPriorityPreference());
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	aDataWrapper.INFO_PRINTF3(_L("ReadMdaPriorityPreference %S = %d"), &KPriorityPreference(), ret);

	return ret;
	}

TBool CT_MMUtil::ReadThreadPriority(CDataWrapper& aDataWrapper, const TDesC& aSection, TThreadPriority& aPriority)
	{
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, KThreadPriority(), str);
	if ( ret )
		{
		if ( str==KEPriorityNull )
			{
			aPriority = EPriorityNull;
			}
		else if ( str==KEPriorityMuchLess )
			{
			aPriority = EPriorityMuchLess;
			}
		else if ( str==KEPriorityLess )
			{
			aPriority = EPriorityLess;
			}
		else if ( str==KEPriorityNormal )
			{
			aPriority = EPriorityNormal;
			}
		else if ( str==KEPriorityMore )
			{
			aPriority = EPriorityMore;
			}
		else if ( str==KEPriorityMuchMore )
			{
			aPriority = EPriorityMuchMore;
			}
		else if ( str==KEPriorityRealTime )
			{
			aPriority = EPriorityRealTime;
			}
		else if ( str==KEPriorityAbsoluteVeryLow )
			{
			aPriority = EPriorityAbsoluteVeryLow;
			}
		else if ( str==KEPriorityAbsoluteLow )
			{
			aPriority = EPriorityAbsoluteLow;
			}
		else if ( str==KEPriorityAbsoluteBackground )
			{
			aPriority = EPriorityAbsoluteBackground;
			}
		else if ( str==KEPriorityAbsoluteForeground )
			{
			aPriority = EPriorityAbsoluteForeground;
			}
		else if ( str==KEPriorityAbsoluteHigh )
			{
			aPriority = EPriorityAbsoluteHigh;
			}
		else
			{
			TInt	intVal;
			if ( aDataWrapper.GetIntFromConfig(aSection, KThreadPriority(), intVal) )
				{
				aPriority=(TThreadPriority)intVal;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TThreadPriority %S"), &KThreadPriority());
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	aDataWrapper.INFO_PRINTF3(_L("ReadThreadPriority %S = %d"), &KThreadPriority(), ret);

	return ret;
	}

TBool CT_MMUtil::ReadFourCC(CDataWrapper& aDataWrapper, const TDesC& aSection, TFourCC& aFourCC)
	{
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, KFourCC(), str);

	if ( ret )
		{
		if( str==KDesMMFFourCCCodePCM8 )
			{
			aFourCC=KMMFFourCCCodePCM8;
			}
		else if( str==KDesMMFFourCCCodePCMU8 )
			{
	 		aFourCC=KMMFFourCCCodePCMU8;
			}
		else if( str==KDesMMFFourCCCodePCM16 )
			{
			aFourCC=KMMFFourCCCodePCM16;
			}
		else if( str==KDesMMFFourCCCodePCMU16 )
			{
			aFourCC=KMMFFourCCCodePCMU16;
			}
		else if( str==KDesMMFFourCCCodePCM16B )
			{
			aFourCC=KMMFFourCCCodePCM16B;
			}
		else if( str==KDesMMFFourCCCodePCMU16B )
			{
	 		aFourCC=KMMFFourCCCodePCMU16B;
			}
		else if( str==KDesMMFFourCCCodeIMAD )
			{
			aFourCC=KMMFFourCCCodeIMAD;
			}
		else if( str==KDesMMFFourCCCodeIMAS )
			{
	 		aFourCC=KMMFFourCCCodeIMAS;
			}
		else if( str==KDesMMFFourCCCodeALAW )
			{
	 		aFourCC=KMMFFourCCCodeALAW;
			}
		else if( str==KDesMMFFourCCCodeMuLAW )
			{
	 		aFourCC=KMMFFourCCCodeMuLAW;
			}
		else if( str==KDesMMFFourCCCodeGSM610 )
			{
	 		aFourCC=KMMFFourCCCodeGSM610;
			}
		else if( str==KDesFourCCCodeNULL )
			{
	 		aFourCC=KMMFFourCCCodeNULL;
			}
		else if( str==KDesMMFFourCCCodeAMR )
			{
	 		aFourCC=KMMFFourCCCodeAMR;
			}
		else if( str==KDesMMFFourCCCodeAAC )
			{
	 		aFourCC=KMMFFourCCCodeAAC;
			}
		else if( str==KDesMMFFourCCCodeAWB )
			{
	 		aFourCC=KMMFFourCCCodeAWB;
			}
		else if( str==KDesMMFFourCCCodeMP3	)
			{
	 		aFourCC=KMMFFourCCCodeMP3;
			}
		else if( str==KDesMMFFourCCCodeATRAC3 )
			{
	 		aFourCC=KMMFFourCCCodeATRAC3;
			}
		else if( str==KDesMMFFourCCCodeSBC )
			{
	 		aFourCC=KMMFFourCCCodeSBC;
			}
		else if( str==KDesMMFFourCCCodeAMRW )
			{
	 		aFourCC=KMMFFourCCCodeAMRW;
			}
		else if( str==KDesMMFFourCCCodeADTS )
			{
	 		aFourCC=KMMFFourCCCodeADTS;
			}
		else
			{
			TInt	intVal;
			if ( aDataWrapper.GetIntFromConfig(aSection, KFourCC(), intVal) )
				{
				aFourCC=intVal;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal TFourCC %S"), &KFourCC());
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	return ret;
	}

TBool CT_MMUtil::ReadImageType(CDataWrapper& aDataWrapper, const TDesC& aSection, CImageDecoder::TImageType& aImageType)
	{
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, KImageType(), str);

	if ( ret )
		{
		if ( str==KImageTypeThumbnail )
			{
			aImageType=CImageDecoder::EImageTypeThumbnail;
			}
		else if ( str==KImageTypeMain )
			{
			aImageType=CImageDecoder::EImageTypeMain;
			}
		else
			{
			TInt	intVal;
			if ( aDataWrapper.GetIntFromConfig(aSection, KImageType(), intVal) )
				{
				aImageType=(CImageDecoder::TImageType)intVal;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal CImageDecoder::TImageType %S"), &KImageType());
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	return ret;
	}

TBool CT_MMUtil::ReadIntent(CDataWrapper& aDataWrapper, const TDesC& aSection, ContentAccess::TIntent& aIntent)
	{
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, KIntent(), str);

	if ( ret )
		{
		if ( str==KIntentPeek )
			{
			aIntent=ContentAccess::EPeek;
			}
		else if ( str==KIntentPlay )
			{
			aIntent=ContentAccess::EPlay;
			}
		else if ( str==KIntentView )
			{
			aIntent=ContentAccess::EView;
			}
		else if ( str==KIntentExecute )
			{
			aIntent=ContentAccess::EExecute;
			}
		else if ( str==KIntentPrint )
			{
			aIntent=ContentAccess::EPrint;
			}
		else if ( str==KIntentPause )
			{
			aIntent=ContentAccess::EPause;
			}
		else if ( str==KIntentContinue )
			{
			aIntent=ContentAccess::EContinue;
			}
		else if ( str==KIntentStop )
			{
			aIntent=ContentAccess::EStop;
			}
		else if ( str==KIntentUnknown )
			{
			aIntent=ContentAccess::EUnknown;
			}
		else if ( str==KIntentInstall )
			{
			aIntent=ContentAccess::EInstall;
			}
		else
			{
			TInt	intVal;
			if ( aDataWrapper.GetIntFromConfig(aSection, KIntent(), intVal) )
				{
				aIntent=(ContentAccess::TIntent)intVal;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal ContentAccess::TIntent %S"), &KIntent());
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	return ret;
	}

TBool CT_MMUtil::ReadMidiState(CDataWrapper& aDataWrapper, const TDesC& aSection, const TDesC& aKeyName, TMidiState& aState)
	{
	TPtrC	str;
	TBool	ret=aDataWrapper.GetStringFromConfig(aSection, aKeyName, str);

	if ( ret )
		{
		if ( str==KMidiStateClosedDisengaged )
			{
			aState=EMidiStateClosedDisengaged;
			}
		else if ( str==KMidiStateOpenDisengaged )
			{
			aState=EMidiStateOpenDisengaged;
			}
		else if ( str==KMidiStateOpenPlaying )
			{
			aState=EMidiStateOpenPlaying;
			}
		else if ( str==KMidiStateClosedEngaged )
			{
			aState=EMidiStateClosedEngaged;
			}
		else if ( str==KMidiStateOpenEngaged )
			{
			aState=EMidiStateOpenEngaged;
			}
		else
			{
			TInt	intVal;
			if ( aDataWrapper.GetIntFromConfig(aSection, aKeyName, intVal) )
				{
				aState=(TMidiState)intVal;
				}
			else
				{
				ret=EFalse;
				aDataWrapper.ERR_PRINTF2(_L("Illegal TMidiState %S"), &aKeyName);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}

	return ret;
	}

TBool CT_MMUtil::ConvertToAudioCaps(const TDesC& aDesc, TMdaAudioDataSettings::TAudioCaps& aAudioCaps)
	{
	TBool	ret=ETrue;

	if ( aDesc==KSampleRateFixed )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRateFixed;
		}
	else if ( aDesc==KSampleRateAnyInRange )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRateAnyInRange;
		}
	else if ( aDesc==KSampleRate8000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate8000Hz;
		}
	else if ( aDesc==KSampleRate11025Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate11025Hz;
		}
	else if ( aDesc==KSampleRate12000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate12000Hz;
		}
	else if ( aDesc==KSampleRate16000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate16000Hz;
		}
	else if ( aDesc==KSampleRate22050Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate22050Hz;
		}
	else if ( aDesc==KSampleRate24000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate24000Hz;
		}
	else if ( aDesc==KSampleRate32000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate32000Hz;
		}
	else if ( aDesc==KSampleRate44100Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate44100Hz;
		}
	else if ( aDesc==KSampleRate48000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate48000Hz;
		}
	else if ( aDesc==KSampleRate96000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate96000Hz;
		}
	else if ( aDesc==KSampleRate64000Hz )
		{
		aAudioCaps=TMdaAudioDataSettings::ESampleRate64000Hz;
		}
	else if ( aDesc==KChannelsMono )
		{
		aAudioCaps=TMdaAudioDataSettings::EChannelsMono;
		}
	else if ( aDesc==KChannelsStereo )
		{
		aAudioCaps=TMdaAudioDataSettings::EChannelsStereo;
		}
	else if ( aDesc==KRoutingControl )
		{
		aAudioCaps=TMdaAudioDataSettings::ERoutingControl;
		}
	else if ( aDesc==KRealTime )
		{
		aAudioCaps=TMdaAudioDataSettings::ERealTime;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

TBool CT_MMUtil::ConvertToAudioFlags(const TDesC& aDesc, TMdaAudioDataSettings::TAudioFlags& aAudioFlags)
	{
	TBool	ret=ETrue;

	if ( aDesc==KNoLocalRouting )
		{
		aAudioFlags=TMdaAudioDataSettings::ENoLocalRouting;
		}
	else if ( aDesc==KNoNetworkRouting )
		{
		aAudioFlags=TMdaAudioDataSettings::ENoNetworkRouting;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

TBool CT_MMUtil::ConvertToOption(const TDesC& aDesc, CImageDecoder::TOptions& aOption)
	{
	TBool	ret=ETrue;

	if ( aDesc==KOptionNone )
		{
		aOption=CImageDecoder::EOptionNone;
		}
	else if ( aDesc==KOptionNoDither )
		{
		aOption=CImageDecoder::EOptionNoDither;
		}
	else if ( aDesc==KOptionAlwaysThread )
		{
		aOption=CImageDecoder::EOptionAlwaysThread;
		}
	else if ( aDesc==KOptionAllowZeroFrameOpen )
		{
		aOption=CImageDecoder::EOptionAllowZeroFrameOpen;
		}
	else if ( aDesc==KAllowGeneratedMask )
		{
		aOption=CImageDecoder::EAllowGeneratedMask;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

TBool CT_MMUtil::ConvertToOption(const TDesC& aDesc, CImageEncoder::TOptions& aOption)
	{
	TBool	ret=ETrue;

	if ( aDesc==KOptionNone )
		{
		aOption=CImageEncoder::EOptionNone;
		}
	else if ( aDesc==KOptionAlwaysThread )
		{
		aOption=CImageEncoder::EOptionAlwaysThread;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

TBool CT_MMUtil::ConvertToAgentProperty(const TDesC& aDesc, ContentAccess::TAgentProperty& aAgentProperty)
	{
	TBool	ret=ETrue;

	if ( aDesc==KAgentPropertyBufferSize )
		{
		aAgentProperty=ContentAccess::EAgentPropertyBufferSize;
		}
	else if ( aDesc==KAgentPropertyAgentUI )
		{
		aAgentProperty=ContentAccess::EAgentPropertyAgentUI;
		}
	else if ( aDesc==KAgentPropertyMultipleSequence )
		{
		aAgentProperty=ContentAccess::EAgentPropertyMultipleSequence;
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

const TDesC& CT_MMUtil::GetFourCCString(TUint32 aValue)
	{
	switch(aValue)
		{
	case KMMFFourCCCodePCM8:
		return KDesMMFFourCCCodePCM8;
	case KMMFFourCCCodePCMU8:
		return KDesMMFFourCCCodePCMU8;
	case KMMFFourCCCodePCM16:
		return KDesMMFFourCCCodePCM16;
	case KMMFFourCCCodePCMU16:
		return KDesMMFFourCCCodePCMU16;
	case KMMFFourCCCodePCM16B:
		return KDesMMFFourCCCodePCM16B;
	case KMMFFourCCCodePCMU16B:
		return KDesMMFFourCCCodePCMU16B;
	case KMMFFourCCCodeIMAD:
		return KDesMMFFourCCCodeIMAD;
	case KMMFFourCCCodeIMAS:
		return  KDesMMFFourCCCodeIMAS;
	case KMMFFourCCCodeALAW:
		return KDesMMFFourCCCodeALAW;
	case KMMFFourCCCodeMuLAW:
		return KDesMMFFourCCCodeMuLAW;
	case KMMFFourCCCodeGSM610:
		return KDesMMFFourCCCodeGSM610;
	case KMMFFourCCCodeNULL:
		return KDesFourCCCodeNULL;
	case KMMFFourCCCodeAMR:
		return KDesMMFFourCCCodeAMR;
	case KMMFFourCCCodeAAC:
		return KDesMMFFourCCCodeAAC;
	case KMMFFourCCCodeAWB:
		return KDesMMFFourCCCodeAWB;
	case KMMFFourCCCodeMP3:
		return KDesMMFFourCCCodeMP3;
	case KMMFFourCCCodeATRAC3:
		return KDesMMFFourCCCodeATRAC3;
	case KMMFFourCCCodeSBC:
		return KDesMMFFourCCCodeSBC;
	case KMMFFourCCCodeAMRW:
		return KDesMMFFourCCCodeAMRW;
	case KMMFFourCCCodeADTS:
		return KDesMMFFourCCCodeADTS;
	default:
		break;
		};
	return KDesMMFFourCCCodeUnknown;
	}

TBool CT_MMUtil::ReadAudioDataSettings(CDataWrapper& aDataWrapper, TMdaAudioDataSettings& aSettings, const TDesC& aSection)
	{
	TBool	ret=EFalse;
	TBool	callQuery=ETrue;

	//	Read capabilities
	TInt	index=0;
	TBool	moreData=ETrue;
	TPtrC	capabilityString;
	TInt	capability=0;
	TBool	hasCapabilty=EFalse;
	while ( moreData )
		{
		TBuf<KMaxTestExecuteCommandLength>	capabilityTemp;
		capabilityTemp.Format(KCmdCapabilty(), ++index);
		if ( aDataWrapper.GetStringFromConfig(aSection, capabilityTemp, capabilityString) )
			{
			TMdaAudioDataSettings::TAudioCaps	audioCap;
			if ( ConvertToAudioCaps(capabilityString, audioCap) )
				{
				capability |= audioCap;
				hasCapabilty=ETrue;
				}
			else
				{
				aDataWrapper.ERR_PRINTF2(_L("Illegal capability %S"), &capabilityString);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		else
			{
			moreData=EFalse;
			}
		}
	if ( hasCapabilty )
		{
		aSettings.iCaps=capability;
		ret=ETrue;
		}

	//	Read channels
	TInt	channels=0;
	if ( aDataWrapper.GetIntFromConfig(aSection, KCmdChannels(), channels) )
		{
		aSettings.iChannels=channels;
		callQuery=EFalse;
		ret=ETrue;
		}

	//	Read flags
	TMdaAudioDataSettings::TAudioFlags	flags;
	TPtrC								flagsString;
	if ( aDataWrapper.GetStringFromConfig(aSection, KCmdFlags(), flagsString) )
		{
		if ( ConvertToAudioFlags(flagsString, flags) )
			{
			aSettings.iFlags=flags;
			ret=ETrue;
			}
		else
			{
			aDataWrapper.ERR_PRINTF2(_L("Illegal flags %S"), &flagsString);
			aDataWrapper.SetBlockResult(EFail);
			}
		}

	//	Read max volume
	TInt	maxVolume=0;
	if ( aDataWrapper.GetIntFromConfig(aSection, KCmdMaxVolume(), maxVolume) )
		{
		aSettings.iMaxVolume=maxVolume;
		ret=ETrue;
		}

	//	Read sample rate
	TInt	sampleRate=0;
	if ( aDataWrapper.GetIntFromConfig(aSection, KCmdSampleRate(), sampleRate) )
		{
		aSettings.iSampleRate=sampleRate;
		callQuery=EFalse;
		ret=ETrue;
		}

	//	Read volume
	TInt	volume=0;
	if ( aDataWrapper.GetIntFromConfig(aSection, KCmdVolume(), volume) )
		{
		aSettings.iVolume=volume;
		callQuery=EFalse;
		ret=ETrue;
		}

	if ( callQuery )
		{
		aSettings.Query();
		}

	return ret;
	}

TBool CT_MMUtil::ConvertToDisplayMode(const TDesC& aDisplayModeStr, TDisplayMode& aDisplayMode)
	{
	TBool	ret = ETrue;

	if (aDisplayModeStr == KDModeNone)
		{
		aDisplayMode = ENone;
		}
	else if (aDisplayModeStr == KDModeGray2)
		{
		aDisplayMode = EGray2;
		}
	else if (aDisplayModeStr == KDModeGray4)
		{
		aDisplayMode = EGray4;
		}
	else if (aDisplayModeStr == KDModeGray16)
		{
		aDisplayMode = EGray16;
		}
	else if (aDisplayModeStr == KDModeGray256)
		{
		aDisplayMode = EGray256;
		}
	else if (aDisplayModeStr == KDModeColor16)
		{
		aDisplayMode = EColor16;
		}
	else if (aDisplayModeStr == KDModeColor256)
		{
		aDisplayMode = EColor256;
		}
	else if (aDisplayModeStr == KDModeColor64K)
		{
		aDisplayMode = EColor64K;
		}
	else if (aDisplayModeStr == KDModeColor16M)
		{
		aDisplayMode = EColor16M;
		}
	else if (aDisplayModeStr == KDModeRgb)
		{
		aDisplayMode = ERgb;
		}
	else if (aDisplayModeStr == KDModeColor4K)
		{
		aDisplayMode = EColor4K;
		}
	else if (aDisplayModeStr == KDModeColor16MU)
		{
		aDisplayMode = EColor16MU;
		}
	else if (aDisplayModeStr == KDModeColor16MA)
		{
		aDisplayMode = EColor16MA;
		}
	else if (aDisplayModeStr == KDModeColorLast)
		{
		aDisplayMode = EColorLast;
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}

TBool CT_MMUtil::ConvertToVideoRotation(const TDesC& aVideoRotationStr, TVideoRotation& aVideoRotation)
	{
	TBool	ret = ETrue;

	if (aVideoRotationStr == KEVideoRotationNone)
		{
		aVideoRotation = EVideoRotationNone;
		}
	else if (aVideoRotationStr == KEVideoRotationClockwise90)
		{
		aVideoRotation = EVideoRotationClockwise90;
		}
	else if (aVideoRotationStr == KEVideoRotationClockwise180)
		{
		aVideoRotation = EVideoRotationClockwise180;
		}
	else if (aVideoRotationStr == KEVideoRotationClockwise270)
		{
		aVideoRotation = EVideoRotationClockwise270;
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}

const TDesC& CT_MMUtil::GetVideoRotationString(TVideoRotation& aVideoRotation)
	{
	switch(aVideoRotation)
		{
	case EVideoRotationNone:
		return KEVideoRotationNone;
	case EVideoRotationClockwise90:
		return KEVideoRotationClockwise90;
	case EVideoRotationClockwise180:
		return KEVideoRotationClockwise180;
	case EVideoRotationClockwise270:
		return KEVideoRotationClockwise270;
	default:
		break;
		};
	return KEVideoRotationUnknown;
	}


void CT_MMUtil::VerifyMIMETypeArrayL(CDataWrapper& aDataWrapper, const TDesC& aSection, const RFileExtensionMIMETypeArray& aFileExtensionArray)
	{


	TInt	count=aFileExtensionArray.Count();
	for ( TInt index=0; index<count; ++index )
		{
		CFileExtensionMIMEType*	data=aFileExtensionArray[index];
		const TDesC&			displayName=data->DisplayName();
		const TDesC&			fileExtension=data->FileExtension();
		TUid					imageSubType=data->ImageSubType();
		TUid					imageType=data->ImageType();
		TUid					implementationUid=data->ImplementationUid();
		const TDesC8&			mimeType8=data->MIMEType();
		HBufC*					mimeType=HBufC::NewLC(mimeType8.Length());
		TPtr					mimeDes=mimeType->Des();
		mimeDes.Copy(mimeType8);
		aDataWrapper.INFO_PRINTF3(_L("DisplayName[%d]      =%S"), index, &displayName);
		aDataWrapper.INFO_PRINTF3(_L("FileExtension[%d]    =%S"), index, &fileExtension);
		aDataWrapper.INFO_PRINTF3(_L("ImageSubType[%d]     =0x%X"), index, imageSubType.iUid);
		aDataWrapper.INFO_PRINTF3(_L("ImageType[%d]        =0x%X"), index, imageType.iUid);
		aDataWrapper.INFO_PRINTF3(_L("ImplementationUid[%d]=0x%X"), index, implementationUid.iUid);
		aDataWrapper.INFO_PRINTF3(_L("MIMEType[%d]         =%S"), index, &mimeDes);
		CleanupStack::PopAndDestroy(mimeType);

		//	Verify that it is unique in the array
		for ( TInt index2=index+1; index2<count; ++index2 )
			{
			CFileExtensionMIMEType*	data2=aFileExtensionArray[index2];
			if	(	(fileExtension.Compare(data2->FileExtension())==0) &&
					(imageSubType==data2->ImageSubType()) &&
					(imageType==data2->ImageType()) &&
					(implementationUid==data2->ImplementationUid()) &&
					(mimeType8.Compare(data2->MIMEType())==0)
				)
				{
				aDataWrapper.ERR_PRINTF3(_L("Duplicate Types in entries %d and %d"), index, index2);
				aDataWrapper.SetBlockResult(EFail);
				}
			}
		}
	TInt	expectedCount=0;
	if ( aDataWrapper.GetIntFromConfig(aSection, KCount(), expectedCount) )
		{
		if ( count != expectedCount )
			{
			aDataWrapper.ERR_PRINTF3(_L("count (%d) != expectedCount (%d)"), count, expectedCount);
			aDataWrapper.SetBlockResult(EFail);
			}
		}
	}

void CT_MMUtil::GetControllerAndFormatUidL(CDataWrapper& aDataWrapper, const TDesC& aFileName, TUid aMediaId, TBool aRecord, TUid& aControllerUid, TUid& aFormatUid)
	{
	RFs	fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TParse	parse;
	fs.Parse(aFileName, parse);
	TPtrC	extension=parse.Ext();

	CMMFControllerPluginSelectionParameters*	controllerSelection = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters*				formatSelection = CMMFFormatSelectionParameters::NewLC();

	// Set the play and record format selection parameters to be blank.
	// Format support is only retrieved if requested.
	controllerSelection->SetRequiredPlayFormatSupportL(*formatSelection);
	controllerSelection->SetRequiredRecordFormatSupportL(*formatSelection);

	// Set the media ids
	RArray<TUid>	mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(aMediaId));

	// Get plugins that support at least audio
	controllerSelection->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	controllerSelection->SetPreferredSupplierL(KNullDesC, CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

	// Array to hold all the controllers support the match data
	RMMFControllerImplInfoArray	controllers;
	CleanupResetAndDestroyPushL(controllers);
	controllerSelection->ListImplementationsL(controllers);

	// Find the first controller with at least one record format available
	aControllerUid=TUid::Null();
	aFormatUid=TUid::Null();

	TBool	set = EFalse;
	TBool	stillNull = ETrue;
	for( TInt controllerIndex=0; (controllerIndex<controllers.Count()) && !set; ++controllerIndex )
		{
		const CMMFControllerImplementationInformation*	controller=controllers[controllerIndex];
		LogL(aDataWrapper, *controller);

		if ( aRecord )
			{
			const RMMFFormatImplInfoArray&	formats=controller->RecordFormats();

			for( TInt formatIndex=0; (formatIndex<formats.Count()) && !set; ++formatIndex )
				{
				CMMFFormatImplementationInformation*	formatInfo=formats[formatIndex];
				const CDesC8Array&						extensions=formatInfo->SupportedFileExtensions();

				for ( TInt extensionsIndex=0; (extensionsIndex<extensions.Count()) && !set; ++extensionsIndex )
					{
					HBufC*	buffer=HBufC::NewLC(extensions[extensionsIndex].Length());
					TPtr	bufferPtr=buffer->Des();
					bufferPtr.Copy(extensions[extensionsIndex]);	// Create a 16 bit copy of the 8 bit original

					if ( stillNull )
						{
						aControllerUid = controller->Uid();
						aFormatUid = formats[0]->Uid();
						stillNull = EFalse;
						}
					if ( bufferPtr == extension )
						{
						aControllerUid = controller->Uid();
						aFormatUid = formats[0]->Uid();
						set = ETrue;
						}
					CleanupStack::PopAndDestroy(buffer);
					}
				}
			}
		else
			{
			const RMMFFormatImplInfoArray&	formats=controller->PlayFormats();

			for( TInt formatIndex=0; (formatIndex<formats.Count()) && !set; ++formatIndex )
				{
				CMMFFormatImplementationInformation*	formatInfo=formats[formatIndex];
				const CDesC8Array&						extensions=formatInfo->SupportedFileExtensions();

				for ( TInt extensionsIndex=0; (extensionsIndex<extensions.Count()) && !set; ++extensionsIndex )
					{
					HBufC*	buffer=HBufC::NewLC(extensions[extensionsIndex].Length());
					TPtr	bufferPtr=buffer->Des();
					bufferPtr.Copy(extensions[extensionsIndex]);	// Create a 16 bit copy of the 8 bit original

					if ( stillNull )
						{
						aControllerUid = controller->Uid();
						aFormatUid = formats[0]->Uid();
						stillNull = EFalse;
						}
					if ( bufferPtr == extension )
						{
						aControllerUid = controller->Uid();
						aFormatUid = formats[0]->Uid();
						set = ETrue;
						}
					CleanupStack::PopAndDestroy(buffer);
					}
				}
			}
		}

	CleanupStack::PopAndDestroy(5, &fs);
	}

void CT_MMUtil::LogL(CDataWrapper& aDataWrapper, const CMMFControllerImplementationInformation& aControllerInfo)
	{
	aDataWrapper.INFO_PRINTF2(_L("Controller DisplayName = %S"),	&(aControllerInfo.DisplayName()));
	aDataWrapper.INFO_PRINTF2(_L("Controller Supplier = %S"),		&(aControllerInfo.Supplier()));
	aDataWrapper.INFO_PRINTF2(_L("Controller Format Uid = 0x%X"),	aControllerInfo.Uid().iUid);

	// Print out Play Formats information
	TInt	index;

	const RMMFFormatImplInfoArray&	playFormatArray = aControllerInfo.PlayFormats();
	for ( index=0; index<playFormatArray.Count(); ++index )
		{
		CMMFFormatImplementationInformation*	formatInfo=playFormatArray[index];
		aDataWrapper.INFO_PRINTF2(_L("PlayFormat [%d]"),		index);
		LogL(aDataWrapper, *formatInfo);
		}

	// Print out Record Formats information
	const RMMFFormatImplInfoArray&	recordFormatArray = aControllerInfo.RecordFormats();
	for ( index=0; index<recordFormatArray.Count(); ++index )
		{
		CMMFFormatImplementationInformation*	formatInfo=recordFormatArray[index];
		aDataWrapper.INFO_PRINTF2(_L("RecordFormat [%d]"),		index);
		LogL(aDataWrapper, *formatInfo);
		}
	}

void CT_MMUtil::LogL(CDataWrapper& aDataWrapper, const CMMFFormatImplementationInformation& aFormatInfo)
	{
	aDataWrapper.INFO_PRINTF2(_L("  DisplayName = %S"),	&(aFormatInfo.DisplayName()));
	aDataWrapper.INFO_PRINTF2(_L("  Supplier = %S"),		&(aFormatInfo.Supplier()));
	aDataWrapper.INFO_PRINTF2(_L("  Uid = 0x%X"),			aFormatInfo.Uid().iUid);

	TInt				index;
	const CDesC8Array&	fileext=aFormatInfo.SupportedFileExtensions();

	for ( index=0; index<fileext.Count(); ++index )
		{
		HBufC*	buffer=HBufC::NewLC(fileext[index].Length());
		TPtr	bufferPtr=buffer->Des();
		bufferPtr.Copy(fileext[index]);	// Create a 16 bit copy of the 8 bit original
		aDataWrapper.INFO_PRINTF3(_L("    Supported file extension [%d]: %S"), index, &bufferPtr);
		CleanupStack::PopAndDestroy(buffer);
		}

	const CDesC8Array&	mimetypes=aFormatInfo.SupportedMimeTypes();
	for ( index=0; index<mimetypes.Count(); ++index )
		{
		HBufC*	buffer=HBufC::NewLC(mimetypes[index].Length());
		TPtr	bufferPtr=buffer->Des();
		bufferPtr.Copy(mimetypes[index]);	// Create a 16 bit copy of the 8 bit original
		aDataWrapper.INFO_PRINTF3(_L("    Supported mime type [%d]: %S"), index, &bufferPtr);
		CleanupStack::PopAndDestroy(buffer);
		}

	const CDesC8Array&	headerdata=aFormatInfo.SupportedHeaderData();
	for ( index=0; index<headerdata.Count(); ++index )
		{
		HBufC*	buffer=HBufC::NewLC(headerdata[index].Length());
		TPtr	bufferPtr=buffer->Des();
		bufferPtr.Copy(headerdata[index]);	// Create a 16 bit copy of the 8 bit original
		aDataWrapper.INFO_PRINTF3(_L("    Supported header data [%d]: %S"), index, &bufferPtr);
		CleanupStack::PopAndDestroy(buffer);
		}
	}
