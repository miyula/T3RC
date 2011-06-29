<?php
// $Id$
/**
 * Template to display user profile page.
 *
 * Fields available:
 * $person: user object
 * $module_path: the path of module folder in the syetem
 */
?>
<div id="user-card">
    <div id="user-card-left">
        <dl><?php if(!empty($person->profiles->photo)){
	    echo "<img src='{$person->profiles->photo}' class='profile-head-photo'/>";
	}if($person->user_self) { ?>
        <input type="button" value="Edit my profile" onclick="window.location.href='<?=url("user/profiles/{$person->uid}/edit")?>'"/> 
        <input type="button" value="Admin account" onclick="window.location.href='<?=url("user/{$person->uid}/edit")?>'" />
        <?php } ?>
        </dl>
        <dl>
            <dt>Name</dt>
            <dd><?=$person->profiles->firstname?> <?=$person->profiles->lastname?></dd>
        </dl>
        <dl>
            <dt>Age</dt>
            <dd><?=$person->profiles->age?></dd>
        </dl>
        <dl>
            <dt>Sex</dt>
            <dd><?php echo $person->profiles->gender=='F'?"Female":"Male"; ?></dd>
        </dl>
        <dl>
            <dt>Phone number</dt>
            <dd><?=$person->profiles->phone?></dd>
        </dl>
        <dl>
            <dt>Email</dt>
            <dd><?=$person->mail?></dd>
        </dl>
    
    </div>
    <div id="user-card-right">
        <h2>Research Projects</h2>
        <div id="user-card-right-info">
            <dl><dd><?=$person->work_project?></dd><dt>work for</dt></dl>
            <dl><dd><?=$person->participate_project?></dd><dt>participate</dt></dl>
        </div>
    </div>
</div>
<div class="projects-list-div">
    <h2>Projects working for</h2>
    <?php if($person->user_self&&user_access('Create research page')){?>
       <p><input type='button' value='Create new project' onclick='window.location.href="<?=url('node/add/researchproject')?>"' /></p>  
    <?php } ?>
    <ul class="projects-list">
    <?php foreach($person->work_projects as $project){ ?>
        <li><a href="<?=url("node/".$project[nid])?>"><?=$project['title']?></a> <span class="history-text">(Joined in <?=format_interval(time()-$project['history'])?> ago)</span></li>
    <?php } ?>
    </ul>
</div>
<div class="projects-list-div">
    <h2>Projects participating in</h2>
    <ul class="projects-list">
    <?php foreach($person->participate_projects as $project){ ?>
        <li><a href="<?=url("node/".$project[nid])?>"><?=$project['title']?></a> <span class="history-text">(Joined in <?=format_interval(time()-$project['history'])?> ago)</span></li>
    <?php } ?>    
    </ul>
</div>