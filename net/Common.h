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

#define CHECK_RETURN(ret) ({__typeof__(ret) rc = (ret);\
	                       if(rc != 0) exit(rc); });


#endif /* NET_COMMON_H_ */
