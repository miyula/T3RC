<?php
// $Id$
/**
 * @file
 * page callbacks for user profile pages.
 */


/**
 * Menu callback for user/profiles/%
 */ 
function display_user_profiles_page($person){
    $path = drupal_get_path('module','research_project');
    drupal_add_css(drupal_get_path('module','research_project')."/profile.css");
    drupal_add_css($path.'/jquery/css/jquery.window.css');
    drupal_add_css($path.'/css/project_welcome_page.css');
    
        
    drupal_add_js($path.'/jquery/jquery.js');
    drupal_add_js($path.'/jquery/jquery-ui.min.js');
    drupal_add_js($path.'/jquery/jquery.window.min.js');
    //drupal_add_js($path.'/js/project_welcome_page.js');
    
    //$person = user_load($uid);
    
    //get user profiles
    load_user_profiles($person);
    
    //count user projects
    $count_work_sql = "SELECT COUNT(id) FROM {project_researchers} WHERE uid=%d";
    $count_work = db_result(db_query($count_work_sql,$person->uid));
    $person->work_project = $count_work;
    $count_participate_sql = "SELECT COUNT(id) FROM {project_persons} WHERE uid=%d";
    $count_participate = db_result(db_query($count_participate_sql,$person->uid));
    $person->participate_project = $count_participate;
    
    //get user projects
    $work_projects = array();
    $work_projects_sql = "SELECT n.nid, n.title, UNIX_TIMESTAMP(r.created) history FROM {node} AS n, {research_project_page} AS p, {project_researchers} AS r "
                        ."WHERE r.uid={$person->uid} AND r.pid=p.id AND n.nid=p.nid ORDER by r.created";
                        
    $res2 = db_query($work_projects_sql);
    while($project=db_fetch_object($res2)){
        
        //get tool list
        $tool_sql = "SELECT t.id id, t.name name, t.logo logo, t.download download "
               ."FROM {project_tools} AS p, {research_tool} AS t "
               ."WHERE p.pid=%d AND t.id=p.tid";
        $res4 = db_query($tool_sql,$project->nid);
        $tool_list = array();
        while($tool = db_fetch_array($res4)){
            $tool_list[] = $tool;
        }
        $project->tool_list = $tool_list;
        $work_projects[] = $project;
        
    }
    //print_r($work_projects);
    $person->work_projects = $work_projects;
    
    $participate_projects = array();
    $joined_projects_sql = "SELECT n.nid, n.title, UNIX_TIMESTAMP(pp.created) history FROM {node} AS n, {research_project_page} AS p, {project_persons} AS pp "
                        ."WHERE pp.uid={$person->uid} AND pp.pid=p.id AND n.nid=p.nid ORDER BY pp.created";
    $res3 = db_query($joined_projects_sql);
    while($project=db_fetch_array($res3)){
        $participate_projects[] = $project;                  
    }
    $person->participate_projects = $participate_projects;
    
    //check current user
    global $user;
    if($user->uid==$person->uid){
        $person->user_self = TRUE;
    }else{
        $person->user_self = FALSE;
    }
    
    $content = theme('project_user_profile_page',$person);
    return $content;
}

/**
 * Menu callback for user/profiles/%user/edit
 */
function edit_user_profiles_page($user){
    $content = drupal_get_form('project_user_profiles_form',$user);
    return $content;
}
/**
 * Form for user profiles info
 */
