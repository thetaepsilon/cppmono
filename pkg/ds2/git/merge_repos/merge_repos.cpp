extern "C" {
// access to errno, duh
#include <errno.h>
// posix_spawn
#include <spawn.h>
// waitpid
#include <sys/wait.h>
// mkdir
#include <sys/stat.h>
// (f)printf, fputs
#include <stdio.h>
// chdir()
#include <unistd.h>
}
// std::is_base_of
//#include <type_traits>



namespace ds2::posix {
// fix some posix/C quirkiness here:
// a couple of posix_spawn's parameters are of type array of pointers to *mutable* strings,
// yet it is not reasonably possible for posix_spawn to modify these arguments in it's contracted behaviour
// (particularly with respect to it's thread safety promises).
// this appears to be a C nicety where C-literals are mutable,
// so work around that here even if it is blatantly abusing constness rules.
inline int posix_spawn_c(
	pid_t *pid,
	const char *path,
	const posix_spawn_file_actions_t *file_actions,
	const posix_spawnattr_t *attrp,
	const char *const argv[],
	const char *const envp[])
{
	return posix_spawn(
		pid, path, file_actions, attrp,
		(char**)argv, (char**)envp
	);
}
inline int posix_spawnp_c(
	pid_t *pid,
	const char *name,
	const posix_spawn_file_actions_t *file_actions,
	const posix_spawnattr_t *attrp,
	const char *const argv[],
	const char *const envp[])
{
	return posix_spawnp(
		pid, name, file_actions, attrp,
		(char**)argv, (char**)envp
	);
}



// some error throwing wrappers.
struct SystemException {
	int sys;
	constexpr inline SystemException(int s): sys(s) {}
};
template <typename T>
void throwErrno() {
	throw T(errno);
}
// I wish C++ let you use ! to indicate macros like rust.
// it's far prettier than UPPER_CAPS (which I refuse to use),
// but still indicates what you're doing is a macro and therefore one should pay attention.
// anyway, define two exception classes ultimately carrying SystemException,
// a generic one (for catching in error description handlers)
// and a stronger typed one (for type info stuffz).
#define declare_errno_exception(X) \
	struct TypeErased ## X : public SystemException { using SystemException::SystemException; }; \
	template <typename T> struct X : public TypeErased ## X { using TypeErased ## X::TypeErased ## X; };



// then we do some more wrapping for convenience's sake here:
// auto-wait and copy the program name out of argv.
// auto-throws exceptions in the event of failure at any step.
declare_errno_exception(SpawnException);
struct SpawnWaitException {
	int sys;
	pid_t pid;
};
struct ProcessDiedException {
	pid_t pid;
	int wait_status;
};

template <typename T>
void spawn_wait_throw(
	const char* const* argv,
	const char* const* envp)
{
	pid_t pid;
	int status;

	int ret = posix_spawnp_c(&pid, argv[0], nullptr, nullptr, argv, envp);
	if (ret != 0) throw SpawnException<T> { errno };

	ret = waitpid(pid, &status, 0);
	if (ret < 0) throw SpawnWaitException { errno, pid };

	if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0))
		throw ProcessDiedException { pid, status };
}






declare_errno_exception(ChdirException);
template <typename T> void cd(const char* path) {
	if (chdir(path) != 0) throwErrno<ChdirException<T>>();
}

declare_errno_exception(MkdirException);
template <typename T> void mkdir(const char* path, mode_t mode) {
	if (::mkdir(path, mode) != 0) throwErrno<MkdirException<T>>();
}
// mode-less override of mkdir that uses 0777, so as to simply respect the user's umask for the most part.
template <typename T> inline void mkdir(const char* path) {
	return mkdir<T>(path, 0777);
}

// something based on my shell alias of the same name:
// create and change directory.
template <typename T> void ccd(const char* path) {
	mkdir<T>(path);
	cd<T>(path);
}



} // end namespace ds2::posix. rip indentation






using namespace ds2::posix;	// e.g. use wrapped mkdir instead of raw one below

// this needs to go in a header, maybe in the above posix-y namespace
using strv_ref = const char* const*;

// now for some git stuff
struct GitInit {};
struct GitConfig {};
struct GitEmptyCommit {};
// I'm starting to like macros too much.
#define allocate_argv(name, ...) const char* const name[] = { __VA_ARGS__, nullptr }
#define declare_static_argv(name, ...) static allocate_argv(name, __VA_ARGS__)
void gitInit(strv_ref envp) {
	declare_static_argv(cmd, "git", "init");
	return spawn_wait_throw<GitInit>(cmd, envp);
}
void gitConfig(strv_ref envp, const char* key, const char* value) {
	allocate_argv(cmd, "git", "config", key, value);
	return spawn_wait_throw<GitConfig>(cmd, envp);
}
void gitEmptyCommit(strv_ref envp, const char* message) {
	declare_static_argv(cmd, "git", "commit", "--allow-empty", "-m", message);
	return spawn_wait_throw<GitEmptyCommit>(cmd, envp);
}

// helper type used in run() below - just bundles the envp around for convenience.
struct Git {
	strv_ref envp;

	inline void init() {
		return gitInit(envp);
	}
	inline void config(const char* key, const char* value) {
		return gitConfig(envp, key, value);
	}
	inline void empty_commit(const char* message) {
		return gitEmptyCommit(envp, message);
	}
};






int check_usage(int argc) {
	if (argc < 3) {
		fputs("# usage: merge_repos <target dir> <name>\n", stderr);
		fputs("# name will be an empty subdirectory to create as a fresh git repository.\n", stderr);
		return 1;
	}
	return 0;
}






// maximal supported lengths for various strings related to an input repository.
// needed to perform string concatenation directly into a (length checked!) target buffer.
// having to dynamically allocate and possibly fail is just a bit stupid, it's entirely avoidable.


// structure representing an individual input repository.
// for now the various strings are assumed immortal and allocated elsewhere,
// e.g. as const strings during development or a static sized file read buffer.
struct input_repo {
};






// dummy type hacks to exploit glibc's typename based crash handler.
struct BaseDir {};
struct RepoDir {};
template <typename Git>
void run(Git& git, const char* basedir, const char* reponame) {
	cd<BaseDir>(basedir);
	ccd<RepoDir>(reponame);
	// probably should use a git virtual instead of passing envp everywhere...
	git.init();
	// this can always be overriden later.
	git.config("user.name", "merge_repos");
	git.config("user.email", "merge_repos.program@localhost");
	git.empty_commit("repository merge initial commit");

	// TODO: fetching individual repositories into git store.
	// git fetch --no-tags $path refs/*:refs/monorepo_merge/$name/*
	// --no-tags is just there to prevent tags polluting the main tag list;
	// they won't appear in git tag after but will be stored as an accessible ref still
	// (despite what git tag's output may indicate).
}



int main(int argc, char** argv, char** envp) {
	const char* basedir;
	const char* reponame;

	int r = check_usage(argc);
	if (r != 0) return r;
	basedir = argv[1];
	reponame = argv[2];
	Git git {envp};
	try {
		run(git, basedir, reponame);
	} catch (const MkdirException<RepoDir>& e) {
		if (e.sys == EEXIST) {
			fputs("# Target repository will only be created clean. Please remove the old one.\n", stderr);
		} else {
			// shame we can't fall-through to the next catch block to let that print the error code.
			fprintf(stderr, "# Target repo mkdir failed %d\n", e.sys);
		}
		throw;
	} catch (const SystemException& e) {
		fprintf(stderr, "errno=%d\n", e.sys);
		throw;
	}
}



