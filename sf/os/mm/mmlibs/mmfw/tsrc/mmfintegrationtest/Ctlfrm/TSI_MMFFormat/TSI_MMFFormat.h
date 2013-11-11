// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSI_MMF_FORMAT_H__
#define __TSI_MMMF_FORMAT_H__

#include <mmf/server/mmfformat.h>
#include <ecom/implementationproxy.h>

class CMMFReadFormat : public CMMFFormatDecode, public MAsyncEventHandler
	{
public:
	static CMMFReadFormat* NewL();
	virtual ~CMMFReadFormat();
	
	virtual TUint Streams(TUid aMediaType) const;
	virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;

	//from MDataSource
	virtual void ConstructSourceL(const TDesC8& aInitData);
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference); 
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference);
	virtual void BufferEmptiedL(CMMFBuffer* aBuffer); 
	virtual TBool CanCreateSourceBuffer();
	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId); //returns FourCC code for the mediaId
	virtual TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId aMediaId);
	virtual TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler);
	virtual void SourceThreadLogoff();
	virtual void NegotiateSourceL(MDataSink& aSink);
	virtual void SourcePrimeL();
	virtual void SourcePlayL();
	virtual void SourcePauseL();
	virtual void SourceStopL();

	//from MAsyncEventHandler
	virtual TInt SendEventToClient(const TMMFEvent& /*aEvent*/) {return KErrNone;}
	
private:
	CMMFReadFormat();


	};


#endif

