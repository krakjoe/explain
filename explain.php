<?php
$input = @$argv[1];
$lastline = 1;
$classes = array();
$functions = array();
$main = false;

$scanpath = function($path) use (&$scanpath) {
  $files = array();
  if (is_dir($path)) {
    foreach (scandir($path) as $file) {
      if ($file != "." && $file != "..") {
        $scan = "{$path}/{$file}";
        if (is_dir($scan)) {
          $files = array_merge(
            $files, $scanpath($scan));
        } else if (preg_match("~\.php\$~", $scan)) {
          $files[] = $scan;
        }
      }
    }
  }
  return $files;
};

$table = function($id, &$explained, &$lines) {
  ?>
  <table id="<?=sprintf("table-%s", md5($id)) ?>" style="display:none;">
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
            <th>EXT</th>
        </tr>
    </thead>
    <tbody>
    <?php foreach ($explained as $num => $opline): ?>
    <?php if (@$opline["lineno"] != @$explained[$num+1]["lineno"]): ?>
    <?php   if (@$lines[$opline["lineno"]-1]): ?>
    <tr>
      <td class="code">#<?=$opline["lineno"] ?></td>
      <td colspan="9" class="code">
      <pre>
        <code class="php">
          <?php
          if (function_exists('gzcompress')) {
            echo htmlentities(rtrim(gzuncompress($lines[$opline["lineno"]-1])));
          } else echo htmlentities(rtrim($lines[$opline["lineno"]-1]));
          ?>
        </code>
      </pre>
      </td>
    </tr>
    <?php endif; ?>
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
            printf("<td>%s</td>", htmlentities($opline[$op]));
          } else printf("<td>-</td>");
        }
        if (isset($opline["extended_value"])) {
          printf("<td>0x%08.x</td>", $opline["extended_value"]);
        } else printf("<td>-</td>");
        ?>
        
    </tr>
    <?php endforeach; ?>
    </tbody>
    </table>
  <?php
};

if (is_dir($input)) {
  foreach ($scanpath($input) as $file) {
    $name = substr($file, strlen($input));
    
    $explained[$name] = explain(
      $file, EXPLAIN_FILE, $classes[$name], $functions[$name]);
      foreach (preg_split("~(\n)~", file_get_contents($file)) as $line) {
        if (function_exists('gzcompress')) {
          $lines[$name][] = gzcompress($line);
        } else $lines[$name][] = $line;
      }
  }
} else {
  if ($input && ($code = @file_get_contents($input))) {
    foreach (preg_split("~(\n)~", $code) as $line) {
      if (function_exists('gzcompress')) {
        $lines[$input][] = gzcompress($line);
      } else $lines[$input][] = $line;
    }
    $explained[$input] = explain(
      $input, EXPLAIN_FILE, $classes[$input], $functions[$input]);
  } else $explained = false;
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
        <?php foreach ($explained as $file => $explanation): ?>
        <li id="<?=md5($file) ?>">
        <a href="#"><?=$file ?></a>
          <ul>
            <?php if ($classes[$file]): ?>
            <li id="classes"><a href="#">Classes</a>
              <ul>
                <?php foreach ($classes[$file] as $class => $methods): ?>
                <li><a href="#"><?=$class ?></a>
                  <ul>
                    <?php foreach($methods as $method => $opcodes): ?>
                    <li id="<?=md5("{$file}-{$class}-{$method}") ?>"><a href="#"><?=$method ?></a></li>
                    <?php endforeach; ?>
                  </ul>
                </li>
                <?php endforeach; ?>
              </ul>
            </li>
            <?php endif; ?>
            
            <?php if ($functions[$file]): ?>
            <li id="functions"><a href="#">Functions</a>
              <ul>
              <?php   foreach($functions[$file] as $function => $opcodes): ?>
                <li id="<?=md5("{$file}-{$function}") ?>"><a href="#"><?=$function ?></a></li>
              <?php   endforeach; ?>
              </ul>
            </li>
            <?php endif; ?>
          </ul>
        </li>
        <?php endforeach; ?>
      </ul>
    </div>
  </div>
  <div id="right">
  <?php
  if ($explained) {
    foreach ($explained as $file => $explanation) {
      $table($file, $explanation, $lines[$file]);

      if ($classes[$file]): 
         foreach ($classes[$file] as $class => $methods): 
           foreach ($methods as $method => $opcodes):
             $table("{$file}-{$class}-{$method}", $opcodes, $lines[$file]);
           endforeach;
         endforeach;
      endif;
      
      if ($functions[$file]):
        foreach ($functions[$file] as $function => $opcodes):
          $table("{$file}-{$function}", $opcodes, $lines[$file]);
        endforeach;
      endif;
    }
  }
  ?>
  </div>
</div>

<script>hljs.initHighlightingOnLoad();</script>

</body>
</html>
