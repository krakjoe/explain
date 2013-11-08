<?php
$input = @$argv[1];
$lastline = 1;
$classes = array();
$functions = array();

if ($input && ($code = @file_get_contents($input))) {
  $lines = preg_split("~(\n)~", $code);  
  $explained = explain(
    $input, EXPLAIN_FILE, $classes, $functions);
} else $explained = false;

function table($id, $explained, $lines) {
  ?>
  <?php if ($id == "main"): ?>
  <table id="table-main" >
  <?php else: ?>
  <table id="<?=sprintf("table-%s", md5($id)) ?>" style="display:none;">
  <?php endif; ?>
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
    <?php foreach ($explained as $num => $opline): ?>
    <?php if (@$opline["lineno"] != @$explained[$num+1]["lineno"]): ?>
    <?php   if ($lines[$opline["lineno"]-1]): ?>
    <tr>
      <td class="code">#<?=$opline["lineno"] ?></td>
      <td colspan="8" class="code">
      <pre>
        <code class="php">
          <?=htmlentities(rtrim($lines[$opline["lineno"]-1])); ?>
        </code>
      </pre>
      </td>
    </tr>
    <?php   endif; ?>
    <?php endif; ?>
    <tr>
        <td>&nbsp;</td>
        <td><?=$opline["opline"] ?></td>
        <td><?=explain_opcode($opline["opcode"]) ?></td>
        <?php
        foreach (array("op1", "op2", "result") as $op) {
          if (isset($opline["{$op}_type"]) && 
              $opline["{$op}_type"] != EXPLAIN_IS_UNUSED) {
              printf("<td>%s</td>", explain_optype($opline["{$op}_type"]));
          } else printf("<td>-</td>");
          if (isset($opline[$op])) {
            printf("<td>%s</td>", htmlentities(rtrim($opline[$op])));
          } else printf("<td>-</td>");
        }
        ?>
    </tr>
    <?php endforeach; ?>
    </tbody>
    </table>
  <?php
}
?>
<!DOCTYPE html>
<html lang="en">
  <head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta name="description" content="explain: <?=$input ?>">
  <meta name="author" content="krakjoe@php.net && dm@php.net">

  <title>Explain: <?=$input ?></title>

  <script type="text/javascript" src="assets/js/highlight.min.js"></script>
  <script type="text/javascript" src="assets/js/jquery.js"></script>
  <script type="text/javascript" src="assets/js/jquery.cookie.js"></script>
  <script type="text/javascript" src="assets/js/jquery.hotkeys.js"></script>
  <script type="text/javascript" src="assets/js/jquery.jstree.js"></script>
  <script type="text/javascript" src="assets/js/default.js"></script>
  
  <link type="text/css" media="all" rel="stylesheet" href="assets/css/fonts.css" />
  <link type="text/css" media="all" rel="stylesheet" href="assets/css/default.css" />
  <link type="text/css" media="all" rel="stylesheet" href="assets/css/highlight.css" />
</head>
<body>
<div id="container">
  <div id="left">
    <div id="tree" class="jstree">
      <ul>
        <li id="main"><a href="#">{main}</a></li>
        <?php if ($classes): ?>
        <li id="classes"><a href="#">Classes</a>
          <ul>
            <?php foreach ($classes as $class => $methods): ?>
            <li><a href="#"><?=$class; ?></a>
              <ul>
                <?php foreach($methods as $method => $opcodes): ?>
                <li id="<?=md5("{$class}-{$method}") ?>"><a href="#"><?=$method ?></a></li>
                <?php endforeach; ?>
              </ul>
            </li>
            <?php endforeach; ?>
          </ul>
        </li>
        <?php endif; ?>
        
        <?php if ($functions): ?>
        <li id="functions"><a href="#">Functions</a>
          <ul>
          <?php   foreach($functions as $function => $opcodes): ?>
            <li id="<?=md5($function) ?>"><a href="#"><?=$function ?></a></li>
          <?php   endforeach; ?>
          </ul>
        </li>
        <?php endif; ?>
        </ul>
      </ul>
    </div>
  </div>
  <div id="right">
  <?php
  if ($explained) {
    table("main", $explained, $lines);
  
    if ($classes): 
       foreach ($classes as $class => $methods): 
         foreach ($methods as $method => $opcodes):
           table("{$class}-{$method}", $opcodes, $lines);
         endforeach;
       endforeach;
    endif;
    
    if ($functions):
      foreach ($functions as $function => $opcodes):
        table($function, $opcodes, $lines);
      endforeach;
    endif;
  }
  ?>
  </div>
</div>

<script>hljs.initHighlightingOnLoad();</script>

</body>
</html>
