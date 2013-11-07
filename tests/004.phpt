--TEST--
Check class map
--SKIPIF--
<?php if (!extension_loaded("explain")) print "skip"; ?>
--FILE--
<?php 
var_dump(count(explain(<<<HERE
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
, EXPLAIN_STRING, $classes, $functions)));
var_dump(count($classes), count($functions));
?>
--EXPECT--
int(26)
int(0)
int(1)
