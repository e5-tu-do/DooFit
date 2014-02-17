/**
 * @file stacktrace.h
 * @author Timo Bingmann
 * stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
 * published under the WTFPL v2.0
 *
 * @autho Manuel Schiller
 * @date 2013-01-27
 * added code to compile under FreeBSD, fallback stacktraces using gdb
 */

#ifndef _STACKTRACE_H_
#define _STACKTRACE_H_

#include <cstdio>
#include <cstdlib>

#if defined(__GLIBC__)

// when using glibc, it does most of the work for us, only need to demangle
// C++ symbol names...
#include <execinfo.h>
#include <cxxabi.h>

/** Print a demangled stack backtrace of the caller function to FILE* out. */
static inline void stacktrace(FILE *out = stderr)
{
    fprintf(out, "stack trace:\n");

    // storage array for stack trace address data
    void* addrlist[64];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0) {
	fprintf(out, "  <empty, possibly corrupt>\n");
	return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // string which will be filled with the demangled function name
    size_t funcnamesize = 0;
    char* funcname = 0;

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++)
    {
	char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

	// find parentheses and +address offset surrounding the mangled name:
	// ./module(function+0x15c) [0x8048a6d]
	for (char *p = symbollist[i]; *p; ++p)
	{
	    if (*p == '(')
		begin_name = p;
	    else if (*p == '+')
		begin_offset = p;
	    else if (*p == ')' && begin_offset) {
		end_offset = p;
		break;
	    }
	}

	if (begin_name && begin_offset && end_offset
	    && begin_name < begin_offset)
	{
	    *begin_name++ = '\0';
	    *begin_offset++ = '\0';
	    *end_offset = '\0';

	    // mangled name is now in [begin_name, begin_offset) and caller
	    // offset in [begin_offset, end_offset). now apply
	    // __cxa_demangle():

	    int status;
	    char* ret = abi::__cxa_demangle(begin_name,
					    funcname, &funcnamesize, &status);
	    if (status == 0) {
		funcname = ret; // use possibly realloc()-ed string
		fprintf(out, "  %s : %s+%s\n",
			symbollist[i], funcname, begin_offset);
	    }
	    else {
		// demangling failed. Output function name as a C function with
		// no arguments.
		fprintf(out, "  %s : %s()+%s\n",
			symbollist[i], begin_name, begin_offset);
	    }
	}
	else
	{
	    // couldn't parse the line? print the whole line.
	    fprintf(out, "  %s\n", symbollist[i]);
	}
    }

    free(funcname);
    free(symbollist);
}

#elif defined(__FreeBSD__)

// the following appears work under FreeBSD:
// 1. use the gcc builtins __builtin_frame_address, __builtin_return_address
//    to get a stack trace
// 2. use the C library's dladdr to translate return addresses to symbol names
// 3. use C++ runtime to demangle symbol names
//
// note: depending on your architecture, you may need to compile with
//       -fno-omit-frame-pointer to make this succeed, or it may not work at
//       all
#include <dlfcn.h>
#include <cxxabi.h>

#undef SRET
#undef SRET2
#undef SRET4
#undef SRET8
#undef SRET16
#undef SRET32
#undef SRET64
#define SRET(i) { addrs[i] = ((0 == i || addrs[i - 1]) && \
	__builtin_frame_address(i)) ? __builtin_return_address(i) : 0; }
#define SRET2(i) SRET(2 * i); SRET(2 * i + 1);
#define SRET4(i) SRET2(2 * i); SRET2(2 * i + 1);
#define SRET8(i) SRET4(2 * i); SRET4(2 * i + 1);
#define SRET16(i) SRET8(2 * i); SRET8(2 * i + 1);
#define SRET32(i) SRET16(2 * i); SRET16(2 * i + 1);
#define SRET64(i) SRET32(2 * i); SRET32(2 * i + 1);

/** Print a demangled stack backtrace of the caller function to FILE* out. */
static inline void stacktrace(FILE* out = stderr)
{
    const char *fmtstr =
	(2 == sizeof(void*)) ? "  #%2u: 0x%04x %s+0x%04x\t(%s)\n" :
	((4 == sizeof(void*)) ? "  #%2u: 0x%08x %s+0x%04x\t(%s)\n" :
	 ((8 == sizeof(void*)) ? "  #%2u: 0x%016x %s+0x%04x\t(%s)\n" :
	  "  #%2u: 0x%x\t%s+0x%04x\t(%s)\n"));
    const char *fmtstrnoinfo =
	(2 == sizeof(void*)) ? "  #%2u: 0x%04x ???\n" :
	((4 == sizeof(void*)) ? "  #%2u: 0x%08x ???\n" :
	 ((8 == sizeof(void*)) ? "  #%2u: 0x%016x ???\n" :
	  "  #%2u: 0x%x\t???\n"));
    void *addrs[64];
    size_t funcnamesz = 0;
    char *funcname = 0;
    unsigned level = 0;
    SRET64(0);
    std::fprintf(out, "stack trace:\n");
    for (level = 0; level < 64 && addrs[level]; ++level) {
	void* addr = addrs[level];
	Dl_info info;
	int dlrc = dladdr(addr, &info);
	if (dlrc) {
	    // try to demangle c++ names
	    bool demangleOK = false;
	    if (info.dli_sname) {
		int status;
		char *ret = abi::__cxa_demangle(info.dli_sname, funcname,
			&funcnamesz, &status);
		if (0 == status) {
		    funcname = ret; // use possibly realloced string
		    demangleOK = 0 != funcname;
		}
	    }
	    std::fprintf(out, fmtstr, level, addr,
		    demangleOK ? funcname : info.dli_sname,
		    static_cast<void*>(static_cast<char*>(0) +
			(static_cast<char*>(addr) -
			 static_cast<char*>(info.dli_saddr))),
		    info.dli_fname);
	} else {
	    std::fprintf(out, fmtstrnoinfo, level, addr);
	}
    };
    if (0 == level) std::fprintf(out, "  <empty, possibly corrupt>\n");
    free(funcname);
}
#undef SRET64
#undef SRET32
#undef SRET16
#undef SRET8
#undef SRET4
#undef SRET2
#undef SRET

#else

// when nothing else works, and gdb is available, we can use it

#include <unistd.h>

/** Print a demangled stack backtrace of the caller function to FILE* out. */
static inline void stacktrace(FILE* out = stderr)
{
    char cmd[128];
    std::snprintf(cmd, 127,
	    "gdb -q -batch -ex \"attach %d\" -ex 'bt' -x detach 1>&%d 2>&%d",
	    getpid(), fileno(out), fileno(out));
    cmd[127] = 0;
    if (0 != std::system(cmd))
	fprintf(out, "Unable to produce stack trace with gdb!\n");
}

#endif

#endif // _STACKTRACE_H_
// vim:sw=4:tw=78:ft=cpp
