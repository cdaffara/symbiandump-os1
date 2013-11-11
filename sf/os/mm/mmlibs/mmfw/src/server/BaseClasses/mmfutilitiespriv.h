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
// include\mmf\common\mmfutilitiespriv.h
// 
//

#ifndef MMFUTILITIESPRIV_H
#define MMFUTILITIESPRIV_H

#include <mmf/common/mmfutilities.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfhelper.h>
#endif


class CChannelAndSampleRateConverter;

/*
version of CMMFChannelAndSampleRateConverter that is implemented as adaptor to 
CChannelAndSampleRateConverter class
*/
NONSHARABLE_CLASS (CMMFForwardingChannelAndSampleRateConverter) : public CMMFChannelAndSampleRateConverter
	{
public:
	static CMMFForwardingChannelAndSampleRateConverter* NewL(TInt aFromRate,TInt aFromChannels,
										    			      TInt aToRate,TInt aToChannels);
										    			     
	~CMMFForwardingChannelAndSampleRateConverter();

protected:
	CMMFForwardingChannelAndSampleRateConverter();
	void ConstructL(TInt aFromRate,TInt aFromChannels,
					TInt aToRate,TInt aToChannels);
					
	// from CMMFChannelAndSampleRateConverter
	TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer);
	void Reset();
	TUint MaxConvertBufferSize(TUint aSrcBufferSize);
	
private:
	CChannelAndSampleRateConverter* iRealConverter;
	};

#endif
