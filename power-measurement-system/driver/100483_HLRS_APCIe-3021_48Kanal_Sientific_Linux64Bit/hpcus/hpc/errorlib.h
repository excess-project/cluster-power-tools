/** @file errorlib.h
 * @brief Utility functions for error messages  
 */

#ifndef ERRORLIB_H_
#define ERRORLIB_H_



/** unconditionaly pretty print IOCTL return code string 
 * @parm __ret function return call
 * @parm __fcn function or command called
 */
static inline int __check_status_msg(int __ret, const char* __fcnname, const char* (*__msgfcn)(int) )
{
	if(__ret)
		printf("[%s: call result was \"%s\" (%d)]\n", __fcnname,( (__ret) > 0 ? __msgfcn(__ret) : strerror(errno)  ) ,(__ret));
	return __ret;
}

/** pretty print IOCTL return code string if __ret not 0
 * @parm __ret function return call
 * @parm __fcn function or command called
 * 
 * @retval returns __ret, which allows to chain calls 
 */ 
#define STATUS_MSG(__ret, __fcnname, __msgfcn)  __check_status_msg( __ret, __fcnname, __msgfcn )

/** encapsulate IOCTL command __cmd on filedescriptor __fd with argument __arg, printing out an message if the return value is not 0 (a function named msg_"__cmd" must be defined */ 
#define call_ioctl(__fd,__request,__arg) STATUS_MSG( ioctl(__fd, __request,__arg), #__request, msg_##__request)

#endif /*ERRORLIB_H_*/
