// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MMF_DEVSUBTITLE_POS_H
#define TSU_MMF_DEVSUBTITLE_POS_H

// INCLUDES
#include "tsu_mmf_devsubtitle.h"

// Positive testing

/* 
MM-MMF-DevSubtitle-U-0001-HP 
*/
class RDevSubtitleTestStepPos0001 : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleTestStepPos0001* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0001();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0001(const TDesC& aStepName);
	};
	
/*
MM-MMF-DevSubtitle-U-0002-HP
*/
class RDevSubtitleTestStepPos0002 : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleTestStepPos0002* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0002();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0002(const TDesC& aStepName);
	};

/*
MM-MMF-DevSubtitle-U-0003-HP
*/	
class CDevSubtitleTestStepPos0003Ao : public CDevSubtitleTestStepAoBase
	{
public:
	~CDevSubtitleTestStepPos0003Ao();
	
	static CDevSubtitleTestStepPos0003Ao* NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	
	void StartL();
	
	// From CActive
	void RunL();

private:
	CDevSubtitleTestStepPos0003Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	};

class RDevSubtitleTestStepPos0003 : public RDevSubtitleTestStepAsync
	{
public:
	static RDevSubtitleTestStepPos0003* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0003();
	
    // From RAsyncTestStep
    void KickoffTestL();
	void CloseTest();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0003(const TDesC& aStepName);
	
	CDevSubtitleTestStepPos0003Ao* iTestStep;
	};

	


/*	
MM-MMF-DevSubtitle-U-0004-HP
*/
class CDevSubtitleTestStepPos0004Ao : public CDevSubtitleTestStepAoBase
	{
public:
	~CDevSubtitleTestStepPos0004Ao();
	
	static CDevSubtitleTestStepPos0004Ao* NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	
	void StartL();
	
	// From CActive
	void RunL();

private:
	CDevSubtitleTestStepPos0004Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	};

class RDevSubtitleTestStepPos0004 : public RDevSubtitleTestStepAsync
	{
public:
	static RDevSubtitleTestStepPos0004* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0004();
	
	// From RAsyncTestStep
    void KickoffTestL();
	void CloseTest();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0004(const TDesC& aStepName);
	
	CDevSubtitleTestStepPos0004Ao* iTestStep;
	};
	
/*
MM-MMF-DevSubtitle-U-0005-HP
*/
class RDevSubtitleTestStepPos0005 : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleTestStepPos0005* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0005();
	
	// From RTestStep
	TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0005(const TDesC& aStepName);
	};
	

/*
MM-MMF-DevSubtitle-U-0007-HP
*/
class CDevSubtitleTestStepPos0007Ao : public CDevSubtitleTestStepAoBase
	{
public:
	~CDevSubtitleTestStepPos0007Ao();
	
	static CDevSubtitleTestStepPos0007Ao* NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	
	void StartL();
	
	// From CActive
	void RunL();

private:
	CDevSubtitleTestStepPos0007Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	};
	
	
class RDevSubtitleTestStepPos0007 : public RDevSubtitleTestStepAsync
	{
public:
	static RDevSubtitleTestStepPos0007* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0007();
	    
    // From RAsyncTestStep
    void KickoffTestL();
	void CloseTest();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0007(const TDesC& aStepName);
	
	CDevSubtitleTestStepPos0007Ao* iTestStep;
	};
	
/*
MM-MMF-DevSubtitle-U-0009-HP
*/
class CDevSubtitleTestStepPos0009Ao : public CDevSubtitleTestStepAoBase
	{
public:
	~CDevSubtitleTestStepPos0009Ao();
	
	static CDevSubtitleTestStepPos0009Ao* NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	
	void StartL();
	
	// From CActive
	void RunL();

private:
	CDevSubtitleTestStepPos0009Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	};

class RDevSubtitleTestStepPos0009 : public RDevSubtitleTestStepAsync
	{
public:
	static RDevSubtitleTestStepPos0009* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0009();
	
    // From RAsyncTestStep
    void KickoffTestL();
	void CloseTest();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0009(const TDesC& aStepName);
	
	CDevSubtitleTestStepPos0009Ao* iTestStep;
	};
	
/*
MM-MMF-DevSubtitle-U-0010-HP
*/
class RDevSubtitleTestStepPos0010 : public RDevSubtitleTestStep
	{
public:
	static RDevSubtitleTestStepPos0010* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0010();
	
    // From RAsyncTestStep
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	TVerdict DoTestStepL();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0010(const TDesC& aStepName);
	};

/*
MM-MMF-DevSubtitle-U-0011-HP
*/	
class CDevSubtitleTestStepPos0011Ao : public CDevSubtitleTestStepAoBase
	{
public:
	~CDevSubtitleTestStepPos0011Ao();
	
	static CDevSubtitleTestStepPos0011Ao* NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	
	void StartL();
	
	// From CActive
	void RunL();

private:
	CDevSubtitleTestStepPos0011Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);
	};
	
class RDevSubtitleTestStepPos0011 : public RDevSubtitleTestStepAsync
	{
public:
	static RDevSubtitleTestStepPos0011* NewL(const TDesC& aStepName);
	~RDevSubtitleTestStepPos0011();
	
    // From RAsyncTestStep
    void KickoffTestL();
	void CloseTest();
    
private:
	void ConstructL();
	RDevSubtitleTestStepPos0011(const TDesC& aStepName);
	
	CDevSubtitleTestStepPos0011Ao* iTestStep;
	};

#endif // TSU_MMF_DEVSUBTITLE_POS_H
