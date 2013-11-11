// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the Class ElfConsumer for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "pl_elfconsumer.h"
#include "parameterlistinterface.h"
#include "errorhandler.h"
#include <iostream>
#include <string>

using std::list;
using std::cout;
using std::endl;
using std::min;

/**
Constructor for class ElfConsumer
@param aParameterListInterface - instance of class ParameterListInterface
@internalComponent
@released
*/
ElfConsumer::ElfConsumer(ParameterListInterface *aParameterListInterface) :\
 ElfExecutable(aParameterListInterface) ,\
 iMemBlock(NULL)
{
}


/**
Destructor for class ElfConsumer
@internalComponent
@released
*/
ElfConsumer::~ElfConsumer(){

	DELETE_PTR_ARRAY(iMemBlock);
}


/**
This operation takes the member of the ElfFileWriter object that is to be populated with the
export info in the iExports member (from the iInputElfFile member of ElfConsumer).
@param aFile - Elf file name
@internalComponent
@released
*/
PLUINT32 ElfConsumer::ReadElfFile(char* aFile){
	FILE*	aFd;

	if( (aFd = fopen(aFile,"rb")) == NULL) {
		throw FileError(FILEOPENERROR, aFile);
	}

	fseek(aFd, 0, SEEK_END);

	PLUINT32 aSz = ftell(aFd);
	iMemBlock = new char[aSz];

	fseek(aFd, 0, SEEK_SET);

	// Certain Windows devices (e.g., network shares) limit the size of I/O operations to 64MB
	// or less.  We read all the data in individual KMaxWindowsIOSize (32MB) chunks to be safe.
	PLUINT32 chunkSize = 0;
	for( PLUINT32 bytesRead = 0; bytesRead < aSz; bytesRead += chunkSize) {
		
		chunkSize = min(aSz - bytesRead, PLUINT32(KMaxWindowsIOSize));
		
		if( fread(iMemBlock + bytesRead, chunkSize, 1, aFd) != 1) {
			throw FileError(FILEREADERROR, aFile);
		}		
	}
	
	return 0;
}


/**
Funtion for getting elf symbol list
@param aList - list of symbols found in elf files
@return - 0 for no exports in elf files, otherwise number of symbols found
@internalComponent
@released
*/
int ElfConsumer::GetElfSymbolList(list<Symbol*>& aList){

	if( !iExports ) 
		return 0;

	//Get the exported symbols
	vector<DllSymbol*> aTmpList = iExports->GetExports(true);

	typedef vector<DllSymbol*> List;
	List::iterator aItr = aTmpList.begin();
	while( aItr != aTmpList.end() ){
		aList.push_back((Symbol*) (*aItr));
		aItr++;
	}
	aTmpList.clear();
	return aList.size();
}

/**
Funtion for getting image details
@internalComponent
@released
*/
void ElfConsumer::GetImageDetails(/*E32ImageInterface aInterface*/){

}


/**
Funtion for processing elf file
@internalComponent
@released
*/
PLUINT32 ElfConsumer::ProcessElfFile(){

	Elf32_Ehdr *aElfHdr = ELF_ENTRY_PTR(Elf32_Ehdr, iMemBlock, 0);

	try
	{
		ElfExecutable::ProcessElfFile(aElfHdr);
		
		/* The following is a workaround for the ARM linker problem.
		 * Linker Problem: ARM linker generates Long ARM to Thumb veneers for which
		 * relocation entries are not generated.
		 * The linker problem is resolved in ARM Linker version RVCT 2.2 Build 616.
		 * Hence the workaround is applicable only for executables generated
		 * by ARM linker 2.2 and if build number is below 616.
		 */
		char * aARMCompiler = "ARM Linker, RVCT";
		int length = strlen(aARMCompiler);
		char * aCommentSection = ElfExecutable::FindCommentSection();
   		/* The .comment section in an elf file contains the compiler version information and 
   		 * it is used to apply the fore mentioned workaround. 
   		 * Some build tool chains generating elf file output without the .comment section, 
   		 * just to save the disk space. In this case the variable aCommentSection gets the NULL value.
   		 * Solution: This workaround is only applicable for RVCT compiler. So if the .comment section 
   		 * is not available in the elf file, then this workaround is no need to be applied.
   		 */
   		if(aCommentSection != NULL)
   		{
		if (!strncmp(aCommentSection, aARMCompiler, length))
		{
			int WorkAroundBuildNo = 616;
			int BuildNo = 0;
			char* RVCTVersion = aCommentSection+length;

			/* RVCTVersion contains the following string
			 * "<MajorVersion>.<MinorVersion> [Build <BuildNumber>]"
			 * Example: "2.2 [Build 616]"
			 */
			String Version(RVCTVersion);
			size_t pos = Version.find_last_of(' ');
			size_t size = Version.size();
			if (pos < size)
			{
				size_t index = pos + 1;
				if (index < size)
				{
					BuildNo = atoi(strtok(RVCTVersion+index, "]"));
				}
			}

			/* Workaround is applicable only when the version is 2.2 and if the
			 * build number is below 616.
			 */
			size_t minorVersionPos = Version.find_first_of('.');
			char RVCTMinorVersion='0';
			if (minorVersionPos < size)
			{
				size_t index = minorVersionPos + 1;
				if (index < size)
				{
					RVCTMinorVersion = *(RVCTVersion + index);
				}
			}

			if ((*RVCTVersion == '2') && (RVCTMinorVersion == '2') &&
				(BuildNo < WorkAroundBuildNo))
			{ 
				/* The static symbol table should be processed to identify the veneer symbols.
				 * Relocation entries should be generated for these symbols if the linker 
				 * is not generating the same.
				 */
				ElfExecutable::FindStaticSymbolTable();
				ElfExecutable::ProcessVeneers();
			}
		}
		}
	}
	catch(ErrorHandler&) 
	{
		throw;
	} 
	/*catch(...) // If there are any other unhandled exception,they are handled here.
	{
		//Warning to indicate that there had been an exception at this point.
		MessageHandler::GetInstance()->ReportWarning(ELFFILEERROR,(char*)iParameterListInterface->ElfInput());
		throw;
	} */
	return 0;
}



