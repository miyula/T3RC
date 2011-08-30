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
</div>
