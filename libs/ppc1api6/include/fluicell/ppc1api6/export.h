/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | PPC1 API                                                                  |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

 // for shared libraries 
 
#ifndef __PPC1API_EXPORT_
#define __PPC1API_EXPORT_

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef PPC1API
#    define PPC1API_EXPORT __declspec(dllexport)
#  else
#    define PPC1API_EXPORT __declspec(dllimport)
#  endif
#else
#  define PPC1API_EXPORT
#endif

#endif // __PPC1API_EXPORT_

