// SkeletonTemplate.cpp
//
// [[COPYRIGHT]]
//

#include <e32base.h>
#include <e32cons.h>
#include "SkeletonTemplate.h"

void MainL()
	{
	CConsoleBase* cons = Console::NewL(_L(" SkeletonTemplate "), TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(cons);
	
	CActiveScheduler* as = new(ELeave)CActiveScheduler();
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);
	
	CSkeletonTemplate* echo = CSkeletonTemplate::NewLC(*cons);
	
	cons->Printf(_L("Hello World!\n"));
	cons->Printf(_L("Press escape to exit\n"));
	
	echo->Start();
	
	CActiveScheduler::Start();
	
	
	CleanupStack::PopAndDestroy(3); // cons, as, echo
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TInt err = KErrNone;
	
	if (cleanup)
		{
		TRAP(err, MainL() );
		delete cleanup; // destroy clean-up stack
		}
	else
		{
		err = KErrNoMemory;
		}
	
	__UHEAP_MARKEND;
	return err;
	}
	
//______________________________________________________________________________
//						CSkeletonTemplate
CSkeletonTemplate* CSkeletonTemplate::NewLC(CConsoleBase& aConsole)
	{
	CSkeletonTemplate* self = new(ELeave)CSkeletonTemplate(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSkeletonTemplate* CSkeletonTemplate::NewL(CConsoleBase& aConsole)
	{
	CSkeletonTemplate* self = NewLC(aConsole);
	CleanupStack::Pop(self);
	return self;
	}

CSkeletonTemplate::~CSkeletonTemplate()
	{
	Cancel();
	}
	
void CSkeletonTemplate::Start()
	{
	if (!IsActive())
		{
		iConsole.Read(iStatus);
		SetActive();
		}
	}

void CSkeletonTemplate::RunL()
	{
	TKeyCode key = iConsole.KeyCode();
	if (key == EKeyEscape)
		{
		CActiveScheduler::Stop();
		return;
		}
	else if (key == EKeyEnter)
		{
		iConsole.Printf(_L("\n"));
		}
	else
		{
		TBuf<1> buf;
		buf.Append(key);
		iConsole.Write(buf);
		}
	// reissue read request
	Start();
	}

void CSkeletonTemplate::DoCancel()
	{
	iConsole.ReadCancel();
	}

CSkeletonTemplate::CSkeletonTemplate(CConsoleBase& aConsole)
	: CActive(CActive::EPriorityStandard), iConsole(aConsole)
	{
	}

void CSkeletonTemplate::ConstructL()
	{
	CActiveScheduler::Add(this);
	}