function project_user_profiles_form(&$form_state,$user){
    //get user profiles
    load_user_profiles($user);
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['uid'] = array(
        '#type' => 'hidden',
        '#value' => $user->uid,
    );
    $form['firstname'] = array(
        '#type' => 'textfield',
        '#title' => t('First name'),
        '#required' => TRUE,
        '#default_value' => empty($user->profiles->firstname)?"":$user->profiles->firstname,
    );
    $form['lastname'] = array(
        '#type' => 'textfield',
        '#title' => t('Last name'),
        '#required' => TRUE,
        '#default_value' => empty($user->profiles->lastname)?"":$user->profiles->lastname,
    );
    $options = array(
        'F' => t('Female'),
        'M' => t('Male'),
    );
    $form['gender'] = array(
        '#type' => 'radios',
        '#title' => t('Gender'),
        '#required' => TRUE,
        '#options' => $options,
        '#default_value' => empty($user->profiles->gender)?"":$user->profiles->gender,
    );
    $form['phone'] = array(
        '#type' => 'textfield',
        '#title' => t('Phone number'),
        '#required' => FALSE,
        '#default_value' => empty($user->profiles->phone)?"":$user->profiles->phone,
    );
    $form['birthday'] = array(
        '#type' => 'textfield',
        '#title' => t('Birthday'),
        '#required' => FALSE,
        '#description' => t('Data format is YYYY-MM-DD. e.g. 2000-06-21'),
        '#default_value' => empty($user->profiles->birthday)?"":$user->profiles->birthday,    
    );
    $form['picture'] = array(
        '#type'  => 'file',
        '#title' => t('Picture'),
        '#description' => t('If no change, keep this empty.'),
        '#suffix' => empty($user->profiles->photo)?"":"<img src='{$user->profiles->photo}' height='80px'/><br/>",  
    );
    $form['picture-url'] = array(
        '#type' => 'hidden',
        '#value' => empty($user->profiles->photo)?"":$user->profiles->photo,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    
    return $form;
}


function test(){
global $user;
$content = theme('test_page',$user);
    return $content;
}
/**
 * Handle validate for profile form
 */
function project_user_profiles_form_validate($form,&$form_state){
    $phone = $form_state['values']['phone'];
    if(!empty($phone)&&!valid_phone_number($phone)){
        form_set_error('phone','Invalide phone number input.');
    }
    $birthday = $form_state['values']['birthday'];
    if(!empty($birthday)){
        //validate birthday value
        if(!preg_match('/^[1-2][0-9][0-9][0-9][-][0-9][0-9][-][0-9][0-9]$/', $birthday)){
            form_set_error('birthday','Invalide birthday input.');
        }
    }
}
/**
 * Handel submit for user profile form
 */
function project_user_profiles_form_submit($form,&$form_state){
    $uid = $form_state['values']['uid'];
    $firstname = $form_state['values']['firstname'];
    $lastname = $form_state['values']['lastname'];
    $phone = $form_state['values']['phone'];
    $birthday = $form_state['values']['birthday'];
    $gender = $form_state['values']['gender'];
    
    //Upload image
    $validators = array(
        'file_validate_is_image' => array(),
    );
    $file = file_save_upload('picture', $validators);
    if($file!=0){
        $info = image_get_info($file->filepath);
        //create tools_logo folder if not existed
        $folder = "users_photo";
        $path = file_create_path($folder);
        file_check_directory($path, FILE_CREATE_DIRECTORY);
        
        $destination = $folder.'/'.time().'.'. $info['extension'];
        if(file_copy($file, $destination, FILE_EXISTS_REPLACE)){
	    $photo = url($file->filepath);
            $photo = str_replace("?q=","", $photo );
        }else{
            //keep old logo
           $photo = $form_state['values']['picture-url'];
        }
    }else{
        //no change in logo
        $photo = $form_state['values']['picture-url'];
    }
    
    //find profile
    $find = "SELECT COUNT(*) FROM {person_profiles} WHERE uid=%d";
    if(db_result(db_query($find,$uid))>0){
        $update = "UPDATE {person_profiles} "
                 ."SET phone='%s', firstname='%s', lastname='%s', photo='%s', birthday='%s', gender='%s' "
                 ."WHERE uid=%d";
        db_query($update,$phone,$firstname,$lastname,$photo,$birthday,$gender,$uid);
    }else{
        //insert new
        $insert = "INSERT INTO {person_profiles}"
                ."(uid,phone,firstname,lastname,photo,birthday,gender) "
                ."VALUES(%d,'%s','%s','%s','%s','%s','%s')";
        db_query($insert,$uid,$phone,$firstname,$lastname,$photo,$birthday,$gender);
    }
    drupal_goto("user/profiles/$uid");
}
?>