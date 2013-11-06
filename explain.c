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
#define EXPLAIN_QUIET  0x00000100

#define EXPLAIN_OPCODE_NAME(c) \
    {#c, sizeof(#c), c}

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
    EXPLAIN_OPCODE_NAME(ZEND_JMP_SET),
    EXPLAIN_OPCODE_NAME(ZEND_DECLARE_LAMBDA_FUNCTION),
    EXPLAIN_OPCODE_NAME(ZEND_ADD_TRAIT),
    EXPLAIN_OPCODE_NAME(ZEND_BIND_TRAITS),
    EXPLAIN_OPCODE_NAME(ZEND_SEPARATE),
    EXPLAIN_OPCODE_NAME(ZEND_QM_ASSIGN_VAR),
    EXPLAIN_OPCODE_NAME(ZEND_JMP_SET_VAR),
    EXPLAIN_OPCODE_NAME(ZEND_DISCARD_EXCEPTION),
    EXPLAIN_OPCODE_NAME(ZEND_YIELD),
    EXPLAIN_OPCODE_NAME(ZEND_GENERATOR_RETURN),
    EXPLAIN_OPCODE_NAME(ZEND_FAST_CALL),
    EXPLAIN_OPCODE_NAME(ZEND_FAST_RET),
#ifdef ZEND_RECV_VARIADIC
    EXPLAIN_OPCODE_NAME(ZEND_RECV_VARIADIC),
#endif
    {NULL, 0, 0}
};

static inline const void explain_opcode(long opcode, zval **return_value_ptr TSRMLS_DC) {
    explain_opcode_t decode = opcodes[opcode];
    
    if (decode.opcode == opcode) {
        ZVAL_STRINGL(
            *return_value_ptr, decode.name, decode.name_len, 1);
    } else ZVAL_STRINGL(*return_value_ptr, "unknown", sizeof("unknown"), 1);
}

static inline void explain_zend_op(zend_op_array *ops, znode_op *op, zend_uint type, const char *name, size_t name_len, zval **return_value_ptr TSRMLS_DC) {
    if (type == IS_UNUSED)
        return;
        
    switch (type) {
        case IS_CV: {
            add_assoc_stringl_ex(*return_value_ptr, name, name_len, (char*) ops->vars[op->var].name, ops->vars[op->var].name_len, 1);
        } break;
        
        case IS_VAR:
        case IS_TMP_VAR: {
            add_assoc_long_ex(*return_value_ptr, name, name_len, ops->vars - op->var);
        } break;
        
        case IS_CONST: {
            zval  *copy;
            
            ALLOC_ZVAL(copy);
            *copy = (op->literal->constant);
            
            add_assoc_zval_ex(*return_value_ptr, name, name_len, copy);
        } break;
    }
}

static inline const char * explain_optype(zend_uint type, zval **return_value_ptr TSRMLS_DC) {
    switch (type) {
        case IS_CV: ZVAL_STRINGL(*return_value_ptr, "IS_CV", sizeof("IS_CV"), 1); break;
        case IS_TMP_VAR: ZVAL_STRINGL(*return_value_ptr, "IS_TMP_VAR", sizeof("IS_TMP_VAR"), 1); break;
        case IS_VAR: ZVAL_STRINGL(*return_value_ptr, "IS_VAR", sizeof("IS_VAR"), 1); break;
        case IS_CONST: ZVAL_STRINGL(*return_value_ptr, "IS_CONST", sizeof("IS_CONST"), 1); break;
        case IS_UNUSED: ZVAL_STRINGL(*return_value_ptr, "IS_UNUSED", sizeof("IS_UNUSED"), 1); break;
        default:
                ZVAL_STRINGL(*return_value_ptr, "unknown", sizeof("unknown"), 1); break;
    }
}

/* {{{ proto array explain(string file [, long options])
   Explain a file */
PHP_FUNCTION(explain)
{
	zval *code;
    zend_ulong options = EXPLAIN_FILE;
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &code, &options) == FAILURE) {
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
            zval_ptr_dtor(&code);
            zend_error(E_WARNING, "invalid options passed to explain (%d), please see documentation", options);
        }
        
        if (ops) {
            zend_uint next = 0;

            array_init(return_value);

            do {
                zval *zopline;
                
                MAKE_STD_ZVAL(zopline);
                
                array_init(zopline);
                {
                    zend_op *opline = &ops->opcodes[next];

                    add_assoc_long_ex(
                        zopline, "opcode", sizeof("opcode"), opline->opcode);
                    add_assoc_long_ex(
                        zopline, "op1_type", sizeof("op1_type"), opline->op1_type);
                    add_assoc_long_ex(
                        zopline, "op2_type", sizeof("op2_type"), opline->op2_type);
                    add_assoc_long_ex(
                        zopline, "extended_value", sizeof("extended_value"), opline->extended_value);
                    add_assoc_long_ex(
                        zopline, "result_type", sizeof("result_type"), opline->result_type);
                    add_assoc_long_ex(
                        zopline, "lineno", sizeof("lineno"), opline->lineno);
                    
                    explain_zend_op(ops, &opline->op1, opline->op1_type, "op1", sizeof("op1"), &zopline TSRMLS_CC);
                    explain_zend_op(ops, &opline->op2, opline->op2_type, "op2", sizeof("op2"), &zopline TSRMLS_CC);
                }
                
                add_next_index_zval(return_value, zopline);
            } while (++next < ops->last);

            destroy_op_array(ops TSRMLS_CC);
            efree(ops);
        } else {
            RETURN_FALSE;
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
    ZEND_ARG_INFO(0, file)
    ZEND_ARG_INFO(0, options)
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
    REGISTER_LONG_CONSTANT("EXPLAIN_STRING", EXPLAIN_STRING, CONST_CS | CONST_PERSISTENT TSRMLS_CC);
    REGISTER_LONG_CONSTANT("EXPLAIN_FILE", EXPLAIN_FILE, CONST_CS | CONST_PERSISTENT TSRMLS_CC);
    REGISTER_LONG_CONSTANT("EXPLAIN_QUIET", EXPLAIN_QUIET, CONST_CS | CONST_PERSISTENT TSRMLS_CC);
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
