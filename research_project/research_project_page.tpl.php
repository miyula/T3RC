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
<div id="tools-div" class="r-page-part">
    <h2>
        <span class="sub-title">Tools</span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button" onclick="edit_tools(<?=$node->pid?>)">Edit</span>
    <?php } ?>
    </h2>
    <ul id="tools-list">
    <?php foreach($node->tool_list as $tool) {?>
        <li><?=$tool['name']?> --  
            <a href="<?=url("research/tools/{$tool['id']}")?>" target="_blank">
                <?php if(!empty($tool['logo'])){ ?><img src="<?=$tool['logo']?>" height="16px"/> <?php } ?> (Learn more)
            </a>
        </li>
    <?php } ?>
    </ul>
</div>
<div id="researchers-div" class="r-page-part">
    <h2>
        <span class="sub-title">Researchers</span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button" onclick="show_new_researcher_window(<?=$node->pid?>)">Add</span><span class="action-button">Edit</span>
    <?php } ?>
    </h2>
    <ul id="researchers-list">
    <?php foreach($node->researcher_list as $researcher) { ?>
        <li>
            <img class="staff-photo" src="<?=$module_path?>/images/user_default.gif" alt=""/>
            <span class="staff-name"><a href="<?=$researcher['uid']?>" target="_blank"><?=$researcher['name']?></a></span>
            <span class="staff-intro">(<?=$researcher['introduction']?>)</span></li>
    <?php } ?>
    </ul>
</div>
<div id="participants-div" class="r-page-part">
    <h2>
        <span class="sub-title">Participants</span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button"><a href="<?=url("researchpage/participants/page/{$node->pid}")?>">Invite new</a></span>
    <?php } ?>
    </h2>
    <?php if(isset($node->partipatent_count)) { ?>
    <p>There are <?=$node->partipatent_count?> people joined in this project. </p>
    <?php }else{ ?>
    <p>No information.</p>
    <?php } ?>
</div>
<div id="reports-div" class="r-page-part">
    <h2>
        <span class="sub-title">Reports</span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button">Add</span><span class="action-button">Edit</span>
    <?php } ?>
    </h2>
    <ul id="reports-list">
        <li><a href="" target="_blank">Report one</a></li>
        <li><a href="" target="_blank">Report two</a></li>
        <li><a href="" target="_blank">Report three</a></li>
    </ul>
</div>

<?php if($node->edit_permission){ ?>
<div id="edit-dialog-div" title="">
    <div id="onloading-div"><center><img src="<?=$module_path?>/images/loading.gif"/></center></div>
    <div id="edit-dialog-content"></div>
</div>
<?php } ?>