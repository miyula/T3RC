<?php
// $Id$
/**
 * @file
 * project manage page callbacks for research project management module.
 */

/**
 * Menu callback for node/%/manage
 */
function manage_project_page($node){
    $path = drupal_get_path('module', 'research_project');
    drupal_add_css($path.'/research_project.css');
    //drupal_add_css($path.'/jquery/css/jquery.window.css');
    //drupal_add_css($path.'/css/project_welcome_page.css');
    drupal_add_css($path.'/jquery/jquery-ui.css');
    
    //$node = node_prepare($node, $teaser); // get it ready for display
    
    //check current user permission
    global $user;
    $node->edit_permission = _access_edit_research_page($node, $user);
    
    //check if current user is the owner for this project page
    if($node->uid==$user->uid){
	$node->is_owner = TRUE;
    }else{
	$node->is_owner = FALSE;
    }
    
    if($node->edit_permission){
	
	//add setting Drupal.settings.research_project.tool_window
	drupal_add_js(array('research_project' => array('tool_window' => url("researchpage/tools/window/"))), 'setting');
	//add setting Drupal.settings.research_project.tool_changed
	drupal_add_js(array('research_project' => array('tool_changed' => url("researchpage/tools/changed/"))), 'setting');
	//add setting Drupal.settings.research_project.tool_refresh
	drupal_add_js(array('research_project' => array('tool_refresh' => url("researchpage/tools/refresh/"))), 'setting');
	//add setting Drupal.settings.research_project.research_window
	drupal_add_js(array('research_project' => array('research_window' => url("researchpage/researcher/window/"))), 'setting');
	//add setting Drupal.settings.research_project.research_email_check
	drupal_add_js(array('research_project' => array('research_email_check' => url("researchpage/researcher/checkemail/"))), 'setting');
	//add setting Drupal.settings.research_project.research_invite_new
	drupal_add_js(array('research_project' => array('research_invite_new' => url("researchpage/researcher/invitenew/"))), 'setting');
	//add setting Drupal.settings.research_project.research_refresh
	drupal_add_js(array('research_project' => array('research_refresh' => url("researchpage/researcher/refresh/"))), 'setting');
	//add setting Drupal.settings.research_project.report_window
	drupal_add_js(array('research_project' => array('report_window' => url("researchpage/report/window/"))), 'setting');
	//add setting Drupal.settings.research_project.report_bind
	drupal_add_js(array('research_project' => array('report_bind' => url("researchpage/report/bind/"))), 'setting');
	//add setting Drupal.settings.research_project.report_refresh
	drupal_add_js(array('research_project' => array('report_refresh' => url("researchpage/reports/refresh/"))), 'setting');
	
    }
    
    drupal_add_js($path.'/jquery/jquery.js');
    drupal_add_js($path.'/jquery/jquery-ui.min.js');
    drupal_add_js($path.'/jquery/jquery.window.min.js');
    drupal_add_js($path.'/research_project.js');
    //drupal_add_js($path.'/js/project_welcome_page.js');
    
    //get tools list
    $tool_sql = "SELECT t.id id, t.name name, t.logo logo, t.download download "
               ."FROM {project_tools} AS p, {research_tool} AS t "
               ."WHERE p.pid=%d AND t.id=p.tid";
    $res1 = db_query($tool_sql,$node->pid);
    $tool_list = array();
    while($tool = db_fetch_array($res1)){
	$tool_list[] = $tool;
    }
    $node->tool_list = $tool_list;
    
    //get project page owner
    
    //get researcher list 
    $researcher_sql = "SELECT r.id id, r.uid uid, introduction, u.name name, p.photo photo "
                     ."FROM {project_researchers} AS r, {users} AS u, {person_profiles} AS p "
                     ."WHERE r.pid=%d AND u.uid=r.uid AND p.uid=r.uid "
		     ."ORDER BY r.weight";
    $res2 = db_query($researcher_sql,$node->pid);
    $researcher_list = array();
    while($researcher = db_fetch_array($res2)){
	$researcher_list[] = $researcher;
    }
    $node->researcher_list = $researcher_list;
    
    //get partipatents
    $persons_count = "SELECT COUNT(id) FROM {project_persons} WHERE pid=%d";
    if($count = db_result(db_query($persons_count,$node->pid))){
	$node->partipatent_count = $count;
    }
    $get_first_five_person = "SELECT p.uid uid, p.status status "
                            ."FROM {project_persons} AS p, {users} AS u "
			    ."WHERE p.pid=%d AND u.uid=p.uid "
			    ."ORDER BY p.created DESC "
			    ."";
    
    //get project documents
    $get_documents_sql = "SELECT d.id id, n.nid nid, n.title title, d.introduction introduction FROM {project_documents} AS d, {node} AS n WHERE pid=%d AND d.nid=n.nid ORDER by d.weight";
    $res3 = db_query($get_documents_sql,$node->pid);
    $document_list = array();
    while($document = db_fetch_array($res3)){
	$document_list[] = $document;
    }
    $node->document_list = $document_list;
    
    
    return theme('research_project_page', $node);
    
    //drupal_not_found();
    exit;
}

