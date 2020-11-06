#!/bin/sh
set -eu;
testdir="$1";

tmpdir="";
cleanup() {
	test -z "$tmpdir" || rm -rf "$tmpdir";
};
trap cleanup EXIT;

# mktemp(1) isn't in posix? waaaaaaaaaaat
# all jokes aside, we literally can't do anything if this doesn't exist,
# as we _need_ this to hand us what a suitable writeable temp dir is.
# it would be erroneous to assume anything else here.
tmpdir="$(mktemp -d)";
#echo "$tmpdir";
outdir="$tmpdir/output";
mkdir "$outdir";
empty="$tmpdir/empty";
mkdir "$empty";



log() {
	echo "#" "$@" >&2;
};

runtest() {
	_output="$outdir/$1";
	mkdir "$_output";
	_test="$testdir/$1";
	./compiletests_listgen.lua "$_output" < "$_test/input.txt" || {
		log "script under test died with code $?.";
		return 1;
	};

	# some test cases should result in genuine empty outputs,
	# however a git limitation means such empty directories would not be preserved.
	# furthermore, one would have to remember to create dotfiles to fix this for all of them,
	# which could turn out to be highly error prone.
	# instead, if the directory does not exist and an explicit marker is present,
	# use a known prepared empty directory, namely the one we created in $tmpdir above.
	# the marker is above the expected data files directory so it isn't mixed up as one itself.
	_expected="$_test/expected";
	if test -e "$_test/nothing_expected"; then {
		# avoid a silent confusion if the directory actually does exist.
		test ! -e "$_expected" || {
			log "test had both nothing_expected and expected, make your mind up!";
			return 1;
		};
		# otherwise re-point _expected to the prepped empty dir.
		_expected="$empty";
	}; else {
		test -e "$_expected" || {
			log "test was missing expected directory and nothing_expected was not set.";
			return 1;
		};
	}; fi;

	./selftest_compare.sh "$_expected" "$_output" || {
		log "comparison failed, code $?.";
		return 1;
	};
};



# ugh, set -o pipefail isn't portable,
# and nothing in posix can ensure (failing_program) | other_thing is automatically fatal.
# can't have silent directory listing errors now can we...
tests="$tmpdir/tests.txt";

(
	cd "$testdir";
	ls -1 > "$tests";
);

(
	IFS='
';
	while read -r dirname; do {
		echo "$dirname";
		runtest "$dirname" || {
			log "test case $dirname failed.";
			exit 1;
		};
	}; done;
) < "$tests";

