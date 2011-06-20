<?php
// $Id$
/**
 * Template to display research tool page.
 *
 * Fields available:
 * $tool: research tool object
 * $module_path: the path of module folder in the syetem
 */
?>
<?php if($tool->edit_permission) { ?>
<div><?=$tool->edit_link?></div>
<?php } ?>
<?php if(!empty($tool->logo)){ ?>
    <img src="<?=$tool->logo?>" alt="logo"/>
<?php } ?>
<div class="r-page-part" id="introduction">
    <h2>Introduction</h2>
    <?=$tool->description?>
</div>
<div class="r-page-part" id="guidance">
    <h2>How to use it</h2>
<?php if(!empty($tool->download)){ ?>
    <p><a href="<?=$tool->download?>" target="_blank"><img src="<?=$module_path?>/images/download.png" alt="Download"/></a></p>
<?php } ?>    
    <?=$tool->guidance?>
</div>
