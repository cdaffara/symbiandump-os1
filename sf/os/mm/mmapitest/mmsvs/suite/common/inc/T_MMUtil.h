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



#if (!defined __T_MM_UTIL_H__)
#define __T_MM_UTIL_H__

#include <mmf/common/mmfutilities.h>
#include <mda/common/audio.h>
#include <imageconversion.h>
#include <datawrapper.h>
#include <mmf/common/mmfvideo.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <midiclientutility.h>

class CT_MMUtil
	{
public:
	static TBool		ReadMdaPriority(CDataWrapper& aDataWrapper, const TDesC& aSection, TInt& aPriority);
	static TBool		ReadMdaPriorityPreference(CDataWrapper& aDataWrapper, const TDesC& aSection, TMdaPriorityPreference& aPreference);
	static TBool		ReadThreadPriority(CDataWrapper& aDataWrapper, const TDesC& aSection, TThreadPriority& aPriority);
	static TBool		ReadFourCC(CDataWrapper& aDataWrapper, const TDesC& aSection, TFourCC& aFourCC);
	static TBool		ReadImageType(CDataWrapper& aDataWrapper, const TDesC& aSection, CImageDecoder::TImageType& aImageType);
	static TBool		ReadIntent(CDataWrapper& aDataWrapper, const TDesC& aSection, ContentAccess::TIntent& aIntent);
	static TBool		ReadMidiState(CDataWrapper& aDataWrapper, const TDesC& aSection, const TDesC& aKeyName, TMidiState& aState);
	static TBool		ConvertToAudioCaps(const TDesC& aDesc, TMdaAudioDataSettings::TAudioCaps& aAudioCaps);
	static TBool		ConvertToAudioFlags(const TDesC& aDesc, TMdaAudioDataSettings::TAudioFlags& aAudioFlags);
	static TBool		ConvertToOption(const TDesC& aDesc, CImageDecoder::TOptions& aOption);
	static TBool		ConvertToOption(const TDesC& aDesc, CImageEncoder::TOptions& aOption);
	static TBool		ConvertToAgentProperty(const TDesC& aDesc, ContentAccess::TAgentProperty& aAgentProperty);
	static const TDesC&	GetFourCCString(TUint32 aValue);
	static TBool		ReadAudioDataSettings(CDataWrapper& aDataWrapper, TMdaAudioDataSettings& aSettings, const TDesC& aSection);
	static TBool		ConvertToDisplayMode(const TDesC& aDisplayModeStr, TDisplayMode& aDisplayMode);
	static TBool		ConvertToVideoRotation(const TDesC& aVideoRotationStr, TVideoRotation& aVideoRotation);
	static const TDesC&	GetVideoRotationString(TVideoRotation& aVideoRotation);
	static void			VerifyMIMETypeArrayL(CDataWrapper& aDataWrapper, const TDesC& aSection, const RFileExtensionMIMETypeArray& aFileExtensionArray);
	static void			GetControllerAndFormatUidL(CDataWrapper& aDataWrapper, const TDesC& aFileName, TUid aMediaId, TBool aRecord, TUid& aControllerUid, TUid& aFormatUid);
	static void			LogL(CDataWrapper& aDataWrapper, const CMMFControllerImplementationInformation& aControllerInfo);

private:
	static void			LogL(CDataWrapper& aDataWrapper, const CMMFFormatImplementationInformation& aFormatInfo);
	};

#endif /* __T_MM_UTIL_H__ */
