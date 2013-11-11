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
// Header file: DevVideoRecord tests.
// 
//

/**
 @file TestDevVideoRecord.h
*/

#ifndef __TESTDEVVIDEORECORD_H__
#define __TESTDEVVIDEORECORD_H__

#include "TSU_MMF_DevVideo.h"

/**
 * DevVideoRecord test base class
 *
 * @class CTestDevVideoRecordStep
 *
 * Req. under test REQ1931.3
 */

class CTestDevVideoRecordStep : public CTestDevVideoStep, public MMMFDevVideoRecordObserver
	{
public:
	CTestDevVideoRecordStep(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoRecordStep();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord) = 0;

	// MMMFDevVideoRecordObserver
    virtual void MdvroReturnPicture(TVideoPicture* aPicture);
    virtual void MdvroSupplementalInfoSent();
    virtual void MdvroNewBuffers();
    virtual void MdvroFatalError(TInt aError);
    virtual void MdvroInitializeComplete(TInt aError);
    virtual void MdvroStreamEnd();

protected:
	TInt iError;
	TTestType iTestType;					// Type of test
	CMMFDevVideoRecord* iDevVideoRecord;
	};

//

class CTestDevVideoRecordFindEncoders : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordFindEncoders* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordFindEncoders(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordFindEncoders() {};
	TInt CheckEncoders(const RArray<TUid>& aFoundEncoders, const RArray<TUid>& aExpectedEncoders, const RArray<TUid>& aUnExpectedEncoders);
	};
//

class CTestDevVideoRecordFindPreProc : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordFindPreProc* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordFindPreProc(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordFindPreProc() {};
	TInt CheckPreProx(const RArray<TUid>& aFoundPreProx, const RArray<TUid>& aExpectedPreProx, const RArray<TUid>& aUnExpectedPreProx);
	};
//

class CTestDevVideoRecordListEncoders : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordListEncoders* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordListEncoders(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordListEncoders() {};
	};
//

class CTestDevVideoRecordListPreProc : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordListPreProc* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordListPreProc(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordListPreProc() {};
	};
//

class CTestDevVideoRecordGetEncoderInfo : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordGetEncoderInfo* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordGetEncoderInfo(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordGetEncoderInfo() {};
	TBool ValidEncoderInfo(CVideoEncoderInfo* aInfo);
	};
//

class CTestDevVideoRecordGetPreProcInfo : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordGetPreProcInfo* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordGetPreProcInfo(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordGetPreProcInfo() {};
	TBool ValidPreProcInfo(CPreProcessorInfo* aInfo);
	};
//

class CTestDevVideoRecordSelectEncoder : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSelectEncoder* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSelectEncoder(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSelectEncoder() {};
	};
//

class CTestDevVideoRecordSelectPreProc : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSelectPreProc* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSelectPreProc(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSelectPreProc() {};
	};

//

class CTestDevVideoRecordSetInputFormat : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetInputFormat* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetInputFormat(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetInputFormat() {};
	};

//

class CTestDevVideoRecordSetSourceCam : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetSourceCam* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetSourceCam(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetSourceCam() {};
	};

//

class CTestDevVideoRecordSetSourceMem : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetSourceMem* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetSourceMem(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetSourceMem() {};
	};

//

class CTestDevVideoRecordSetOutputFormat : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetOutputFormat* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetOutputFormat(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetOutputFormat() {};
	};

//

class CTestDevVideoRecordSetOutputFormatUC : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetOutputFormatUC* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetOutputFormatUC(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetOutputFormatUC() {};
	};

//

class CTestDevVideoRecordSetOutputRect : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetOutputRect* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetOutputRect(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetOutputRect() {};
	};

//

class CTestDevVideoRecordSetClockSource : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetClockSource* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetClockSource(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetClockSource() {};
	};

//

class CTestDevVideoRecordSetPreProcTypes : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetPreProcTypes* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetPreProcTypes(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetPreProcTypes() {};
	};

//

class CTestDevVideoRecordSetRgbToYuvOptions : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetRgbToYuvOptions* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetRgbToYuvOptions(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetRgbToYuvOptions() {};
	};

//

class CTestDevVideoRecordSetYuvToYuvOptions : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetYuvToYuvOptions* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetYuvToYuvOptions(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetYuvToYuvOptions() {};
	};

//

class CTestDevVideoRecordSetRotate : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetRotate* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetRotate(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetRotate() {};
	};

//

class CTestDevVideoRecordSetScale : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetScale* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetScale(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetScale() {};
	};

//

class CTestDevVideoRecordSetInputCrop : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetInputCrop* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetInputCrop(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetInputCrop() {};
	};

//

class CTestDevVideoRecordSetOutputCrop : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetOutputCrop* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetOutputCrop(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetOutputCrop() {};
	};

