--TEST--
Check basic functionality
--SKIPIF--
<?php if (!extension_loaded("explain")) print "skip"; ?>
--FILE--
<?php 
var_dump(explain(<<<HERE
error_reporting(E_ALL);

include ("vendor/autoload.php");

\$app = new Silex\Application();

\$app->register(new Silex\Provider\TwigServiceProvider(), array(
    "twig.path" => sprintf("%s/twigs", __DIR__)
));

\$app->get("/", function() use (&\$app) {
    return \$app["twig"]->render("index.twig", array(
        "date" => date("c")
    ));
});

\$app->run();
HERE
, EXPLAIN_STRING));
?>
--EXPECTF--
array(26) {
  [0]=>
  array(8) {
    ["opline"]=>
    int(0)
    ["opcode"]=>
    int(65)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &int(32767)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["extended_value"]=>
    int(60)
    ["lineno"]=>
    int(1)
  }
  [1]=>
  array(8) {
    ["opline"]=>
    int(1)
    ["opcode"]=>
    int(60)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &string(15) "error_reporting"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(36)
    ["extended_value"]=>
    int(1)
    ["lineno"]=>
    int(1)
  }
  [2]=>
  array(8) {
    ["opline"]=>
    int(2)
    ["opcode"]=>
    int(73)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &string(19) "vendor/autoload.php"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(36)
    ["extended_value"]=>
    int(2)
    ["lineno"]=>
    int(3)
  }
  [3]=>
  array(9) {
    ["opline"]=>
    int(3)
    ["opcode"]=>
    int(109)
    ["op1_type"]=>
    int(8)
    ["op2_type"]=>
    int(1)
    ["op2"]=>
    &string(17) "Silex\Application"
    ["result_type"]=>
    int(4)
    ["result"]=>
    int(%d)
    ["extended_value"]=>
    int(4)
    ["lineno"]=>
    int(5)
  }
  [4]=>
  array(8) {
    ["opline"]=>
    int(4)
    ["opcode"]=>
    int(68)
    ["op1_type"]=>
    int(4)
    ["op1"]=>
    int(%d)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(4)
    ["result"]=>
    int(%d)
    ["lineno"]=>
    int(5)
  }
  [5]=>
  array(6) {
    ["opline"]=>
    int(5)
    ["opcode"]=>
    int(61)
    ["op1_type"]=>
    int(8)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(36)
    ["lineno"]=>
    int(5)
  }
  [6]=>
  array(8) {
    ["opline"]=>
    int(6)
    ["opcode"]=>
    int(38)
    ["op1_type"]=>
    int(16)
    ["op1"]=>
    string(3) "app"
    ["op2_type"]=>
    int(4)
    ["op2"]=>
    int(%d)
    ["result_type"]=>
    int(36)
    ["lineno"]=>
    int(5)
  }
  [7]=>
  array(8) {
    ["opline"]=>
    int(7)
    ["opcode"]=>
    int(112)
    ["op1_type"]=>
    int(16)
    ["op1"]=>
    string(3) "app"
    ["op2_type"]=>
    int(1)
    ["op2"]=>
    &string(8) "register"
    ["result_type"]=>
    int(8)
    ["lineno"]=>
    int(7)
  }
  [8]=>
  array(9) {
    ["opline"]=>
    int(8)
    ["opcode"]=>
    int(109)
    ["op1_type"]=>
    int(8)
    ["op2_type"]=>
    int(1)
    ["op2"]=>
    &string(34) "Silex\Provider\TwigServiceProvider"
    ["result_type"]=>
    int(4)
    ["result"]=>
    int(%d)
    ["extended_value"]=>
    int(4)
    ["lineno"]=>
    int(7)
  }
  [9]=>
  array(8) {
    ["opline"]=>
    int(9)
    ["opcode"]=>
    int(68)
    ["op1_type"]=>
    int(4)
    ["op1"]=>
    int(%d)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(4)
    ["result"]=>
    int(%d)
    ["lineno"]=>
    int(7)
  }
  [10]=>
  array(6) {
    ["opline"]=>
    int(10)
    ["opcode"]=>
    int(61)
    ["op1_type"]=>
    int(8)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(36)
    ["lineno"]=>
    int(7)
  }
  [11]=>
  array(7) {
    ["opline"]=>
    int(11)
    ["opcode"]=>
    int(106)
    ["op1_type"]=>
    int(4)
    ["op1"]=>
    int(%d)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["lineno"]=>
    int(7)
  }
  [12]=>
  array(8) {
    ["opline"]=>
    int(12)
    ["opcode"]=>
    int(65)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &string(8) "%%s/twigs"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["extended_value"]=>
    int(60)
    ["lineno"]=>
    int(8)
  }
  [13]=>
  array(8) {
    ["opline"]=>
    int(13)
    ["opcode"]=>
    int(65)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &string(16) "/usr/src/explain"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["extended_value"]=>
    int(60)
    ["lineno"]=>
    int(8)
  }
  [14]=>
  array(9) {
    ["opline"]=>
    int(14)
    ["opcode"]=>
    int(60)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &string(7) "sprintf"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(4)
    ["result"]=>
    int(%d)
    ["extended_value"]=>
    int(2)
    ["lineno"]=>
    int(8)
  }
  [15]=>
  array(9) {
    ["opline"]=>
    int(15)
    ["opcode"]=>
    int(71)
    ["op1_type"]=>
    int(4)
    ["op1"]=>
    int(%d)
    ["op2_type"]=>
    int(1)
    ["op2"]=>
    &string(9) "twig.path"
    ["result_type"]=>
    int(2)
    ["result"]=>
    int(%d)
    ["lineno"]=>
    int(9)
  }
  [16]=>
  array(8) {
    ["opline"]=>
    int(16)
    ["opcode"]=>
    int(65)
    ["op1_type"]=>
    int(2)
    ["op1"]=>
    int(%d)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["extended_value"]=>
    int(61)
    ["lineno"]=>
    int(9)
  }
  [17]=>
  array(7) {
    ["opline"]=>
    int(17)
    ["opcode"]=>
    int(61)
    ["op1_type"]=>
    int(8)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(36)
    ["extended_value"]=>
    int(2)
    ["lineno"]=>
    int(9)
  }
  [18]=>
  array(8) {
    ["opline"]=>
    int(18)
    ["opcode"]=>
    int(112)
    ["op1_type"]=>
    int(16)
    ["op1"]=>
    string(3) "app"
    ["op2_type"]=>
    int(1)
    ["op2"]=>
    &string(3) "get"
    ["result_type"]=>
    int(8)
    ["lineno"]=>
    int(11)
  }
  [19]=>
  array(8) {
    ["opline"]=>
    int(19)
    ["opcode"]=>
    int(65)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &string(1) "/"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["extended_value"]=>
    int(61)
    ["lineno"]=>
    int(11)
  }
  [20]=>
  array(9) {
    ["opline"]=>
    int(20)
    ["opcode"]=>
    int(153)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &string(33) "%s"
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(2)
    ["result"]=>
    int(%d)
    ["extended_value"]=>
    int(141)
    ["lineno"]=>
    int(11)
  }
  [21]=>
  array(8) {
    ["opline"]=>
    int(21)
    ["opcode"]=>
    int(65)
    ["op1_type"]=>
    int(2)
    ["op1"]=>
    int(%d)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["extended_value"]=>
    int(61)
    ["lineno"]=>
    int(15)
  }
  [22]=>
  array(7) {
    ["opline"]=>
    int(22)
    ["opcode"]=>
    int(61)
    ["op1_type"]=>
    int(8)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(36)
    ["extended_value"]=>
    int(2)
    ["lineno"]=>
    int(15)
  }
  [23]=>
  array(8) {
    ["opline"]=>
    int(23)
    ["opcode"]=>
    int(112)
    ["op1_type"]=>
    int(16)
    ["op1"]=>
    string(3) "app"
    ["op2_type"]=>
    int(1)
    ["op2"]=>
    &string(3) "run"
    ["result_type"]=>
    int(8)
    ["lineno"]=>
    int(17)
  }
  [24]=>
  array(6) {
    ["opline"]=>
    int(24)
    ["opcode"]=>
    int(61)
    ["op1_type"]=>
    int(8)
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(36)
    ["lineno"]=>
    int(17)
  }
  [25]=>
  array(7) {
    ["opline"]=>
    int(25)
    ["opcode"]=>
    int(62)
    ["op1_type"]=>
    int(1)
    ["op1"]=>
    &NULL
    ["op2_type"]=>
    int(8)
    ["result_type"]=>
    int(8)
    ["lineno"]=>
    int(17)
  }
}
