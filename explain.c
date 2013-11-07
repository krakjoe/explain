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

static explain_opcode_t opcodes[]= {
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_ADD),
	EXPLAIN_OPCODE_NAME(ZEND_SUB),
	EXPLAIN_OPCODE_NAME(ZEND_MUL),
	EXPLAIN_OPCODE_NAME(ZEND_DIV),
	EXPLAIN_OPCODE_NAME(ZEND_MOD),
	EXPLAIN_OPCODE_NAME(ZEND_SL),
	EXPLAIN_OPCODE_NAME(ZEND_SR),
	EXPLAIN_OPCODE_NAME(ZEND_CONCAT),
	EXPLAIN_OPCODE_NAME(ZEND_BW_OR),
	EXPLAIN_OPCODE_NAME(ZEND_BW_AND),
	EXPLAIN_OPCODE_NAME(ZEND_BW_XOR),
	EXPLAIN_OPCODE_NAME(ZEND_BW_NOT),
	EXPLAIN_OPCODE_NAME(ZEND_BOOL_NOT),
	EXPLAIN_OPCODE_NAME(ZEND_BOOL_XOR),
	EXPLAIN_OPCODE_NAME(ZEND_IS_IDENTICAL),
	EXPLAIN_OPCODE_NAME(ZEND_IS_NOT_IDENTICAL),
	EXPLAIN_OPCODE_NAME(ZEND_IS_EQUAL),
	EXPLAIN_OPCODE_NAME(ZEND_IS_NOT_EQUAL),
	EXPLAIN_OPCODE_NAME(ZEND_IS_SMALLER),
	EXPLAIN_OPCODE_NAME(ZEND_IS_SMALLER_OR_EQUAL),
	EXPLAIN_OPCODE_NAME(ZEND_CAST),
	EXPLAIN_OPCODE_NAME(ZEND_QM_ASSIGN),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_ADD),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_SUB),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_MUL),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_DIV),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_MOD),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_SL),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_SR),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_CONCAT),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_BW_OR),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_BW_AND),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_BW_XOR),
	EXPLAIN_OPCODE_NAME(ZEND_PRE_INC),
	EXPLAIN_OPCODE_NAME(ZEND_PRE_DEC),
	EXPLAIN_OPCODE_NAME(ZEND_POST_INC),
	EXPLAIN_OPCODE_NAME(ZEND_POST_DEC),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_REF),
	EXPLAIN_OPCODE_NAME(ZEND_ECHO),
	EXPLAIN_OPCODE_NAME(ZEND_PRINT),
	EXPLAIN_OPCODE_NAME(ZEND_JMP),
	EXPLAIN_OPCODE_NAME(ZEND_JMPZ),
	EXPLAIN_OPCODE_NAME(ZEND_JMPNZ),
	EXPLAIN_OPCODE_NAME(ZEND_JMPZNZ),
	EXPLAIN_OPCODE_NAME(ZEND_JMPZ_EX),
	EXPLAIN_OPCODE_NAME(ZEND_JMPNZ_EX),
	EXPLAIN_OPCODE_NAME(ZEND_CASE),
	EXPLAIN_OPCODE_NAME(ZEND_SWITCH_FREE),
	EXPLAIN_OPCODE_NAME(ZEND_BRK),
	EXPLAIN_OPCODE_NAME(ZEND_CONT),
	EXPLAIN_OPCODE_NAME(ZEND_BOOL),
	EXPLAIN_OPCODE_NAME(ZEND_INIT_STRING),
	EXPLAIN_OPCODE_NAME(ZEND_ADD_CHAR),
	EXPLAIN_OPCODE_NAME(ZEND_ADD_STRING),
	EXPLAIN_OPCODE_NAME(ZEND_ADD_VAR),
	EXPLAIN_OPCODE_NAME(ZEND_BEGIN_SILENCE),
	EXPLAIN_OPCODE_NAME(ZEND_END_SILENCE),
	EXPLAIN_OPCODE_NAME(ZEND_INIT_FCALL_BY_NAME),
	EXPLAIN_OPCODE_NAME(ZEND_DO_FCALL),
	EXPLAIN_OPCODE_NAME(ZEND_DO_FCALL_BY_NAME),
	EXPLAIN_OPCODE_NAME(ZEND_RETURN),
	EXPLAIN_OPCODE_NAME(ZEND_RECV),
	EXPLAIN_OPCODE_NAME(ZEND_RECV_INIT),
	EXPLAIN_OPCODE_NAME(ZEND_SEND_VAL),
	EXPLAIN_OPCODE_NAME(ZEND_SEND_VAR),
	EXPLAIN_OPCODE_NAME(ZEND_SEND_REF),
	EXPLAIN_OPCODE_NAME(ZEND_NEW),
	EXPLAIN_OPCODE_NAME(ZEND_INIT_NS_FCALL_BY_NAME),
	EXPLAIN_OPCODE_NAME(ZEND_FREE),
	EXPLAIN_OPCODE_NAME(ZEND_INIT_ARRAY),
	EXPLAIN_OPCODE_NAME(ZEND_ADD_ARRAY_ELEMENT),
	EXPLAIN_OPCODE_NAME(ZEND_INCLUDE_OR_EVAL),
	EXPLAIN_OPCODE_NAME(ZEND_UNSET_VAR),
	EXPLAIN_OPCODE_NAME(ZEND_UNSET_DIM),
	EXPLAIN_OPCODE_NAME(ZEND_UNSET_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_FE_RESET),
	EXPLAIN_OPCODE_NAME(ZEND_FE_FETCH),
	EXPLAIN_OPCODE_NAME(ZEND_EXIT),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_R),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_DIM_R),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_OBJ_R),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_W),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_DIM_W),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_OBJ_W),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_RW),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_DIM_RW),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_OBJ_RW),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_IS),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_DIM_IS),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_OBJ_IS),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_FUNC_ARG),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_DIM_FUNC_ARG),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_OBJ_FUNC_ARG),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_UNSET),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_DIM_UNSET),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_OBJ_UNSET),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_DIM_TMP_VAR),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_CONSTANT),
	EXPLAIN_OPCODE_NAME(ZEND_GOTO),
	EXPLAIN_OPCODE_NAME(ZEND_EXT_STMT),
	EXPLAIN_OPCODE_NAME(ZEND_EXT_FCALL_BEGIN),
	EXPLAIN_OPCODE_NAME(ZEND_EXT_FCALL_END),
	EXPLAIN_OPCODE_NAME(ZEND_EXT_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_TICKS),
	EXPLAIN_OPCODE_NAME(ZEND_SEND_VAR_NO_REF),
	EXPLAIN_OPCODE_NAME(ZEND_CATCH),
	EXPLAIN_OPCODE_NAME(ZEND_THROW),
	EXPLAIN_OPCODE_NAME(ZEND_FETCH_CLASS),
	EXPLAIN_OPCODE_NAME(ZEND_CLONE),
	EXPLAIN_OPCODE_NAME(ZEND_RETURN_BY_REF),
	EXPLAIN_OPCODE_NAME(ZEND_INIT_METHOD_CALL),
	EXPLAIN_OPCODE_NAME(ZEND_INIT_STATIC_METHOD_CALL),
	EXPLAIN_OPCODE_NAME(ZEND_ISSET_ISEMPTY_VAR),
	EXPLAIN_OPCODE_NAME(ZEND_ISSET_ISEMPTY_DIM_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_PRE_INC_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_PRE_DEC_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_POST_INC_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_POST_DEC_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_OP_DATA),
	EXPLAIN_OPCODE_NAME(ZEND_INSTANCEOF),
	EXPLAIN_OPCODE_NAME(ZEND_DECLARE_CLASS),
	EXPLAIN_OPCODE_NAME(ZEND_DECLARE_INHERITED_CLASS),
	EXPLAIN_OPCODE_NAME(ZEND_DECLARE_FUNCTION),
	EXPLAIN_OPCODE_NAME(ZEND_RAISE_ABSTRACT_ERROR),
	EXPLAIN_OPCODE_NAME(ZEND_DECLARE_CONST),
	EXPLAIN_OPCODE_NAME(ZEND_ADD_INTERFACE),
	EXPLAIN_OPCODE_NAME(ZEND_DECLARE_INHERITED_CLASS_DELAYED),
	EXPLAIN_OPCODE_NAME(ZEND_VERIFY_ABSTRACT_CLASS),
	EXPLAIN_OPCODE_NAME(ZEND_ASSIGN_DIM),
	EXPLAIN_OPCODE_NAME(ZEND_ISSET_ISEMPTY_PROP_OBJ),
	EXPLAIN_OPCODE_NAME(ZEND_HANDLE_EXCEPTION),
	EXPLAIN_OPCODE_NAME(ZEND_USER_OPCODE),
	EXPLAIN_OPCODE_NAME(ZEND_NOP),
	EXPLAIN_OPCODE_NAME(ZEND_JMP_SET),
	EXPLAIN_OPCODE_NAME(ZEND_DECLARE_LAMBDA_FUNCTION),
	EXPLAIN_OPCODE_NAME(ZEND_ADD_TRAIT),
	EXPLAIN_OPCODE_NAME(ZEND_BIND_TRAITS),
	EXPLAIN_OPCODE_NAME(ZEND_SEPARATE),
	EXPLAIN_OPCODE_NAME(ZEND_QM_ASSIGN_VAR),
	EXPLAIN_OPCODE_NAME(ZEND_JMP_SET_VAR),