//

class CTestDevVideoRecordSetOutputPad : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetOutputPad* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetOutputPad(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetOutputPad() {};
	};

//

class CTestDevVideoRecordSetColorEnhance : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetColorEnhance* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetColorEnhance(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetColorEnhance() {};
	};

//

class CTestDevVideoRecordSetFrameStabilisation : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetFrameStabilisation* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetFrameStabilisation(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetFrameStabilisation() {};
	};

//

class CTestDevVideoRecordSetCustomISPreProc : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetCustomISPreProc* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetCustomISPreProc(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetCustomISPreProc() {};
	};

//

class CTestDevVideoRecordSetErrors : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetErrors* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetErrors(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetErrors() {};
	};

//

class CTestDevVideoRecordSetMinRandomAccess : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetMinRandomAccess* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetMinRandomAccess(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetMinRandomAccess() {};
	};

//

class CTestDevVideoRecordSetNumBitrateLayers : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetNumBitrateLayers* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetNumBitrateLayers(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetNumBitrateLayers() {};
	};

//

class CTestDevVideoRecordSetScalabilityType : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetScalabilityType* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetScalabilityType(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetScalabilityType() {};
	};

//

class CTestDevVideoRecordSetGlobalReferencePic : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetGlobalReferencePic* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetGlobalReferencePic(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetGlobalReferencePic() {};
	};

//

class CTestDevVideoRecordSetLayerReference : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetLayerReference* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetLayerReference(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetLayerReference() {};
	};

//

class CTestDevVideoRecordSetBufferOptions : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetBufferOptions* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetBufferOptions(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetBufferOptions() {};
	};

//

class CTestDevVideoRecordSetCSEncoderOptions : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetCSEncoderOptions* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetCSEncoderOptions(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetCSEncoderOptions() {};
	};

//

class CTestDevVideoRecordSetISEncoderOptions : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetISEncoderOptions* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordSetISEncoderOptions(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetISEncoderOptions() {};
	};

//

class CTestDevVideoRecordInit : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordInit* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordInit(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordInit() {};
	};

//

class CTestDevVideoRecordGetCSInitOutput : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordGetCSInitOutput* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordGetCSInitOutput(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordGetCSInitOutput() {};
	};

//

class CTestDevVideoRecordGetISInitOutput : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordGetISInitOutput* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordGetISInitOutput(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordGetISInitOutput() {};
	};

//

class CTestDevVideoRecordSetErrorProtectionLevels : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetErrorProtectionLevels* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetErrorProtectionLevels(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetErrorProtectionLevels() {};
	};

//

class CTestDevVideoRecordSetupErrorProtection : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetupErrorProtection* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetupErrorProtection(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetupErrorProtection() {};
	};

//

class CTestDevVideoRecordSetChannelBufferLossRate : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetChannelBufferLossRate* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetChannelBufferLossRate(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetChannelBufferLossRate() {};
	};

//

class CTestDevVideoRecordSetChannelBitErrorRate : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetChannelBitErrorRate* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetChannelBitErrorRate(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetChannelBitErrorRate() {};
	};

//

class CTestDevVideoRecordSetSegTargetSize : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetSegTargetSize* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetSegTargetSize(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetSegTargetSize() {};
	};

//

class CTestDevVideoRecordSetRateControl : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetRateControl* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetRateControl(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetRateControl() {};
	};

//

class CTestDevVideoRecordSetInLayerScalability : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetInLayerScalability* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetInLayerScalability(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetInLayerScalability() {};
	};

//

class CTestDevVideoRecordSetLayerPromPointPeriod : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetLayerPromPointPeriod* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetLayerPromPointPeriod(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetLayerPromPointPeriod() {};
	};

//

class CTestDevVideoRecordCSSettingsOutput : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordCSSettingsOutput* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordCSSettingsOutput(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordCSSettingsOutput() {};
	};

//

class CTestDevVideoRecordISSettingsOutput : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordISSettingsOutput* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordISSettingsOutput(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordISSettingsOutput() {};
	};

//

class CTestDevVideoRecordWritePicture : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordWritePicture* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
	virtual void MdvroReturnPicture(TVideoPicture* aPicture);
private:
	CTestDevVideoRecordWritePicture(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordWritePicture() {};
	TVideoPicture *iPicture;
	};

//

class CTestDevVideoRecordSendSupplementalInfo : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSendSupplementalInfo* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
	virtual void MdvroSupplementalInfoSent();
private:
	CTestDevVideoRecordSendSupplementalInfo(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSendSupplementalInfo() {};
	TBool iSuppInfoSent;
	};

//

class CTestDevVideoRecordSendSupplementalInfoTime : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSendSupplementalInfoTime* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
	virtual void MdvroSupplementalInfoSent();
