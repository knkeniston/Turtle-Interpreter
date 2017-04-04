#!/usr/bin/env perl

use strict;
use warnings;
use utf8;
use Test::More tests => 9;

my $PROG = "turtle";
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
	(system("./$PROG examples/$_.turtle > $_.commands") == 0) or die "Crashed on $_.turtle: $!\n";
	my $out = `diff $_.commands commands/$_.commands`;
	ok(!$?, "$PROG $_.turtle") or diag($out=~ s/^/    /mrg);
	unlink "$_.commands";
}
