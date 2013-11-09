/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Joe Watkins <joe.watkins@live.co.uk>                         |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_EXPLAIN_H
#define PHP_EXPLAIN_H

extern zend_module_entry explain_module_entry;
#define phpext_explain_ptr &explain_module_entry

#define PHP_EXPLAIN_VERSION "0.1.0"
#define PHP_EXPLAIN_EXTNAME "explain"

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(explain)
  HashTable explained;
  HashTable zval_cache;
ZEND_END_MODULE_GLOBALS(explain)

#ifdef ZTS
#define EX_G(v) TSRMG(explain_globals_id, zend_explain_globals *, v)
#else
#define EX_G(v) (explain_globals.v)
#endif

#endif	/* PHP_EXPLAIN_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