private:
	CTestDevVideoRecordSendSupplementalInfoTime(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSendSupplementalInfoTime() {};
	TBool iSuppInfoSent;
	};

//

class CTestDevVideoRecordCancelSupplementalInfo : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordCancelSupplementalInfo* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
	virtual void MdvroSupplementalInfoSent();
private:
	CTestDevVideoRecordCancelSupplementalInfo(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordCancelSupplementalInfo() {};
	TInt iSuppInfoCount;
	};

//

class CTestDevVideoRecordInputEnd : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordInputEnd* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
	virtual void MdvroStreamEnd();
private:
	CTestDevVideoRecordInputEnd(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordInputEnd() {};
	TUint iEndCount;
	};

//

class CTestDevVideoRecordStart : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordStart* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordStart(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordStart() {};
	};

//

class CTestDevVideoRecordStop : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordStop* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordStop(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordStop() {};
	};

//

class CTestDevVideoRecordPause : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordPause* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordPause(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordPause() {};
	};

//

class CTestDevVideoRecordResume : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordResume* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordResume(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordResume() {};
	};

//

class CTestDevVideoRecordFreezePicture : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordFreezePicture* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordFreezePicture(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordFreezePicture() {};
	};

//

class CTestDevVideoRecordReleaseFreeze : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordReleaseFreeze* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordReleaseFreeze(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordReleaseFreeze() {};
	};

//

class CTestDevVideoRecordRecordingPosition : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordRecordingPosition* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordRecordingPosition(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordRecordingPosition() {};
	};

//

class CTestDevVideoRecordGetOutputBufferStatus : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordGetOutputBufferStatus* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordGetOutputBufferStatus(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordGetOutputBufferStatus() {};
	};

//

class CTestDevVideoRecordGetPictureCounters : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordGetPictureCounters* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordGetPictureCounters(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordGetPictureCounters() {};
	};

//

class CTestDevVideoRecordGetFrameStabilisation : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordGetFrameStabilisation* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordGetFrameStabilisation(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordGetFrameStabilisation() {};
	};

//

class CTestDevVideoRecordNumDataBuffers : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordNumDataBuffers* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordNumDataBuffers(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordNumDataBuffers() {};
	};

//

class CTestDevVideoRecordNextBuffer : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordNextBuffer* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
	virtual void MdvroNewBuffers();
	void CheckNewBufferInfoL(TUint aNumBuffers);
	void DoNewBuffersL();
private:
	CTestDevVideoRecordNextBuffer(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordNextBuffer() {};
private:
	TInt iCurrentBufferNumber;
	CMMFDevVideoRecord* iDevVideoRecord;
	};

//

class CTestDevVideoRecordReturnBuffer : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordReturnBuffer* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordReturnBuffer(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordReturnBuffer() {};
	};

//

class CTestDevVideoRecordPictureLoss : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordPictureLoss* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordPictureLoss(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordPictureLoss() {};
	};

//

class CTestDevVideoRecordPictureLossSpec : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordPictureLossSpec* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordPictureLossSpec(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordPictureLossSpec() {};
	};

//

class CTestDevVideoRecordSliceLoss : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSliceLoss* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSliceLoss(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSliceLoss() {};
	};

//

class CTestDevVideoRecordSendRefPictureSelection : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSendRefPictureSelection* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSendRefPictureSelection(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSendRefPictureSelection() {};
	};

//

class CTestDevVideoRecordNumComplexityLevels : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordNumComplexityLevels* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordNumComplexityLevels(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordNumComplexityLevels() {};
	};

//

class CTestDevVideoRecordSetComplexityLevel : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordSetComplexityLevel* NewL(const TDesC& aTestName, const TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvroInitializeComplete(TInt aError);
private:
	CTestDevVideoRecordSetComplexityLevel(const TDesC& aTestName, const TTestType aTestType);
	~CTestDevVideoRecordSetComplexityLevel() {};
	};

//

class CTestDevVideoRecordCommit : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordCommit* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoRecordCommit(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoRecordCommit() {};
	};

//

class CTestDevVideoRecordRevert : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordRevert* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoRecordRevert(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoRecordRevert() {};
	};


//

class CTestDevVideoRecordCustomInterface : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordCustomInterface* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordCustomInterface(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoRecordCustomInterface() {};
	};


//
/*
class CTestDevVideoRecordXXX : public CTestDevVideoRecordStep
	{
public:
	static CTestDevVideoRecordXXX* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoRecord& aDevVideoRecord);
private:
	CTestDevVideoRecordXXX(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoRecordXXX() {};
	};
*/

#endif	// __TESTDEVVIDEORECORD_H__