#ifdef ZEND_DISCARD_EXCEPTION
	EXPLAIN_OPCODE_NAME(ZEND_DISCARD_EXCEPTION),
#else
  EXPLAIN_OPCODE_NAME(ZEND_NOP),
#endif
#ifdef ZEND_GENERATOR_YIELD
  EXPLAIN_OPCODE_NAME(ZEND_YIELD),
#else
  EXPLAIN_OPCODE_NAME(ZEND_NOP),
#endif
#ifdef ZEND_GENERATOR_RETURN
	EXPLAIN_OPCODE_NAME(ZEND_GENERATOR_RETURN),
#else
  EXPLAIN_OPCODE_NAME(ZEND_NOP),
#endif
#ifdef ZEND_FAST_CALL
	EXPLAIN_OPCODE_NAME(ZEND_FAST_CALL),
#else
  EXPLAIN_OPCODE_NAME(ZEND_NOP),
#endif
#ifdef ZEND_FAST_RET
	EXPLAIN_OPCODE_NAME(ZEND_FAST_RET),
#else
  EXPLAIN_OPCODE_NAME(ZEND_NOP),
#endif
#ifdef ZEND_RECV_VARIADIC
	EXPLAIN_OPCODE_NAME(ZEND_RECV_VARIADIC),
#else
  EXPLAIN_OPCODE_NAME(ZEND_NOP),
