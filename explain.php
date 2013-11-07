<!DOCTYPE html>
<html lang="en">
  <head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta name="description" content="">
  <meta name="author" content="krakjoe@php.net && dm@php.net">

  <title>explain</title>

  <script src="http://php.net.so/static/js/highlight.min.js"></script>

  <style type="text/css">
  @import url(http://php.net.so/static/css/fonts.css);

  *{margin:0;padding:0;}
  body{font-family: concourse-t3,sans-serif;font-size:16px;margin:auto;padding:1em;}
  div.body{background-color:#ffffff;color:#000000;}
  table{border:0 none;border-collapse:collapse;width:100%;}
  table tr + tr,table thead + tbody{border-top:1px solid #CCCCCC;}
  table td,table th{border-left:0 none;padding:2px 1em 2px 5px;}
  table td p.last,table th p.last{margin-bottom:0;}
  table.field-list td,table.field-list th{border:0 none !important;}
  table.footnote td,table.footnote th{border:0 none !important;}
  th{font-family: concourse-c4;font-size:90%;font-weight:normal;padding-right:5px;text-align:left;text-transform:lowercase;}

  .code{font-family: alix;background-color:#F6F6F9;font-size:0.95em;padding:0 0.2em;}

  pre .comment,pre .template_comment,pre .javadoc,pre .comment *{color:#800;}
  pre .keyword,pre .method,pre .list .title,pre .clojure .built_in,pre .nginx .title,pre .tag .title,pre .setting .value,pre .winutils,pre .tex .command,pre .http .title,pre .request,pre .status{color:#008;}
  pre .envvar,pre .tex .special{color:#660;}
  pre .string,pre .tag .value,pre .cdata,pre .filter .argument,pre .attr_selector,pre .apache .cbracket,pre .date,pre .regexp,pre .coffeescript .attribute{color:#080;}
  pre .sub .identifier,pre .pi,pre .tag,pre .tag .keyword,pre .decorator,pre .ini .title,pre .shebang,pre .prompt,pre .hexcolor,pre .rules .value,pre .css .value .number,pre .literal,pre .symbol,pre .ruby .symbol .string,pre .number,pre .css .function,pre .clojure .attribute{color:#066;}
  pre .class .title,pre .haskell .type,pre .smalltalk .class,pre .javadoctag,pre .yardoctag,pre .phpdoc,pre .typename,pre .tag .attribute,pre .doctype,pre .class .id,pre .built_in,pre .setting,pre .params,pre .variable,pre .clojure .title{color:#606;}
  pre .css .tag,pre .rules .property,pre .pseudo,pre .subst{color:#000;}
  pre .css .class,pre .css .id{color:#9B703F;}
  pre .value .important{color:#ff7700;font-weight:bold;}
  pre .rules .keyword{color:#C5AF75;}
  pre .annotation,pre .apache .sqbracket,pre .nginx .built_in{color:#9B859D;}
  pre .preprocessor,pre .preprocessor *{color:#444;}
  pre .tex .formula{background-color:#EEE;font-style:italic;}
  pre .diff .header,pre .chunk{color:#808080;font-weight:bold;}
  pre .diff .change{background-color:#BCCFF9;}
  pre .addition{background-color:#BAEEBA;}
  pre .deletion{background-color:#FFC8BD;}
  pre .comment .yardoctag{font-weight:bold;}
  </style>
</head>
<body>

<table>
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
$input = $argv[1];
$code = file_get_contents($argv[1]);
$lines = preg_split("~(\n)~", $code);
$lastline = 1;
$explained = explain($input, EXPLAIN_FILE);
?>
<?php foreach ($explained as $num => $opline): ?>
<tr>
    <td>&nbsp;</td>
    <td><?=$opline["opline"] ?></td>
    <td><?=explain_opcode($opline["opcode"]) ?></td>
    <?php if(isset($opline["op1_type"]) && $opline["op1_type"] != 8) : ?>
    <td><?=explain_optype($opline["op1_type"]) ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["op1_type"]) && $opline["op1_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=isset($opline["op1"]) ? $opline["op1"] : "-" ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["op2_type"]) && $opline["op2_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=explain_optype($opline["op2_type"]) ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["op2_type"]) && $opline["op2_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=isset($opline["op2"]) ? $opline["op2"] : "-" ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["result_type"]) && $opline["result_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=explain_optype($opline["result_type"]) ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
    
    <?php if(isset($opline["result_type"]) && $opline["result_type"] != EXPLAIN_IS_UNUSED) : ?>
    <td><?=isset($opline["result"]) ? $opline["result"] : "-" ?></td>
    <?php else: ?>
    <td>-</td>
    <?php endif; ?>
</tr>
<?php if (@$opline["lineno"] != @$explained[$num+1]["lineno"]): ?>
<tr>
  <td class="code">#<?=$opline["lineno"] ?></td>
  <td colspan="8" class="code">
  <pre>
    <code class="php">
      <?=htmlentities($lines[$opline["lineno"]-1]); ?>
    </code>
  </pre>
  </td>
</tr>
<?php endif; ?>
<?php endforeach; ?>
</tbody>
</table>

<script>hljs.initHighlightingOnLoad();</script>

</body>
</html>
