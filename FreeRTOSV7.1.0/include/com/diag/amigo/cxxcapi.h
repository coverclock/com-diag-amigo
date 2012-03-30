#ifndef _COM_DIAG_AMIGO_CXXCAPI_H_
#define _COM_DIAG_AMIGO_CXXCAPI_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * Adapted from Desperadito\n
 * http://www.diag.com/navigation/downloads/Desperadito.html\n
 * This header file defines preprocessor symbols that support the sharing
 * of header files between C and C++ translation units. SOME OF THE SYMBOLS
 * THAT IT DEFINES MAY NOT HAVE THE USUAL COM_DIAG_AMIGO_ PREFIX. "CXXC" stands
 * for "C++/C" and is pronounced "sexy".
 */

#if !defined(NULL)
    /*
     * I have recently been disabused of the notion that NULL is now
     * a pre-defined symbol in later versionz of the GCC compiler.
     */
#   if defined(__cplusplus)
        /**
         * @def NULL
         *
         * NULL is the value of the null pointer. The use of this manifest
         * constant should be avoided in C++, but may be necessary when
         * writing header files that may be included in either C or C++
         * translation units.
         */
#       define NULL (0)
#   else
        /**
         * @def NULL
         *
         * NULL is the value of the null pointer. The use of this manifest
         * constant should be avoided in C++, but may be necessary when
         * writing header files that may be included in either C or C++
         * translation units.
         */
#       define NULL ((void*)0)
#   endif
#endif


#if defined(__cplusplus)
    /**
     * @def CXXCAPI
     *
     * This symbol is used in both declarations and definitions of
     * functions in C++ translation units that are callable from C
     * translation units.
     */
#   define CXXCAPI extern "C"
#else
    /**
     * @def CXXCAPI
     *
     * This symbol is used in declarations of functions in C translation
     * units that are defined in C++ translation units.
     */
#   define CXXCAPI extern
#endif


#if defined(__cplusplus)
    /**
     * @def CXXCINLINE
     *
     * C++ allows inline keyword.
     */
#	define CXXCINLINE inline
#elif (defined(__STDC_VERSION__)&&((__STDC_VERSION__) > 199901L))
    /**
     * @def CXXCINLINE
     *
     * ANSI C beyond 1999-01 allows inline keyword.
     */
#	define CXXCINLINE inline
#elif (defined(__GNUC__)&&defined(__GNUC_MINOR__)&&((((__GNUC__)*1000)+(__GNUC_MINOR__))>=2007))
    /**
     * @def CXXCINLINE
     *
     * GNU GCC beyond 2.7 defines __inline__ keyword.
     */
#	define CXXCINLINE __inline__
#elif defined(inline)
    /**
     * @def CXXCINLINE
     *
     * Platform has already defined inline preprocessor symbol.
     */
#	define CXXCINLINE inline
#else
    /**
     * @def CXXCINLINE
     *
     * Last ditch effort to at least prevent duplicate symbols at link time.
     */
#	define CXXCINLINE static
#endif


/**
 * @def CXXCTOKEN
 *
 * This exists solely to get around an issue with token replacement in the C
 * preprocessor. cpp from GCC 4.4.3 is seriously unhappy with using the
 * preprocessor pasting operator when one of the tokens is a namespace
 * with the double colon syntax.
 */
#define CXXCTOKEN(_TOKEN_) _TOKEN_

#if defined(__cplusplus)
/**
 * @def CXXCTYPE
 *
 * This symbol is used with type references in header files included in
 * both C and C++ translation units to define the type as to whether it
 * has a namespace prefix. C++ symbols will, C symbols will not.
 */
#	define CXXCTYPE(_NAMESPACE_, _TYPE_) CXXCTOKEN(_NAMESPACE_)CXXCTOKEN(_TYPE_)
#else
/**
 * @def CXXCTYPE
 *
 * This symbol is used with type references in header files included in
 * both C and C++ translation units to define the type as to whether it
 * has a namespace prefix. C++ symbols will, C symbols will not.
 */
#	define CXXCTYPE(_NAMESPACE_, _TYPE_) _TYPE_
#endif


#if defined(__cplusplus)
/**
 * @def CXXCSTATICCAST
 *
 * Perform a static cast. This is probably the cast you want to use.
 */
#	define CXXCSTATICCAST(_TYPE_, _VARIABLE_) (static_cast<_TYPE_>(_VARIABLE_))
/**
 * @def CXXCREINTERPRETCAST
 *
 * Perform a reinterpret cast. If the C++ compiler requires you to do this kind
 * of cast, you should look very closely at what you're doing. It's trying to
 * tell you that you might be in trouble.
 */
#	define CXXCREINTERPRETCAST(_TYPE_, _VARIABLE_) (reinterpret_cast<_TYPE_>(_VARIABLE_))
/**
 * @def CXXCCONSTCAST
 *
 * Perform a const cast. You want to cast away const? Srsly? Think about this.
 */
#	define CXXCCONSTCAST(_TYPE_, _VARIABLE_) (const_cast<_TYPE_>(_VARIABLE_))
#else
/**
 * @def CXXCSTATICCAST
 *
 * Perform a cast.
 */
#	define CXXCSTATICCAST(_TYPE_, _VARIABLE_) ((_TYPE_)(_VARIABLE_))
/**
 * @def CXXCREINTERPRETCAST
 *
 * Perform a cast.
 */
#	define CXXCREINTERPRETCAST(_TYPE_, _VARIABLE_) ((_TYPE_)(_VARIABLE_))
/**
 * @def CXXCCONSTCAST
 *
 * Perform a cast.
 */
#	define CXXCCONSTCAST(_TYPE_, _VARIABLE_) ((_TYPE_)(_VARIABLE_))
#endif


#if defined(__cplusplus)
/**
 * @def CXXCNAMESPACEBEGIN
 *
 * Begin the Amigo namespace.
 */
#	define CXXCNAMESPACEBEGIN namespace com { namespace diag { namespace amigo {
/**
 * @def CXXCNAMESPACEEND
 *
 * End the Amigo namespace.
 */
#	define CXXCNAMESPACEEND } } }
#else
/**
 * @def CXXCNAMESPACEBEGIN
 *
 * Begin the Amigo namespace.
 */
#	define CXXCNAMESPACEBEGIN
/**
 * @def CXXCNAMESPACEEND
 *
 * End the Amigo namespace.
 */
#	define CXXCNAMESPACEEND
#endif


#endif