#endif
	{NULL, 0, 0}
};

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

static inline const char * explain_optype(zend_uint type, zval **return_value_ptr TSRMLS_DC) { /* {{{ */
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
      ZVAL_STRINGL(*return_value_ptr, "unknown", strlen("unknown"), 1); break; 
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

/* {{{ proto array explain(string code [, long options, array &classes, array &functions])
   explain some code */
PHP_FUNCTION(explain)
{
  zval *code = NULL, 
       *classes = NULL, 
       *functions = NULL;
  zend_ulong options = EXPLAIN_FILE;

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
          ops = zend_compile_file(
            &fh, ZEND_INCLUDE TSRMLS_CC);
          zend_destroy_file_handle(&fh TSRMLS_CC);
        } else {
          zend_file_handle_dtor(&fh TSRMLS_CC);
        }
      } else {
        RETURN_FALSE;
      }
    } else if (options & EXPLAIN_STRING) {
      ops = zend_compile_string(code, "explained" TSRMLS_CC);
    } else {
      zend_error(E_WARNING, "invalid options passed to explain (%d), please see documentation", options);
    }

    array_init(return_value);
    
    explain_op_array(ops, return_value TSRMLS_CC);
    
    if (classes) {
      HashPosition position[2];
      zend_class_entry **ppce, *pce;
      
      array_init(classes);
      
      for (zend_hash_internal_pointer_reset_ex(CG(class_table), &position[0]);
          zend_hash_get_current_data_ex(CG(class_table), (void**) &ppce, &position[0]) == SUCCESS && (pce = *ppce);
          zend_hash_move_forward_ex(CG(class_table), &position[0])) {
          
          if (pce->type  == ZEND_USER_CLASS) {
          
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
    
    destroy_op_array(ops TSRMLS_CC);
    efree(ops);
    
    if (functions) {
      HashPosition position;
      zend_function *pfe;
      
      array_init(functions);
      
      for (zend_hash_internal_pointer_reset_ex(CG(function_table), &position);
           zend_hash_get_current_data_ex(CG(function_table), (void**) &pfe, &position) == SUCCESS;
           zend_hash_move_forward_ex(CG(function_table), &position)) {
           if (pfe->common.type == ZEND_USER_FUNCTION) {
             zval *zfe;
           
             ALLOC_INIT_ZVAL(zfe);
             
             array_init(zfe);
             
             explain_op_array(&pfe->op_array, zfe TSRMLS_CC);
             
             add_assoc_zval_ex(functions, pfe->common.function_name, strlen(pfe->common.function_name)+1, zfe);
           }
      }
    }    
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
