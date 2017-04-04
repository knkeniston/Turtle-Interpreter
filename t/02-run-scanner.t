#!/usr/bin/env perl

use strict;
use warnings;
use utf8;
use Test::More tests => 9;

my $PROG = "scannertest";
ok(-e "$PROG", "$PROG exists") or die "No $PROG; bailing\n";

my @IN = (
	"funky",
	"gasket",
	"if",
	"line",
	"polygon",
	"ring",
	"spiral",
	"star"
);

for (@IN) {
	(system("./$PROG examples/$_.turtle > $_.tokens") == 0) or die "Crashed on $_.turtle: $!\n";
	my $out = `diff $_.tokens tokens/$_.tokens`;
	ok(!$?, "$PROG $_.turtle") or diag($out=~ s/^/    /mrg);
	unlink "$_.tokens";
}
