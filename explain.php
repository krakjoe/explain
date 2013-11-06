<style type="text/css">
td, th {
    text-align: center;
}
</style>
<table width="100%">
<thead>
    <tr>
        <th>LINE</th>
        <th>OPLINE</th>
        <th>OPCODE</th>
        <th>OP1(TYPE)</th>
        <th>OP1</th>
        <th>OP2(TYPE)</th>
        <th>OP2</th>
        <th>RESULT(TYPE)</th>
        <th>RESULT</th>
    </tr>
</thead>
<tbody>
<?php
$code = <<<HERE
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
?>
<?php foreach ($explained=explain($code, EXPLAIN_STRING) as $opline): ?>
<tr>
    <td><?=$opline["lineno"] ?></td>
    <td><?=$opline["opline"] ?></td>
    <td><?=explain_opcode($opline["opcode"]) ?> (<?=$opline["opcode"] ?>)</td>
    <?php if(isset($opline["op1_type"]) && $opline["op1_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=explain_optype($opline["op1_type"]) ?> (<?=$opline["op1_type"] ?>)</td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["op1_type"]) && $opline["op1_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=@$opline["op1"] ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["op2_type"]) && $opline["op2_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=explain_optype($opline["op2_type"]) ?> (<?=$opline["op2_type"] ?>)</td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["op2_type"]) && $opline["op2_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=@$opline["op2"] ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["result_type"]) && $opline["result_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=explain_optype($opline["result_type"]) ?> (<?=@$opline["result_type"] ?>)</td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["result_type"]) && $opline["result_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=@$opline["result"] ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
</tr>
<?php endforeach; ?>
</tbody>
</table>
