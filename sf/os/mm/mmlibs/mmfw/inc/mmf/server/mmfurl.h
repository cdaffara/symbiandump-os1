// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMFURL_H_
#define __MMFURL_H_

#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>
#include <s32mem.h>


/**
@publishedAll
@released

Concrete implementation of a URL data source.

This class is used only to pass information regarding the URL and IAP into the
controller plugin. IT CANNOT BE USED TO READ DATA FROM A URL. Each of the MDataSource
APIs are implemented as either returning or leaving with KErrNotSupported as appropriate.

The configuration data for this class is an externalized CMMFUrlParams object.

@see    CMMFUrlParams
*/
class CMMFUrlSource : public CBase, public MDataSource
	{
public:
	static MDataSource* NewSourceL();
	
	/**
	Destructor.
	*/
	virtual ~CMMFUrlSource();

	/**
	Returns the URL string.

	For example:
	@code
	http://www.symbian.com/clip/mpg
	@endcode

	@return The URL string.
	@since	7.0s
	*/
	const TDesC& Url() const {return *iUrl;}

	/**
	Indicates whether the IAP ID should be used.

	@return A boolean indicating if the supplied IAPId should be used. ETrue if the supplied IAPId
	        should be used. EFalse if the default IAPId should be used.

	@since	7.0s
	*/
	TBool UseIAPId() const {return iUseIAPId;}

	/**
	Returns the Internet Access Point ID.

	@return The IAP ID.
	@since	7.0s
	*/
	TInt IAPId() const {return iIAPId;}

	TFourCC SourceDataTypeCode(TMediaId aMediaId);
	TInt SetSourceDataTypeCode(TFourCC /*aSourceFourCC*/, TMediaId /*aMediaId*/);
	void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer,TMediaId /*aMediaId*/);
	void BufferEmptiedL(CMMFBuffer* aBuffer);
	TBool CanCreateSourceBuffer();
	CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference);
	TInt SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	void SourceThreadLogoff();
	void NegotiateSourceL(MDataSink& /* aDataSink*/);
	TBool SourceSampleConvert();
	void SourcePrimeL();
	void SourcePlayL();
	void SourcePauseL();
	void SourceStopL();
	void SetSourcePrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/);
	void SourceCustomCommand(TMMFMessage& aMessage);

private:
	CMMFUrlSource();
	void ConstructSourceL(const TDesC8& aInitData);
private:
	HBufC* iUrl;
	TBool iUseIAPId;
	TInt iIAPId;
	};



/**
@publishedAll
@released

Concrete implementation of a URL data sink.

This class is used only to pass information regarding the URL and IAP into the
controller plugin.  IT CANNOT BE USED TO WRITE DATA TO A URL.  Each of the MDataSink
APIs are implemented as either returning or leaving with KErrNotSupported as appropriate.

The configuration data for this class is an externalized CMMFUrlParams object.

@see    CMMFUrlParams
*/
class CMMFUrlSink : public CBase, public MDataSink
	{
public:
	static MDataSink* NewSinkL();
	virtual ~CMMFUrlSink();

	/**
	Returns the URL string.

	For example:
	@code
	http://www.symbian.com/clip/mpg
	@endcode

	@return The URL string.

	@since  7.0s
	*/
	const TDesC& Url() const {return *iUrl;}

	/**
	Indicates whether the IAP ID should be used.

	@return A boolean indicating if the supplied IAP ID should be used. ETrue if the supplied ID
	        should be used, EFalse if the default IAP ID should be used.

	@since	7.0s
	*/
	TBool UseIAPId() const {return iUseIAPId;}

	/**
	Returns the Internet Access Point ID.

	@return The IAP ID.

	@since  7.0s
	*/
	TInt IAPId() const {return iIAPId;}

	TFourCC SinkDataTypeCode(TMediaId /*aMediaId*/);
	TInt SetSinkDataTypeCode(TFourCC /*aSinkFourCC*/, TMediaId /*aMediaId*/);
	void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/);
	void BufferFilledL(CMMFBuffer* aBuffer);
	TBool CanCreateSinkBuffer();
	CMMFBuffer* CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	TInt SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	void SinkThreadLogoff();
	void NegotiateL(MDataSource& /* aDataSource*/);
	void SinkPrimeL();
	void SinkPlayL();
	void SinkPauseL();
	void SinkStopL();
	void SetSinkPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/);
	void SinkCustomCommand(TMMFMessage& aMessage);

private:
	CMMFUrlSink();
	void ConstructSinkL(const TDesC8& aInitData);
private:
	HBufC* iUrl;
	TBool iUseIAPId;
	TInt iIAPId;
	};



#endif
