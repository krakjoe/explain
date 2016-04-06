/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "php_explain.h"

#include "Zend/zend_interfaces.h"

typedef struct _explain_opcode_t {
  const char *name;
  size_t  name_len;
  zend_uchar opcode;
} explain_opcode_t;

#define EXPLAIN_FILE   0x00000001
#define EXPLAIN_STRING 0x00000010
#define EXPLAIN_OPLINE 0x00000011

#define EXPLAIN_OPCODE_NAME(c) \
  {#c, sizeof(#c)-1, c}

#include "explain_opcodes.h"

ZEND_DECLARE_MODULE_GLOBALS(explain);

static inline void explain_opcode(long opcode, zval *return_value) { /* {{{ */

  if (opcode >= sizeof(opcodes)) {
    ZVAL_STRINGL(return_value, "unknown", strlen("unknown"));
  }

  explain_opcode_t decode = opcodes[opcode];

  if (decode.opcode == opcode) {
    ZVAL_STRINGL(return_value, decode.name, decode.name_len);
  } else {
    ZVAL_STRINGL(return_value, "unknown", strlen("unknown"));
  }
} /* }}} */

static int explain_variable(zend_ulong var, zend_llist *vars) { /* {{{ */
  zend_llist_position position;
  zend_ulong *cache = zend_llist_get_first_ex(vars, &position);
  zend_ulong id = 0;

  while (cache) {
    if (*cache == var) {
      return id;
    }
    cache = zend_llist_get_next_ex(vars, &position);
    id++;
  }

  zend_llist_add_element(vars, &var);

  return id;
} /* }}} */

static inline void explain_zend_op(zend_op_array *ops, znode_op *op, uint32_t type, const char *name, size_t name_len, zend_llist *vars, zval *return_value) { /* {{{ */

  if (!op || type == IS_UNUSED)
    return;

  switch (type) {
  case IS_CV:
    add_assoc_str_ex(return_value, name, name_len, ops->vars[(op->var - sizeof(zend_execute_data)) / sizeof(zval)]);
    break;

  case IS_VAR:
  case IS_TMP_VAR:
    {
      /* convert this to a human friendly number */
      add_assoc_long_ex(return_value, name, name_len, explain_variable((zend_ulong) ops->vars - op->var, vars));
    }
    break;

  case IS_CONST:
    {
      zval *zv = RT_CONSTANT_EX(ops->literals, *op);
      add_assoc_zval_ex(return_value, name, name_len, zv);
    }
    break;
  }
} /* }}} */

static inline void explain_optype(uint32_t type, zval *return_value) { /* {{{ */
  switch (type) {
  case IS_CV: ZVAL_STRINGL(return_value, "IS_CV", strlen("IS_CV")); break;
  case IS_TMP_VAR: ZVAL_STRINGL(return_value, "IS_TMP_VAR", strlen("IS_TMP_VAR")); break;
  case IS_VAR: ZVAL_STRINGL(return_value, "IS_VAR", strlen("IS_VAR")); break;
  case IS_CONST: ZVAL_STRINGL(return_value, "IS_CONST", strlen("IS_CONST")); break;
  case IS_UNUSED: ZVAL_STRINGL(return_value, "IS_UNUSED", strlen("IS_UNUSED")); break;
    /* special case for jmp's */
  case EXPLAIN_OPLINE: ZVAL_STRINGL(return_value, "IS_OPLINE", strlen("IS_OPLINE")); break;

  default:
    if (type & EXT_TYPE_UNUSED) {
      switch (type &~ EXT_TYPE_UNUSED) {
      case IS_CV: ZVAL_STRINGL(return_value, "IS_CV|EXT_TYPE_UNUSED", strlen("IS_CV|EXT_TYPE_UNUSED")); break;
      case IS_TMP_VAR: ZVAL_STRINGL(return_value, "IS_TMP_VAR|EXT_TYPE_UNUSED", strlen("IS_TMP_VAR|EXT_TYPE_UNUSED")); break;
      case IS_VAR: ZVAL_STRINGL(return_value, "IS_VAR|EXT_TYPE_UNUSED", strlen("IS_VAR|EXT_TYPE_UNUSED")); break;
      case IS_CONST: ZVAL_STRINGL(return_value, "IS_CONST|EXT_TYPE_UNUSED", strlen("IS_CONST|EXT_TYPE_UNUSED")); break;
      case IS_UNUSED: ZVAL_STRINGL(return_value, "IS_UNUSED|EXT_TYPE_UNUSED", strlen("IS_UNUSED|EXT_TYPE_UNUSED")); break;
      }
    } else {
      ZVAL_STRINGL(return_value, "unknown", strlen("unknown"));
    }
  }
} /* }}} */

static inline void explain_op_array(zend_op_array *ops, zval *return_value) { /* {{{ */
  if (ops) {
    uint32_t  next = 0;
    zend_llist vars;

    zend_llist_init(&vars, sizeof(zend_ulong), NULL, 0);

    do
      {
        zval zopline;

        array_init(&zopline);
        {
          zend_op *opline = &ops->opcodes[next];

          add_assoc_long_ex(&zopline, "opline", sizeof("opline")-1, next);

          add_assoc_long_ex(&zopline, "opcode", sizeof("opcode")-1, opline->opcode);

          switch (opline->opcode) {
          case ZEND_JMP:
#ifdef ZEND_GOTO
          case ZEND_GOTO:
#endif
#ifdef ZEND_FAST_CALL
          case ZEND_FAST_CALL:
#endif
            add_assoc_long_ex(&zopline, "op1_type", sizeof("op1_type")-1, EXPLAIN_OPLINE);
            add_assoc_long_ex(&zopline, "op1", sizeof("op1")-1, 0/*opline->op1.jmp_addr - ops->opcodes*/);
            break;

          case ZEND_JMPZNZ:
            add_assoc_long_ex(&zopline, "op1_type", sizeof("op1_type")-1, opline->op1_type);
            explain_zend_op(ops, &opline->op1, opline->op1_type, "op1", sizeof("op1")-1, &vars, &zopline);

            /* TODO(krakjoe) needs opline->extended_value on true, opline_num on false */
            add_assoc_long_ex(&zopline, "op2_type", sizeof("op2_type")-1, EXPLAIN_OPLINE);
            add_assoc_long_ex(&zopline, "op2", sizeof("op2")-1, 0/*opline->op2.opline_num*/);

            add_assoc_long_ex(&zopline, "result_type", sizeof("result_type")-1, opline->result_type);
            explain_zend_op(ops, &opline->result, opline->result_type, "result", sizeof("result")-1, &vars, &zopline);
            break;

          case ZEND_JMPZ:
          case ZEND_JMPNZ:
          case ZEND_JMPZ_EX:
          case ZEND_JMPNZ_EX:

#ifdef ZEND_JMP_SET
          case ZEND_JMP_SET:
#endif
#ifdef ZEND_JMP_SET_VAR
          case ZEND_JMP_SET_VAR:
#endif
            add_assoc_long_ex(&zopline, "op1_type", sizeof("op1_type")-1, opline->op1_type);
            explain_zend_op(ops, &opline->op1, opline->op1_type, "op1", sizeof("op1")-1, &vars, &zopline);

            add_assoc_long_ex(&zopline, "op2_type", sizeof("op2_type")-1, EXPLAIN_OPLINE);
            add_assoc_long_ex(&zopline, "op2", sizeof("op2")-1, 0/*opline->op2.jmp_addr - ops->opcodes*/);

            add_assoc_long_ex(&zopline, "result_type", sizeof("result_type")-1, opline->result_type);
            explain_zend_op(ops, &opline->result, opline->result_type, "result", sizeof("result")-1, &vars, &zopline);
            break;

          case ZEND_RECV_INIT:
            add_assoc_long_ex(&zopline, "result_type", sizeof("result_type")-1, opline->result_type);

            explain_zend_op(ops, &opline->result, opline->result_type, "result", sizeof("result")-1, &vars, &zopline);
            break;

          default:
              add_assoc_long_ex(&zopline, "op1_type", sizeof("op1_type")-1, opline->op1_type);
              explain_zend_op(ops, &opline->op1, opline->op1_type, "op1", sizeof("op1")-1, &vars, &zopline);

              add_assoc_long_ex(&zopline, "op2_type", sizeof("op2_type")-1, opline->op2_type);
              explain_zend_op(ops, &opline->op2, opline->op2_type, "op2", sizeof("op2")-1, &vars, &zopline);

              add_assoc_long_ex(&zopline, "result_type", sizeof("result_type")-1, opline->result_type);
              explain_zend_op(ops, &opline->result, opline->result_type, "result", sizeof("result")-1, &vars, &zopline);
          }

          if (opline->extended_value) {
            add_assoc_long_ex(&zopline, "extended_value", sizeof("extended_value")-1, opline->extended_value);
          }

          add_assoc_long_ex(&zopline, "lineno", sizeof("lineno")-1, opline->lineno);
        }
        add_next_index_zval(return_value, &zopline);
      } while (++next < ops->last);
    zend_llist_destroy(&vars);
  } else {
    RETURN_FALSE;
  }
}

static inline void explain_create_caches(HashTable *classes, HashTable *functions) { /* {{{ */
  zend_function tf;
  zend_class_entry *te;

  zend_hash_init(classes, zend_hash_num_elements(classes), NULL, NULL, 0);
  zend_hash_copy(classes, CG(class_table), NULL);

  zend_hash_init(functions, zend_hash_num_elements(functions), NULL, NULL, 0);
  zend_hash_copy(functions, CG(function_table), NULL);
} /* }}} */

static inline void explain_destroy_caches(HashTable *classes, HashTable *functions) { /* {{{ */
  zend_hash_destroy(classes);
  zend_hash_destroy(functions);
} /* }}} */

/* {{{ proto array explain(string code [, long options, array &classes, array &functions])
   explain some code */
PHP_FUNCTION(explain)
{
  zval *code = NULL,
    *classes = NULL,
    *functions = NULL;
  zend_long options = EXPLAIN_FILE;
  HashTable caches[2] = {0, 0};

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|lz/z/", &code, &options, &classes, &functions) == FAILURE) {
    return;
  }

  {
    zend_file_handle fh;
    zend_op_array *ops = NULL;

    if (options & EXPLAIN_FILE) {
      if (php_stream_open_for_zend_ex(Z_STRVAL_P(code), &fh, USE_PATH|STREAM_OPEN_FOR_INCLUDE) == SUCCESS) {
        explain_create_caches(&caches[0], &caches[1]);
        ops = zend_compile_file(&fh, ZEND_INCLUDE);
        zend_destroy_file_handle(&fh);
      } else {
        zend_error(E_WARNING, "file %s couldn't be opened", Z_STRVAL_P(code));
        RETURN_FALSE;
      }
    } else if (options & EXPLAIN_STRING) {
      explain_create_caches(&caches[0], &caches[1]);
      ops = zend_compile_string(code, "explained");
      if (!ops) {
        explain_destroy_caches(&caches[0], &caches[1]);
      }
    } else {
      zend_error(E_WARNING, "invalid options passed to explain (%lu), please see documentation", options);
      RETURN_FALSE;
    }

    if (!ops) {
      explain_destroy_caches(&caches[0], &caches[1]);
      zend_error(E_WARNING, "explain was unable to compile code");
      RETURN_FALSE;
    }

    array_init(return_value);
    zend_hash_next_index_insert_ptr(&EX_G(explained), ops);
    explain_op_array(ops, return_value);

    if (classes) {
      zend_class_entry *pce;
      zend_string *pce_name;

      if (Z_TYPE_P(classes) != IS_ARRAY) {
        array_init(classes);
      }

      ZEND_HASH_FOREACH_STR_KEY_PTR(CG(class_table), pce_name, pce) {
        if (pce->type == ZEND_USER_CLASS &&
            !zend_hash_exists(&caches[0], pce_name)) {

          zval zce;
          zend_function *pfe;

          array_init(&zce);

          ZEND_HASH_FOREACH_PTR(&pce->function_table, pfe) {
            if (pfe->common.type == ZEND_USER_FUNCTION) {
              zval zfe;

              array_init(&zfe);
              explain_op_array(&pfe->op_array, &zfe);
              zend_hash_add_new(Z_ARRVAL_P(&zce), pfe->common.function_name, &zfe);
            }
          } ZEND_HASH_FOREACH_END();

          zend_hash_add_new(Z_ARRVAL_P(classes), pce->name, &zce);
        }
      } ZEND_HASH_FOREACH_END();
    }

    if (functions) {
      HashPosition position;
      zend_function *pfe;
      zend_string *fe_name;

      if (Z_TYPE_P(functions) != IS_ARRAY) {
        array_init(functions);
      }

      ZEND_HASH_FOREACH_STR_KEY_PTR(CG(function_table), fe_name, pfe) {
        if (pfe->common.type == ZEND_USER_FUNCTION &&
            !zend_hash_exists(&caches[1], fe_name)) {
          zval zfe;

          array_init(&zfe);
          explain_op_array(&pfe->op_array, &zfe);
          zend_hash_add(Z_ARRVAL_P(functions), fe_name, &zfe);
        }
      } ZEND_HASH_FOREACH_END();
    }
    explain_destroy_caches(&caches[0], &caches[1]);
  }
}
/* }}} */

