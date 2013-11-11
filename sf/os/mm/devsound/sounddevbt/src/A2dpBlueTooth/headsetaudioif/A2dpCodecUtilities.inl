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


inline void CA2dpAudioCodecConfiguration::SetHeadsetCodecDataType(const TFourCC& aHeadsetCodecDataType)
	{
	iHeadsetCodecDataType = aHeadsetCodecDataType;
	}
	
	
inline void CA2dpAudioCodecConfiguration::SetSampleRate(TUint aSampleRate)
	{
	iSampleRate = aSampleRate;
	}
	
	
inline void CA2dpAudioCodecConfiguration::SetChannels(TUint aChannels)
	{
	iChannels = aChannels;
	}
	

inline void CA2dpAudioCodecConfiguration::SetStereoSupport(TMMFStereoSupport aStereoSupport)
	{
	iStereoSupport = aStereoSupport;
	}


inline const TFourCC& CA2dpAudioCodecConfiguration::HeadsetCodecDataType() const	
	{
	return iHeadsetCodecDataType;
	}
	
	
inline TUint CA2dpAudioCodecConfiguration::SampleRate() const
	{
	return iSampleRate;
	}


inline TUint CA2dpAudioCodecConfiguration::Channels() const
	{
	return iChannels;
	}
	
	
inline TMMFStereoSupport CA2dpAudioCodecConfiguration::StereoSupport() const
	{
	return iStereoSupport;
	}


inline TAvdtpMediaCodecCapabilities* CA2dpAudioCodecConfiguration::RemoteCodecConfiguration() const
	{
	return iRemoteCodecConfiguration;
	}

	
inline const TSBCFrameParameters& CA2dpAudioCodecConfiguration::LocalSBCCodecConfiguration() const
	{
	return iLocalSBCCodecConfiguration;
	}



inline TUint CA2dpCodecFrameHeaderParser::FrameLength() const
	{
	return iFrameLength;
	}
	

inline TUint CA2dpCodecFrameHeaderParser::BitRate() const
	{
	return iBitRate;
	}
	
