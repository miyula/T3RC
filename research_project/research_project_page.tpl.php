<?php
// $Id$
/**
 * Template to display research project page.
 *
 * Fields available:
 * $node: research project node object
 * $module_path: the path of module folder
 */
?>
<?php
$url=curPageURL();
if(strchr($url,"/fi/"))
	include"language/fi.php";
elseif(strchr($url,"/zh-hans/"))	
	include"language/ch.php";
else
	include"language/en.php";
function curPageURL() {
 $pageURL = 'http';
 if ($_SERVER["HTTPS"] == "on") {$pageURL .= "s";}
 $pageURL .= "://";
 if ($_SERVER["SERVER_PORT"] != "80") {
  $pageURL .= $_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"].$_SERVER["REQUEST_URI"];
 } else {
  $pageURL .= $_SERVER["SERVER_NAME"].$_SERVER["REQUEST_URI"];
 }
 return $pageURL;
}
	
?>
<div id="tools-div" class="r-page-part">
    <h2>
        <span class="sub-title"><?php echo TOOLS; ?>  </span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button" onclick="edit_tools(<?=$node->pid?>)"><?php echo EDIT_TOOLS; ?></span>
    <?php } ?>
    </h2>
    <ul id="tools-list">
    <?php foreach($node->tool_list as $tool) {?>
        <li><?=$tool['name']?> --  
            <a href="<?=url("research/tools/{$tool['id']}")?>" target="_blank">
                <?php if(!empty($tool['logo'])){ ?><img src="<?=$tool['logo']?>" height="16px"/> <?php } ?> <?php echo "(".LEARN_MORE.")"; ?>
            </a>
        </li>
    <?php } ?>
    </ul>
</div>
<div id="researchers-div" class="r-page-part">
    <h2>
        <span class="sub-title"><?php echo RESEARCHERS;?></span>
    <?php if($node->is_owner){ ?>
        <span class="action-button" onclick="show_new_researcher_window(<?=$node->pid?>)"><?php echo ADD_NEW_RESERACHER; ?></span>
    <?php } ?>
    </h2>
    <ul id="researchers-list">
    <?php foreach($node->researcher_list as $researcher) { ?>
        <li>
            <img class="staff-photo" src="<?=empty($researcher['photo'])?"$module_path/images/user_default.gif":$researcher['photo']?>" alt=""/>
            <span class="staff-name"><a href="<?=url("user/".$researcher['uid'])?>" target="_blank"><?=$researcher['name']?></a></span>
            <span class="staff-intro">(<?=$researcher['introduction']?>)</span>
        <?php if($node->is_owner) { ?>
            <span class="edit-text">[<a href="javascript:open_edit_researcher_window(<?=$node->pid?>,<?=$researcher['id']?>)"><?php echo EDIT;?></a>]</span>
        <?php } ?>
        </li>
    <?php } ?>
    </ul>
</div>
<div id="participants-div" class="r-page-part">
    <h2>
        <span class="sub-title">Participants</span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button"><a href="<?=url("researchpage/participants/page/{$node->pid}")?>"><?php echo INVITE_NEW; ?></a></span>
    <?php } ?>
    </h2>
    <?php if(isset($node->partipatent_count)) { ?>
    <p><?php echo THERE_ARE;?> <?=$node->partipatent_count?> <?php echo PEOPLE_IN_PROJECT;?> 
	<a href="<?=url("researchpage/participants/list/{$node->pid}")?>"><?php echo VIEW_THEM;?></a>.</p>
    <?php }else{ ?>
    <p><?php echo NO_INFORMATION; ?></p>
    <?php } ?>
</div>
<div id="reports-div" class="r-page-part">
    <h2>
        <span class="sub-title"><?php echo DOCUMENTS;?></span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button" onclick="display_add_new_report_window(<?=$node->pid?>)"><?php echo ADD_NEW_DOCUMENTS; ?></span>
    <?php } ?>
    </h2>
    <ul id="reports-list">
    <?php foreach($node->document_list as $document){ ?>
        <li>
            <a href="<?=url("node/{$document['nid']}")?>" target="_blank"><?=$document['title']?></a>
            <span class="edit-text">
                <a href="javascript:display_edit_report_window(<?=$node->pid?>,<?=$document['id']?>)">[<?php echo EDIT;?>]</a>
            </span>
            <p><?=$document['introduction']?></p>
        </li>
    <?php } ?>
    </ul>
</div>

<?php if($node->edit_permission){ ?>
<div id="edit-dialog-div" title="">
    <div id="onloading-div"><center><img src="<?=$module_path?>/images/loading.gif"/></center></div>
    <div id="edit-dialog-content"></div>
</div>
<?php } ?>