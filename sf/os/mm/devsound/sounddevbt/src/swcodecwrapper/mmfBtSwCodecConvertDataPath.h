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

#ifndef __MMFBTSWCODECCONVERTDATAPATH_H__
#define __MMFBTSWCODECCONVERTDATAPATH_H__

#include <e32base.h>
#include <mdasound.h>

#include "mmfBtSwCodecDataPath.h"

/**
 *  Derived class for convert datapath internal to the Sw codec wrapper
 *  @internalComponent
 */
class CMMFSwCodecConvertDataPath : public CMMFSwCodecDataPath
	{ 
public:	
	static CMMFSwCodecConvertDataPath* NewL();
	virtual ~CMMFSwCodecConvertDataPath(); 
	virtual TInt SetObserver(MMMFHwDeviceObserver &aHwObserver);
	virtual TInt AddCodec(CMMFSwCodec& aCodec);
	virtual TInt Start();
	virtual void Stop();
	virtual void Pause();
	virtual void BufferFilledL(CMMFDataBuffer& aBuffer);
	virtual void BufferEmptiedL(const CMMFDataBuffer& aBuffer);	
	virtual void SoundDeviceException(TInt aError);
	virtual RMdaDevSound& Device();
	void FillSourceBufferL();
	void FillSinkBufferL();
	void EmptySinkBufferL();
private:	
	CMMFSwCodecConvertDataPath() {};
	void ConstructL();	
private: 
	/**
     * Private class for the convert datapath that is an active object
	 * used to drive the conversion.  This class performs the main data transfer
	 * between the source and the sink.
     * This is done in a separate class as opposed to CMMFSwCodecConvertDataPath
     * because the class needs to be an active object to avoid recursive call stacks
     * in cases where the source and sink are not active objects - which is
     * the case with descriptors.  Making CMMFSwCodecConvertDataPath derive
     * from CActive is less desirable as it would involve multiple inheretence
     * @internalComponent
     */
	class CDataPathConverter : public CActive
	{
	public:
		enum TConvertState
		{
		EIdle,
		EFillingSourceBuffer,
		EFillingSinkBuffer,
		EEmptyingSinkBuffer
		};
	public:
		CDataPathConverter(CMMFSwCodecConvertDataPath& aParent, TInt aPriority);
		~CDataPathConverter();
		void Start();
		void BufferFilledL(CMMFDataBuffer& aBuffer);
		void BufferEmptiedL(CMMFDataBuffer& aBuffer);
		void ChangeConvertState(TConvertState aNewConvertState);
		//CActive
		virtual void RunL();
		virtual TInt RunError(TInt aError);
		virtual void DoCancel();
	private:	
		void FillSourceBufferL();
		void FillSinkBufferL();
		void EmptySinkBufferL();
	private:
		TConvertState iConvertState;
		CMMFSwCodecConvertDataPath& iParent;
		CMMFDataBuffer* iSourceBuffer;
	};
private: 
	CDataPathConverter* iDataPathConverter;
	CMMFDataBuffer* iSourceBuffer;
	CMMFDataBuffer* iSinkBuffer;
	TBool iNoMoreSourceData;
	TUint iSinkBufferSize;
	TUint iSourceBufferSize;
	RMdaDevSound iDummyDevSound;//don't need a devsound for convert
	};

#endif

