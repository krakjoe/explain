--TEST--
Check basic functionality
--SKIPIF--
<?php if (!extension_loaded("explain")) print "skip"; ?>
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
  array(7) {
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
    ["lineno"]=>
    int(1)
  }
}