/* {{{ proto string explain_opcode(integer opcode)
   get the friendly name for an opcode */
PHP_FUNCTION(explain_opcode) {
  long opcode;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &opcode) == FAILURE) {
    return;
  }

  explain_opcode(opcode, return_value);
} /* }}} */

/* {{{ proto string explain_optype(integer optype)
   get the friendly name for an optype */
PHP_FUNCTION(explain_optype) {
  long optype;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &optype) == FAILURE) {
    return;
  }

  explain_optype((zend_uchar)optype, return_value);
} /* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(explain)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "explain support", "enabled");
  php_info_print_table_end();
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_explain, 0, 0, 1)
ZEND_ARG_INFO(0, code)
ZEND_ARG_INFO(0, options)
ZEND_ARG_INFO(1, classes)
ZEND_ARG_INFO(1, functions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_explain_opcode, 0, 0, 1)
ZEND_ARG_INFO(0, opcode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_explain_optype, 0, 0, 1)
ZEND_ARG_INFO(0, optype)
ZEND_END_ARG_INFO()

/* {{{ explain_functions[]
 *
 */
const zend_function_entry explain_functions[] = {
  PHP_FE(explain,        arginfo_explain)
  PHP_FE(explain_opcode, arginfo_explain_opcode)
  PHP_FE(explain_optype, arginfo_explain_optype)
  PHP_FE_END
};
/* }}} */

