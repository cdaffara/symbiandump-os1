// SkeletonTemplate.cpp
//
// [[COPYRIGHT]]
//

#include <fshell/ioutils.h>
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdSkeletonTemplate : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSkeletonTemplate();
private:
	CCmdSkeletonTemplate();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	//TODO: arguments/options
	};

EXE_BOILER_PLATE(CCmdSkeletonTemplate)

CCommandBase* CCmdSkeletonTemplate::NewLC()
	{
	CCmdSkeletonTemplate* self = new(ELeave) CCmdSkeletonTemplate();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSkeletonTemplate::~CCmdSkeletonTemplate()
	{
	}

CCmdSkeletonTemplate::CCmdSkeletonTemplate()
	{
	}

const TDesC& CCmdSkeletonTemplate::Name() const
	{
	_LIT(KName, "skeletontemplate");	
	return KName;
	}

void CCmdSkeletonTemplate::ArgumentsL(RCommandArgumentList& aArguments)
	{
	//TODO: aArguments.AppendStringL(iString, _L("example_arg"));
	//TODO: Also remember to update the CIF file for any arguments you add.
	}

void CCmdSkeletonTemplate::OptionsL(RCommandOptionList& aOptions)
	{
	//TODO: aOptions.AppendBoolL(iOpt, _L("example_opt"));
	//TODO: Also remember to update the CIF file for any options you add.
	}

void CCmdSkeletonTemplate::DoRunL()
	{
	// TODO: Add implementation.
	}
