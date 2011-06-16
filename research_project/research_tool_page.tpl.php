<?php
// $Id$
/**
 * Template to display research tool page.
 *
 * Fields available:
 * $tool: research tool object
 */
?>
<h2><?=$tool->name?></h2>
<h3>Introduction</h3>
<?=$tool->description?>
<h3>How to use it</h3>
<p>Download: <a href="<?=$tool->download?>" target="_blank"><?=$tool->download?></a></p>
<?=$tool->guidance?>
