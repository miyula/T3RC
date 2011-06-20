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
        <li><img class="staff-photo" src="http://www.adminvc.com/images/user_default.gif" alt=""/><span class="staff-name"><a href="" target="_blank">Firstname, Lastname</a></span><span class="staff-intro">(Posititon introduction......)</span></li>
    </ul>
</div>
<div id="participants-div" class="r-page-part">
    <h2>
        <span class="sub-title">Participants</span>
    <?php if($node->edit_permission){ ?>
        <span class="action-button">Add</span><span class="action-button">Edit</span>
    <?php } ?>
    </h2>
    <p>There are 100 people joined in this project.</p>
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