#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
#
# Abstract:
# Produce an SVG diagram showing process/thread creation and destruction from utracedecoder output file.
#
# Parses the ulogger textual output from utracedecoder.exe and generates a "seq" file suitable for
# input into parseseq.pl.  For example:
#
#	perl -S processthreadsummary.pl ulogger.txt > proc.seq
#	perl -S parseseq.pl proc.seq
#	<view log.html in SVG browser>
#

my $creatingProcess = 0;

while (<>) {
	if (!/EThreadIdentification/) {
		next;
	}
	#1,3,7,3203155249,0,0x00000000,EThreadIdentification: Process Create: [DProcess=0x00dc0080] 
	if (/EThreadIdentification:\s*Process Create:\s*\[DProcess=0x(.{8})\]/) {
		#print "Process Create: DProcess=$1\n";
		$creatingProcess = $1;
	}
	#2,3,5,3203155347,0,0x00000000,EThreadIdentification: Process Name: [NThread=0x00dc044c] [DProcess=0x00dc0080] [Name=EKern.exe[100041af]0001] 
	elsif (/EThreadIdentification:\s*Process Name:\s*\[NThread=0x(.{8})\] \[DProcess=0x(.{8})\] \[Name=(.*)\]\s*$/) {
		#print "Process Name: NThread=$1, DProcess=$2, Name=$3\n";
		if ($creatingProcess eq $2) {
			my $name = trimName($3);
			$name = uniquifyName($name);
			$processName{$2} = $name;
			print "oc !", $name, " 0 $2\n";
			print "t !", $name, " $name\n";
			$creatingProcess = "";
		}
	}
	#4646,3,2,3236912810,0,0x00000000,EThreadIdentification: Thread Create: [NThread=0x00e26b04] [DProcess=0x00e24108] [Name=Main] 
	elsif (/EThreadIdentification:\s*Thread Create:\s*\[NThread=0x(.{8})\] \[DProcess=0x(.{8})\] \[Name=(.*)\]\s*$/) {
		#print "Thread Create: NThread=$1, DProcess=$2, Name=$3\n";
		print "ac !", $processName{$2}, " $1 $3\n";
		print "t !", $processName{$2}, " $3\n";
	}
	#812,3,3,3229029799,0,0x00000000,EThreadIdentification: Thread Destroy: [NThread=0x00dee718] [DProcess=0x00dede4c] 
	elsif (/EThreadIdentification: Thread Destroy: \[NThread=0x(.{8})\] \[DProcess=0x(.{8})\]\s*$/) {
		#print "Thread Destroy: NThread=$1, DProcess=$2\n";
		print "ad $1\n";
	}
	#813,3,8,3229030152,0,0x00000000,EThreadIdentification: Process Destroy: [DProcess=0x00dede4c] 
	elsif (/EThreadIdentification: Process Destroy: \[DProcess=0x(.{8})\]\s*$/) {
		#print "Process Destroy: DProcess=$1\n";
		print "od !", $processName{$1}, " $1\n";
		print "t !", $processName{$1}," (destroyed)\n";
	}
}

#
# Remove '.exe" extension as well as the UID decoration in the name (e.g. "...[100039e3]0001")
#

sub trimName($)
{
	my $name = $_[0];
	$name =~ s/\[[0-9a-fA-F]{8}\][0-9a-fA-F]{4}\s*$//;
	$name =~ s/\.exe//;
	$name =~ s/\.EXE//;
	return $name;
}

sub uniquifyName()
{
	my ($name) = @_;
	my $index = 0;
	my $proposedName = $name;
	while (defined $uniqueNames{$proposedName})
		{
		$proposedName = $name . "(" . ++$index . ")";
		}

	$uniqueNames{$proposedName} = 1;
	return $proposedName;
}
