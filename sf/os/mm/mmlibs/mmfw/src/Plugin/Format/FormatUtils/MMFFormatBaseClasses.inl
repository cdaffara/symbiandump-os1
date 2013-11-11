// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



TUint CMMFFormatRead::NumChannels() 
	{
	return iChannels;
	}

TUint CMMFFormatRead::SampleRate()
	{
	return iSampleRate;
	}


TUint CMMFFormatRead::BitRate() 
	{
	return (iSampleRate * iBitsPerSample * iChannels);
	}


TUint CMMFFormatRead::MaxNumChannels() 
	{
	return KStereo; //default support for mono & stereo
	}


TBool CMMFFormatRead::SourceSampleConvert() 
	{
	return iNeedsSWConversion;
	}

CMMFFormatRead::CMMFFormatRead() : iState(EUninitialised) 
	{}


TInt CMMFFormatRead::OKToPlay() 
	{
	return KErrNone;
	}




TUint CMMFFormatWrite::NumChannels() 
	{
	return iChannels;
	}

TUint CMMFFormatWrite::SampleRate() 
	{
	return iSampleRate;
	}

TUint CMMFFormatWrite::GetDefaultSampleRate() 
	{
	return iSampleRate;
	}

TUint CMMFFormatWrite::BitRate() 
	{
	return (iSampleRate * iBitsPerSample * iChannels);
	}

	
TInt CMMFFormatWrite::MaximumClipSize() 
	{
	return iMaximumClipSize;
	}

TUint CMMFFormatWrite::MaxNumChannels() 
	{
	return KStereo; //default support for mono & stereo
	}

CMMFFormatWrite::CMMFFormatWrite() : iState(EUninitialised) 
	{}


TInt CMMFFormatWrite::OKToRecord() 
	{
	return KErrNone;
	}

