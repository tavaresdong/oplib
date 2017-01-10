/*
 * Common.H
 *
 *  Created on: Dec 27, 2016
 *      Author: yuchend
 */

#ifndef OPLIB_NET_COMMON_H_
#define OPLIB_NET_COMMON_H_

#define LIKELY(exp) __builtin_expect(!!(exp), 1)
#define UNLIKELY(exp) __builtin_expect(!!(exp), 0)

// Use do..while(0) to wrap multiple statements in a macro
#define CHECK_RETURN(ret) do {__typeof__(ret) rc = (ret);\
												      if(rc != 0) exit(rc); } while(0)


#endif /* OPLIB_NET_COMMON_H_ */
