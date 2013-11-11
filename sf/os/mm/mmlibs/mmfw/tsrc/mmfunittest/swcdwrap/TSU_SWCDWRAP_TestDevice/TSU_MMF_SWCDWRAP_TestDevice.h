/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_SWCDWRAP_TESTDEVICE_H__
#define __TSU_MMF_SWCDWRAP_TESTDEVICE_H__


#include <mmf/server/mmfswcodecwrapper.h>


const TInt KTestBufferSize = 0x2000;
const TInt KTest2To1SourceBufferSize = 8192; // these values make sense for current a3f adaptation, but are adaptation specific
const TInt KTest2To1SinkBufferSize = 4096;

class CMMFTestNullDevice : public CMMFSwCodecWrapper
	{
public:
	static CMMFTestNullDevice* NewL();
	virtual CMMFSwCodec& Codec();
    inline ~CMMFTestNullDevice(); 
private:
	void ConstructL();
	};


class CMMFTestNullCodec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& /*aSource*/, CMMFBuffer& /*aDest*/);
	virtual TUint SourceBufferSize() {return KTestBufferSize;};
	virtual TUint SinkBufferSize() {return KTestBufferSize;};
	virtual TBool IsNullCodec() {return ETrue;};
	};


class CMMFTest2To1Device : public CMMFSwCodecWrapper
	{
public:
	static CMMFTest2To1Device* NewL();
	virtual CMMFSwCodec& Codec();
    inline ~CMMFTest2To1Device(); 
private:
	void ConstructL();
	};


class CMMFTest2To1Codec : public CMMFSwCodec
	{
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& /*aSource*/, CMMFBuffer& /*aDest*/);
	virtual TUint SourceBufferSize() {return KTest2To1SourceBufferSize;};
	virtual TUint SinkBufferSize() {return KTest2To1SinkBufferSize;};
	};
#endif

