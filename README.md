explain all the things !!
=========================

```php
/*
* explain some code
* @param code the file or code to explain
* @param type the type of $code EXPLAIN_FILE or EXPLAIN_STRING
* @param classes array of classes created by compilation of code
* @param functions array of functions created by compilation of code
* @return array
*/
function explain($code, $type = EXPLAIN_FILE, &$classes = array(), &$functions = array());
/*
* explain_opcode
* @param opcode the opcode
* @return string
*/
function explain_opcode($opcode);
/*
* explain_optype
* @param optype the optype
* @return string
*/
function explain_optype($optype);
```

Execution
=========

```
php explain.php /path/to/file.php > output.html
```

Executing the above command will output the same format as in the preview displayed here ...

```
php explain.php /path/to/files > output.html
```

Executing the command above will recursively scan the path for PHP files ...

**note: crank up the memory limit and keep the numbers of files down, for now**

Preview
=======

<img src="https://raw.github.com/krakjoe/explain/master/explain.png" alt="screenshot"/>

[![Build Status](https://travis-ci.org/krakjoe/explain.png?branch=master)](https://travis-ci.org/krakjoe/explain)

See *explain.php* for helpingz ... 
