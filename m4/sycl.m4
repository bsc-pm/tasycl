AC_DEFUN([AX_CHECK_SYCL],[

#Check if SYCL headers and libraries implementation is installed.
AC_ARG_WITH(sycl,
[AS_HELP_STRING([--with-sycl,--with-sycl=PATH],
                [search in system directories or specify prefix directory for installed SYCL package. E.g. for Intel OneAPI this would be the directory 'oneapi-path/compilers/XXXX.X.X/linux'])])
AC_ARG_WITH(sycl-include,
[AS_HELP_STRING([--with-sycl-include=PATH],
                [specify directory for installed SYCL include files - where CL/sycl.hpp is found])])

# Search for SYCL by default
AS_IF([test "$with_sycl" != yes],[
  syclinc="-I$with_sycl/include/sycl -I$with_sycl/include"
])

# Search for SYCL by default
AS_IF([test "x$with_sycl_include" != x],[
  syclinc="-I$with_sycl_include/ -I$with_sycl_include/../"
])

# Tests if provided headers and libraries are usable and correct
AX_VAR_PUSHVALUE([CPPFLAGS],[$CPPFLAGS $syclinc])
AX_VAR_PUSHVALUE([CFLAGS])
AX_VAR_PUSHVALUE([LDFLAGS],[$LDFLAGS $sycllib])
AX_VAR_PUSHVALUE([LIBS],[])


AC_CHECK_HEADERS([CL/sycl.hpp sycl/ext/oneapi/accessor_property_list.hpp], [sycl=yes], [sycl=no])

AX_VAR_POPVALUE([CPPFLAGS])
AX_VAR_POPVALUE([CFLAGS])
AX_VAR_POPVALUE([LDFLAGS])
AX_VAR_POPVALUE([LIBS])

AS_IF([test "$sycl" != "yes"],[
    AC_MSG_ERROR([
------------------------------
SYCL path was not correctly specified.
Please, check that the provided directories are correct.
------------------------------])
])

AC_SUBST([sycl])
AC_SUBST([syclinc])

])dnl AX_CHECK_SYCL

