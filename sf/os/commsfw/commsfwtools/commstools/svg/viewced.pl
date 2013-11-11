# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

my $version = "1.0 (26/4/06)";

#
# Global variables
#

%interestingTables = (
	AccessPointTable	=> 1,
	ProtocolTable		=> 1,
	SCprTable			=> 1,
	CprTable			=> 1,
	MCprTable			=> 1,
	TierTable			=> 1
	);

$tableName = "";
$tableId = 0;
$tableOfInterest = 0;

$topMargin = 100;
$leftMargin = 100;
$verticalSpacing = 100;
$fontSize = 15;
$fontHeight = $fontSize;
$fontVerticalSpacing = $fontHeight + 5;

$rectWidth = $fontSize * 10 ;
$rectFontHeightRatio = 3;
$rectHeight = $fontHeight * $rectFontHeightRatio;
$rectHorizSpacing = 40;
$rectVertSpacing = 40;
$horizontalSpacing = $rectWidth + $rectHorizSpacing;
$verticalSpacing = $rectHeight + $rectVertSpacing;

while (<>) {
	if (/^\[/)
		{
		processTable($_);
		}
	elsif (/ADD_SECTION/)
		{
		processAddSection();
		}
	elsif (/END_ADD/)
		{
		processEndAdd();
		}
	elsif ( /FIELD_COUNT\s*=\s*/ )			# ensure this check is above the field check below
		{
		processFieldCount();
		}
	elsif ( /ADD_TEMPLATE/ )
		{
		processAddTemplate();
		}
	elsif ( /\s*(\w+)=\s*(.*)$/ )
		{
		processField($1, $2);
		}
}

#printDatabase();
outputObjects();

#
# Subroutines
#

sub processTable($)
	{
	($tableName) = $_;
	$tableName =~ s/^\[(\w+)\]$/$1/;
	chomp $tableName;
	$tableId = 0;
	if ($interestingTables{$tableName} == 1)
		{
		$tableOfInterest = 1;
		}
	else
		{
		$tableOfInterest = 0;
		}
	}
	
sub processAddSection()
	{
	++$tableId;
	}

sub processEndAdd()
	{
	}

sub processAddTemplate()
	{
	++$tableId;
	}

sub processFieldCount()
	{
	}
	
sub processField($$)
	{
	my $fieldName = $_[0];
	my $value = $_[1];
#	print "\t\tField $tableName, $tableId, $fieldName = $value\n";
	if ($tableOfInterest == 1)
		{
		$database{$tableName}[$tableId]{$fieldName} = $value;
		}
	}

sub outputObjects()
	{
	openSVGFile();
	outputDocHeader();
	my $apCount = scalar( @{$database{AccessPointTable}} );
	my $ap;
	my $currentY = $topMargin;
	for ($ap = 1 ; $ap < $apCount ; ++$ap)
		{
		outputAP($database{AccessPointTable}[$ap], $currentY);
		$currentY += $verticalSpacing;
		}
	outputDocFooter();
	closeSVGFile();
	}

sub outputAP($$)
	{
	my $ap = $_[0];
	my $y = $_[1];
	my $x = $leftMargin;
	outputTextRect($x, $y, $database{ProtocolTable}[$ap->{Protocol}]{Name}, UidString($database{ProtocolTable}[$ap->{Protocol}]{ProtocolUid}));
	$x += $horizontalSpacing;
	outputTextRect($x, $y, $database{SCprTable}[$ap->{SCpr}]{Name}, UidString($database{SCprTable}[$ap->{SCpr}]{SCprUid}));
	$x += $horizontalSpacing;
	outputTextRect($x, $y, $database{CprTable}[$ap->{Cpr}]{Name}, UidString($database{CprTable}[$ap->{Cpr}]{CprUid}));
	$x += $horizontalSpacing;
	outputTextRect($x, $y, $database{MCprTable}[$ap->{MCpr}]{Name}, UidString($database{MCprTable}[$ap->{MCpr}]{MCprUid}));
	$x += $horizontalSpacing;
	outputTextRect($x, $y, $database{TierTable}[$ap->{Tier}]{TierManagerName}, UidString($database{TierTable}[$ap->{Tier}]{TierManagerUid}));
	$x += $horizontalSpacing;
	
#	print "AP $ap->{Name}:\n";
#	print "\tProtocol $ap->{Protocol} ($database{ProtocolTable}[$ap->{Protocol}]{Name})\n";
#	print "\tSCpr $ap->{SCpr} ($database{SCprTable}[$ap->{SCpr}]{Name})\n";
#	print "\tCpr $ap->{Cpr} ($database{CprTable}[$ap->{Cpr}]{Name})\n";
#	print "\tMCpr $ap->{MCpr} ($database{MCprTable}[$ap->{MCpr}]{Name})\n";
#	print "\tTier $ap->{Tier} ($database{TierTable}[$ap->{Tier}]{TierManagerName}, $database{TierTable}[$ap->{Tier}]{TierManagerUid})\n";
	}

sub UidString($)
	{
	my $num = $_[0];
	return sprintf("%08x", $num);
	}
		
sub printDatabase()
	{
	foreach $table ( keys %database )
		{
		print "$table:\n";
		# entry zero is not initialised (commsdat id entries begin at 1)
		for ($record = 1 ; $record < scalar(@{$database{$table}}) ; ++$record)
			{
			print "\t$record:\n";
			foreach $field ( keys %{$database{$table}[$record]} )
				{
				print "\t\t$field = $database{$table}[$record]{$field}\n";
				}
			}
		}
	}

sub capitaliseText($)
	{
	my $text = $_[0];
	my $doCpr = 1;
	if ($text =~ s/scpr/Scpr/)
		{
		$doCpr = 0;
		};
	if ($text =~ s/mcpr/Mcpr/)
		{
		$doCpr = 0;
		};
	$text =~ s/shim/Shim/;
	if ($doCpr == 1)
		{
		$text =~ s/cpr/Cpr/;
		}
	return $text;
	}

#######################
# SVG output routines
#######################

sub openSVGFile()
	{
	open SVG, ">ced.svg" || die "Cannot open ced.svg for writing\n";
	}

sub closeSVGFile()
	{
	close SVG;
	}

sub outputDocHeader()
	{
	my ($width,$height) = @_;
	print SVG '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',"\n";
	print SVG '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">',"\n";
	print SVG "<svg height=\"$height\" width=\"$width\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
	}

sub outputDocFooter()
	{
	print SVG "</svg>\n";
	}

sub outputText($$$$)
	{
	my ($x,$text,$y,$anchor,$decoration,$colour) = @_;
	
	my $attrs = qq{ x="$x" y="$y" font-size="$fontSize"};
	
	if ($decoration)
		{
		$attrs .= qq{ text-decoration="$decoration" };
		}
	
	if ($anchor)
		{
		$attrs .= qq { text-anchor="$anchor" };
		}
	
	if ($colour)
		{
		if ($colour =~ s/^!//)
			{
			outputRect($x - 1, $y - $fontHeight, 3, $fontHeight);
			}
		if ($colour)
			{
			$attrs .= qq { fill="$colour" };
			}
		}
		
	print SVG "<text $attrs>$text</text>\n";
	}

sub outputRect($$$$)
	{
	my ($x,$y,$width,$height) = @_;
	print SVG qq {<rect fill="white" stroke="black" x="$x" y="$y" width="$width" height="$height" />\n};
	}

sub	outputTextRect($$$$)
	{
	my $x = $_[0];
	my $y = $_[1];
	my $text1 = $_[2];
	my $text2 = $_[3];
	outputRect($x,$y,$rectWidth, $rectHeight);
	$y += $rectHeight/$rectFontHeightRatio + $fontHeight/2;
	outputText($x + $rectWidth/2, capitaliseText($text1), $y, "middle");	
	if ($text2)
		{
		$y += $fontVerticalSpacing;
		outputText($x + $rectWidth/2, $text2, $y, "middle");	
		}
	}
