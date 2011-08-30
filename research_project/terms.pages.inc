<?php
// $Id$
/**
 * @file
 * page callbacks for term of use pages.
 */


/**
 * Menu callback for researchproject/termofuse/%
 */ 
function terms_of_use($project_name){
    drupal_add_css(drupal_get_path('module','research_project')."/term.css");
    $variable_name='termofuse_project_'.$project_name;	
    $terms_template=variable_get($variable_name,variable_get('termofuse_project_inform_researcher_tpl',''));
    $content = theme('project_terms_of_use_page',$terms_template);
    return $content;
}

?>