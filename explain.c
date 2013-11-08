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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "php_explain.h"

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

static inline void explain_opcode(long opcode, zval **return_value_ptr TSRMLS_DC) { /* {{{ */
  explain_opcode_t decode = opcodes[opcode];
  
  if (decode.opcode == opcode) {
    ZVAL_STRINGL(
      *return_value_ptr, decode.name, decode.name_len, 1);
  } else ZVAL_STRINGL(*return_value_ptr, "unknown", strlen("unknown"), 1);
} /* }}} */

static int explain_variable(zend_ulong var, zend_llist *vars TSRMLS_DC) { /* {{{ */
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

static inline void explain_zend_op(zend_op_array *ops, znode_op *op, zend_uint type, const char *name, size_t name_len, zend_llist *vars, zval **return_value_ptr TSRMLS_DC) { /* {{{ */
  if (!op || type == IS_UNUSED)
    return;

  switch (type) {
    case IS_CV: {
      add_assoc_stringl_ex(*return_value_ptr, name, name_len, (char*) ops->vars[op->var].name, ops->vars[op->var].name_len, 1);
    } break;

    case IS_VAR:
    case IS_TMP_VAR: {
      /* convert this to a human friendly number */
      add_assoc_long_ex(*return_value_ptr, name, name_len, explain_variable((zend_ulong) ops->vars - op->var, vars TSRMLS_CC));
    } break;

    case IS_CONST: {
      zval  *copy;

      ALLOC_ZVAL(copy);
      *copy = (op->literal->constant);
      zval_copy_ctor(copy);
      
      add_assoc_zval_ex(
        *return_value_ptr, name, name_len, copy);
        
      Z_DELREF_P(copy);
    } break;
  }
} /* }}} */

static inline void explain_optype(zend_uint type, zval **return_value_ptr TSRMLS_DC) { /* {{{ */
  switch (type) {
    case IS_CV: ZVAL_STRINGL(*return_value_ptr, "IS_CV", strlen("IS_CV"), 1); break;
    case IS_TMP_VAR: ZVAL_STRINGL(*return_value_ptr, "IS_TMP_VAR", strlen("IS_TMP_VAR"), 1); break;
    case IS_VAR: ZVAL_STRINGL(*return_value_ptr, "IS_VAR", strlen("IS_VAR"), 1); break;
    case IS_CONST: ZVAL_STRINGL(*return_value_ptr, "IS_CONST", strlen("IS_CONST"), 1); break;
    case IS_UNUSED: ZVAL_STRINGL(*return_value_ptr, "IS_UNUSED", strlen("IS_UNUSED"), 1); break;
    /* special case for jmp's */
    case EXPLAIN_OPLINE: ZVAL_STRINGL(*return_value_ptr, "IS_OPLINE", strlen("IS_OPLINE"), 1); break;
    
    default: if (type & EXT_TYPE_UNUSED) {
      switch (type &~ EXT_TYPE_UNUSED) {
        case IS_CV: ZVAL_STRINGL(*return_value_ptr, "IS_CV|EXT_TYPE_UNUSED", strlen("IS_CV|EXT_TYPE_UNUSED"), 1); break;
        case IS_TMP_VAR: ZVAL_STRINGL(*return_value_ptr, "IS_TMP_VAR|EXT_TYPE_UNUSED", strlen("IS_TMP_VAR|EXT_TYPE_UNUSED"), 1); break;
        case IS_VAR: ZVAL_STRINGL(*return_value_ptr, "IS_VAR|EXT_TYPE_UNUSED", strlen("IS_VAR|EXT_TYPE_UNUSED"), 1); break;
        case IS_CONST: ZVAL_STRINGL(*return_value_ptr, "IS_CONST|EXT_TYPE_UNUSED", strlen("IS_CONST|EXT_TYPE_UNUSED"), 1); break;
        case IS_UNUSED: ZVAL_STRINGL(*return_value_ptr, "IS_UNUSED|EXT_TYPE_UNUSED", strlen("IS_UNUSED|EXT_TYPE_UNUSED"), 1); break;
      }
    } else {
      ZVAL_STRINGL(*return_value_ptr, "unknown", strlen("unknown"), 1);
    }
  }
} /* }}} */


static inline void explain_op_array(zend_op_array *ops, zval *return_value TSRMLS_DC) { /* {{{ */
  if (ops) {
    zend_uint  next = 0;
    zend_llist vars;

    zend_llist_init(&vars, sizeof(zend_ulong), NULL, 0);

    do
    {
      zval *zopline = NULL;
      
      ALLOC_INIT_ZVAL(zopline);
      
      array_init(zopline);
      {
        zend_op *opline = &ops->opcodes[next];

        add_assoc_long_ex(
          zopline, "opline", sizeof("opline"), next);
        add_assoc_long_ex(
          zopline, "opcode", sizeof("opcode"), opline->opcode);
          
        switch (opline->opcode) {
          case ZEND_JMP:
#ifdef ZEND_GOTO
          case ZEND_GOTO:
#endif
#ifdef ZEND_FAST_CALL
          case ZEND_FAST_CALL:
#endif
            add_assoc_long_ex(
              zopline, "op1_type", sizeof("op1_type"), EXPLAIN_OPLINE);
            add_assoc_long_ex(
              zopline, "op1", sizeof("op1"), opline->op1.jmp_addr - ops->opcodes);
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
            add_assoc_long_ex(
              zopline, "op1_type", sizeof("op1_type"), opline->op1_type);
            explain_zend_op(ops, &opline->op1, opline->op1_type, "op1", sizeof("op1"), &vars, &zopline TSRMLS_CC);
            
            add_assoc_long_ex(
              zopline, "op2_type", sizeof("op2_type"), EXPLAIN_OPLINE);
            add_assoc_long_ex(
              zopline, "op2", sizeof("op2"), opline->op2.jmp_addr - ops->opcodes);

            add_assoc_long_ex(
                zopline, "result_type", sizeof("result_type"), opline->result_type);
            explain_zend_op(ops, &opline->result, opline->result_type, "result", sizeof("result"), &vars, &zopline TSRMLS_CC);
          break;
          
          default: {
            add_assoc_long_ex(
              zopline, "op1_type", sizeof("op1_type"), opline->op1_type);
            explain_zend_op(ops, &opline->op1, opline->op1_type, "op1", sizeof("op1"), &vars, &zopline TSRMLS_CC);
            
            add_assoc_long_ex(
              zopline, "op2_type", sizeof("op2_type"), opline->op2_type);
            explain_zend_op(ops, &opline->op2, opline->op2_type, "op2", sizeof("op2"), &vars, &zopline TSRMLS_CC);
            
            add_assoc_long_ex(
                zopline, "result_type", sizeof("result_type"), opline->result_type);
            explain_zend_op(ops, &opline->result, opline->result_type, "result", sizeof("result"), &vars, &zopline TSRMLS_CC);
          }
        }
        
        if (opline->extended_value) {
          add_assoc_long_ex(
            zopline, "extended_value", sizeof("extended_value"), opline->extended_value);
        }
        
        add_assoc_long_ex(
          zopline, "lineno", sizeof("lineno"), opline->lineno);
      }
      add_next_index_zval(return_value, zopline);
    } while (++next < ops->last);
    
    zend_llist_destroy(&vars);
  } else {
    RETURN_FALSE;
  }
}

static inline void explain_create_caches(HashTable *classes, HashTable *functions TSRMLS_DC) { /* {{{ */
  zend_function tf;
  zend_class_entry *te;
  
  zend_hash_init(classes, zend_hash_num_elements(CG(class_table)), NULL, NULL, 0);
  zend_hash_copy(classes, CG(class_table), NULL, &te, sizeof(zend_class_entry*));
  zend_hash_init(functions, zend_hash_num_elements(CG(function_table)), NULL, NULL, 0);
  zend_hash_copy(functions, CG(function_table), NULL, &tf, sizeof(zend_function));
} /* }}} */

static inline void explain_destroy_caches(HashTable *classes, HashTable *functions TSRMLS_DC) { /* {{{ */
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
  zend_ulong options = EXPLAIN_FILE;
  HashTable caches[2] = {0, 0};
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|lzz", &code, &options, &classes, &functions) == FAILURE) {
    return;
  }

  {
    zend_file_handle fh;
    zend_op_array *ops = NULL;

    if (options & EXPLAIN_FILE) {
      if (php_stream_open_for_zend_ex(Z_STRVAL_P(code), &fh, USE_PATH|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC) == SUCCESS) {
        int dummy = 1;

        if (zend_hash_add(&EG(included_files), fh.opened_path, strlen(fh.opened_path)+1, (void**) &dummy, sizeof(int), NULL) == SUCCESS) {
          explain_create_caches(&caches[0], &caches[1] TSRMLS_CC);
          ops = zend_compile_file(
            &fh, ZEND_INCLUDE TSRMLS_CC);
          if (!ops) {
            explain_destroy_caches(&caches[0], &caches[1] TSRMLS_CC);
          }
          zend_destroy_file_handle(&fh TSRMLS_CC);
        } else {
          zend_file_handle_dtor(&fh TSRMLS_CC);
        }
      } else {
        RETURN_FALSE;
      }
    } else if (options & EXPLAIN_STRING) {
      explain_create_caches(&caches[0], &caches[1] TSRMLS_CC);
      ops = zend_compile_string(code, "explained" TSRMLS_CC);
      if (!ops) {
        explain_destroy_caches(&caches[0], &caches[1] TSRMLS_CC);
      }
    } else {
      zend_error(E_WARNING, "invalid options passed to explain (%lu), please see documentation", options);
    }
    
    if (!ops) {
      RETURN_FALSE;
    }

    array_init(return_value);
    
    explain_op_array(ops, return_value TSRMLS_CC);
    
    if (classes) {
      HashPosition position[2];
      zend_class_entry **ppce, *pce;
      
      if (Z_TYPE_P(classes) != IS_ARRAY) {
        array_init(classes);
      }
      
      for (zend_hash_internal_pointer_reset_ex(CG(class_table), &position[0]);
          zend_hash_get_current_data_ex(CG(class_table), (void**) &ppce, &position[0]) == SUCCESS && (pce = *ppce);
          zend_hash_move_forward_ex(CG(class_table), &position[0])) {
          
          if (pce->type  == ZEND_USER_CLASS && !zend_hash_exists(&caches[0], pce->name, pce->name_length)) {
          
            zval *zce;
            zend_function *pfe;
            
            ALLOC_INIT_ZVAL(zce);
            
            array_init(zce);
            
            for (zend_hash_internal_pointer_reset_ex(&pce->function_table, &position[1]);
                 zend_hash_get_current_data_ex(&pce->function_table, (void**) &pfe, &position[1]) == SUCCESS;
                 zend_hash_move_forward_ex(&pce->function_table, &position[1])) {
                 if (pfe->common.type == ZEND_USER_FUNCTION) {
                   zval *zfe;
                 
                   ALLOC_INIT_ZVAL(zfe);
                   
                   array_init(zfe);
                   
                   explain_op_array(&pfe->op_array, zfe TSRMLS_CC);
                   
                   add_assoc_zval_ex(zce, pfe->common.function_name, strlen(pfe->common.function_name)+1, zfe);
                 }
            }
            
            add_assoc_zval_ex(classes, pce->name, pce->name_length+1, zce);
          }
      }
    }

    if (functions) {
      HashPosition position;
      zend_function *pfe;
      char *fe_name;
      zend_uint fe_name_len;
      zend_ulong fe_idx;
      
      if (Z_TYPE_P(functions) != IS_ARRAY) {
        array_init(functions);
      }
      
      for (zend_hash_internal_pointer_reset_ex(CG(function_table), &position);
           zend_hash_get_current_data_ex(CG(function_table), (void**) &pfe, &position) == SUCCESS;
           zend_hash_move_forward_ex(CG(function_table), &position)) {
           
           if (pfe->common.type == ZEND_USER_FUNCTION && 
               zend_hash_get_current_key_ex(CG(function_table), &fe_name, &fe_name_len, &fe_idx, 0, &position) == HASH_KEY_IS_STRING &&
              !zend_hash_exists(&caches[1], fe_name, fe_name_len)) {
             zval *zfe;
           
             ALLOC_INIT_ZVAL(zfe);
             
             array_init(zfe);
             
             explain_op_array(&pfe->op_array, zfe TSRMLS_CC);
             
             add_assoc_zval_ex(functions, fe_name, fe_name_len, zfe);
           }
      }
    }
    
    destroy_op_array(ops TSRMLS_CC);
    efree(ops);
    explain_destroy_caches(&caches[0], &caches[1] TSRMLS_CC);    
  }
}
/* }}} */

/* {{{ proto string explain_opcode(integer opcode)
    get the friendly name for an opcode */
PHP_FUNCTION(explain_opcode) {
  long opcode;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &opcode) == FAILURE) {
    return;
  }

  explain_opcode(opcode, &return_value TSRMLS_CC);
} /* }}} */

/* {{{ proto string explain_optype(integer optype)
    get the friendly name for an optype */
PHP_FUNCTION(explain_optype) {
  long optype;
    
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &optype) == FAILURE) {
    return;
  }

  explain_optype((zend_uchar)optype, &return_value TSRMLS_CC);
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
  PHP_FE(explain,	arginfo_explain)
  PHP_FE(explain_opcode, arginfo_explain_opcode)
  PHP_FE(explain_optype, arginfo_explain_optype)
  PHP_FE_END
};
/* }}} */

/* {{{ MINIT */
static PHP_MINIT_FUNCTION(explain) {
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

/* {{{ explain_module_entry
 */
zend_module_entry explain_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_EXPLAIN_EXTNAME,
  explain_functions,
  PHP_MINIT(explain),
  NULL,
  NULL,
  NULL,
  PHP_MINFO(explain),
  PHP_EXPLAIN_VERSION,
  STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EXPLAIN
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
