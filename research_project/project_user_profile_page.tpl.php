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
<div id="user-card">
    <div id="user-card-left">
        <dl><?php if(!empty($person->profiles->photo)){
	    echo "<img src='{$person->profiles->photo}' class='profile-head-photo'/>";
	}if($person->user_self) { ?>
        <input type="button" value="<?php echo EDIT_PROFILE;?>" onclick="window.location.href='<?=url("user/profiles/{$person->uid}/edit")?>'"/> 
        <input type="button" value="<?php echo ADMIN_ACCOUNT;?>" onclick="window.location.href='<?=url("user/{$person->uid}/edit")?>'" />
        <?php } ?>
        </dl>
        <dl>
            <dt><?php echo NAME;?></dt>
            <dd><?=$person->profiles->firstname?> <?=$person->profiles->lastname?></dd>
        </dl>
        <dl>
            <dt><?php echo AGE;?></dt>
            <dd><?=$person->profiles->age?></dd>
        </dl>
        <dl>
            <dt><?php echo SEX;?></dt>
            <dd><?php echo $person->profiles->gender=='F'?"Female":($person->profiles->gender=='M'?'Male':''); ?></dd>
        </dl>
        <dl>
            <dt><?php echo PHONE_NUMBER;?></dt>
            <dd><?=$person->profiles->phone?></dd>
        </dl>
        <dl>
            <dt><?php echo EMAIL;?></dt>
            <dd><?=$person->mail?></dd>
        </dl>
    
    </div>
    <div id="user-card-right">
        <h2><?php echo RESEARCH_PROJECT;?></h2>
        <div id="user-card-right-info">
            <dl><dd><?=$person->work_project?></dd><dt><?php echo RESEARCH_FOR;?></dt></dl>
            <dl><dd><?=$person->participate_project?></dd><dt><?php echo PARTICIPANTS;?></dt></dl>
        </div>
    </div>
</div>
<div class="projects-list-div">
    <h2><?php echo PROJECTS_RESEARCH_FOR;?></h2>
    <?php if($person->user_self&&user_access('Create research page')){?>
       <p><input type='button' value='<?php echo CREATE_NEW_PROJECT;?>' onclick='window.location.href="<?=url('node/add/researchproject')?>"' /></p>  
    <?php } ?>
    <ul class="projects-list">
    <?php foreach($person->work_projects as $project){ ?>
        <li>
	  <h3><a href="<?=url("node/".$project->nid)?>"><?=$project->title?></a></h3>
	  <p><span class="history-text">(Joined in <?=format_interval(time()-$project->history)?> <?php echo AGO;?>)</span><a href="<?=url("node/{$project->nid}/manage")?>">manage</a></p>
	  <div class="research-tool-blocks">
	   <?php foreach($project->tool_list as $tool): ?>
	    <div class="research-tool-block">
	     <div class="research-tool-log-div">
	      <a class="tab-window-link" href=""><img class="research-tool-logo" src="<?=$tool['logo']?>" alt="<?=$tool['name']?>"/></a>
	     </div>
	     <p class="research-tool-name-p"><a class="tab-window-link" href="<?=url("research/tools/{$tool['id']}")?>"><?=$tool['name']?></a></p>
	    </div>
	   <?php endforeach; ?>
	  </div>
	</li>
    <?php } ?>
    </ul>
</div>
<div class="projects-list-div">
    <h2><?php echo PROJECTS_PARTICIPANTING_IN;?></h2>
    <ul class="projects-list">
    <?php foreach($person->participate_projects as $project){ ?>
        <li><a href="<?=url("node/".$project[nid])?>"><?=$project['title']?></a> <span class="history-text">(Joined in <?=format_interval(time()-$project['history'])?> <?php echo AGO;?>)</span></li>
    <?php } ?>    
    </ul>
</div>