<style type="text/css">
td, th {
    text-align: center;
}
</style>
<table width="100%">
<thead>
    <tr>
        <th>LINE</th>
        <th>OPCODE</th>
        <th>OP1(TYPE)</th>
        <th>OP2(TYPE)</th>
        <th>OP1</th>
        <th>OP2</th>
        <th>RESULT(TYPE)</th>
        <th>RESULT</th>
    </tr>
</thead>
<tbody>
<?php
$code = <<<HERE
echo "Hello World";
foreach (\$vars as \$var) {
    echo \$var;
    echo "\n";
}
new stdClass();
HERE
?>
<?php foreach ($explained=explain($code, EXPLAIN_STRING) as $opline): ?>
<tr>
    <td><?=$opline["opline"] ?></td>
    <td><?=explain_opcode($opline["opcode"]) ?> (<?=$opline["opcode"] ?>)</td>
    <?php if(isset($opline["op1_type"]) && $opline["op1_type"] != 8) : ?>
    <td><?=explain_optype($opline["op1_type"]) ?> (<?=$opline["op1_type"] ?>)</td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    <?php if(isset($opline["op2_type"]) && $opline["op2_type"] != 8) : ?>
    <td><?=explain_optype($opline["op2_type"]) ?> (<?=$opline["op2_type"] ?>)</td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    <?php if(isset($opline["op1_type"]) && $opline["op1_type"] != 8) : ?>
    <td><?=@$opline["op1"] ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    <?php if(isset($opline["op2_type"]) && $opline["op2_type"] != 8) : ?>
    <td><?=@$opline["op2"] ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    <?php if(isset($opline["result_type"]) && $opline["result_type"] != 8) : ?>
    <td><?=explain_optype($opline["result_type"]) ?> (<?=@$opline["result_type"] ?>)</td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    <?php if(isset($opline["result_type"]) && $opline["result_type"] != 8) : ?>
    <td><?=@$opline["result"] ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
</tr>
<?php endforeach; ?>
</tbody>
</table>
