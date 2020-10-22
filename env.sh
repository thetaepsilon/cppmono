_root="$(git rev-parse --show-toplevel)" && {
	echo "# $_root";
	export cxxp_root="$_root";
	export cxxp_wrapper="cxx_pkg";
}
unset _root;
export cxx_path=cxxp;
