// SkeletonTemplate.h
//
// Copyright (c) 2010 Accenture. All rights reserved.
//

#ifndef SKELETONTEMPLATE_H__
#define SKELETONTEMPLATE_H__

class CSkeletonTemplate : public CActive
	{
public:
	static CSkeletonTemplate* NewLC(CConsoleBase& aConsole);
	static CSkeletonTemplate* NewL(CConsoleBase& aConsole);
	
	~CSkeletonTemplate();
	
	void Start();
	
	// from CActive:
	virtual void RunL();
	virtual void DoCancel();
private:
	CSkeletonTemplate(CConsoleBase& aConsole);
	void ConstructL();
private:
	CConsoleBase& iConsole;
	};


#endif //SKELETONTEMPLATE_H__
