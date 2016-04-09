--TEST--
Check basic functionality (PHP >= 7.0.0)
--SKIPIF--
<?php
    if (!extension_loaded("explain")) print "skip";
    if (version_compare(PHP_VERSION, '7.0.0') < 0) {
        // skip if PHP < 7.0.0
        print "skip";
    }
?>
--FILE--
<?php
var_dump(explain(<<<HERE
echo "Hello World";
HERE
, EXPLAIN_STRING));
?>
--EXPECTF--
array(2) {
  [0]=>
  array(7) {
    ["opline"]=>
    int(0)
    ["opcode"]=>
    int(40)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    string(11) "Hello World"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["lineno"]=>
    int(1)
  }
  [1]=>
  array(8) {
    ["opline"]=>
    int(1)
    ["opcode"]=>
    int(62)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    NULL
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["extended_value"]=>
    int(4294967295)
    ["lineno"]=>
    int(1)
  }
}
