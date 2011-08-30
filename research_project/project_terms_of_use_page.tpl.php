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
<div id="project_term" class="project_term">
    <div id="term_title" class="term_title">
            <?php echo $terms_template['name'];?>
    </div>
	<div id="term_content" class="term_content">
			 <?php echo $terms_template['term_of_use'];?>
	</div>
	<div id="confirm_button">
	<input class="next_btn" type="button" value="Accept" onClick="parent.location='http://t3rc.tkk.fi/t3rc/home_page'"> 
	<input class="next_btn" type="button" value="Decline" onClick="parent.location='http://t3rc.tkk.fi/t3rc/home_page'">
	</div>
</div>