/* {{{ */
static inline void php_explain_globals_ctor(zend_explain_globals *eg) {} /* }}} */

static inline void php_explain_destroy_ops(zend_op_array **ops) { /* {{{ */
  if ((*ops)) {
    destroy_op_array(*ops);
    efree_size(*ops, sizeof(zend_op_array));
  }
} /* }}} */

/* {{{ MINIT */
static PHP_MINIT_FUNCTION(explain) {
  ZEND_INIT_MODULE_GLOBALS(explain, php_explain_globals_ctor, NULL);

  REGISTER_LONG_CONSTANT("EXPLAIN_STRING",          EXPLAIN_STRING,      CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("EXPLAIN_FILE",            EXPLAIN_FILE,        CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("EXPLAIN_OPLINE",          EXPLAIN_OPLINE,      CONST_CS | CONST_PERSISTENT);

  REGISTER_LONG_CONSTANT("EXPLAIN_IS_UNUSED",       IS_UNUSED,           CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("EXPLAIN_IS_VAR",          IS_VAR,              CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("EXPLAIN_IS_TMP_VAR",      IS_TMP_VAR,          CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("EXPLAIN_IS_CV",           IS_CV,               CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("EXPLAIN_EXT_TYPE_UNUSED", EXT_TYPE_UNUSED,     CONST_CS | CONST_PERSISTENT);

  return SUCCESS;
} /* }}} */

static PHP_RINIT_FUNCTION(explain) {
  zend_hash_init(&EX_G(explained), 8, NULL, (dtor_func_t) php_explain_destroy_ops, 0);
  zend_hash_init(&EX_G(zval_cache), 8, NULL, (dtor_func_t) ZVAL_PTR_DTOR, 0);
}

static PHP_RSHUTDOWN_FUNCTION(explain) {
  zend_hash_destroy(&EX_G(explained));
  zend_hash_destroy(&EX_G(zval_cache));
}

/* {{{ explain_module_entry
 */
zend_module_entry explain_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_EXPLAIN_EXTNAME,
  explain_functions,
  PHP_MINIT(explain),
  NULL,
  PHP_RINIT(explain),
  PHP_RSHUTDOWN(explain),
  PHP_MINFO(explain),
  PHP_EXPLAIN_VERSION,
  STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EXPLAIN
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
# endif
ZEND_GET_MODULE(explain)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
