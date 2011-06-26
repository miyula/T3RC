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
    drupal_add_css(drupal_get_path('module','research_project')."/profile.css");
    //$person = user_load($uid);
    
    //check view user profile permission
    access_view_user_profiles($person->uid);
    
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
    $work_projects_sql = "SELECT n.nid, n.title, LEFT(r.created, 10) history FROM {node} AS n, {research_project_page} AS p, {project_researchers} AS r "
                        ."WHERE r.uid={$person->uid} AND r.pid=p.id AND n.nid=p.nid ORDER by r.created";
                        
    $res2 = db_query($work_projects_sql);
    while($project=db_fetch_array($res2)){
        $work_projects[] = $project;
    }
    print_r($work_projects);
    $person->work_projects = $work_projects;
    
    $participate_projects = array();
    $joined_projects_sql = "SELECT n.nid, n.title, LEFT(pp.created, 10) history FROM {node} AS n, {research_project_page} AS p, {project_persons} AS pp "
                        ."WHERE pp.uid={$person->uid} AND pp.pid=p.id AND n.nid=p.nid ORDER BY pp.created";
    $res3 = db_query($joined_projects_sql);
    while($project=db_fetch_array($res3)){
        $participate_projects[] = $project;                  
    }
    $person->participate_projects = $participate_projects;
    
    //check current user
    global $user;
    if($user->uid==$person->uid){
        $person->access_edit_profile = TRUE;
    }else{
        $person->access_edit_profile = FALSE;
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
        'F' => 'Female',
        'M' => 'Male',
    );
    $form['gender'] = array(
        '#type' => 'radios',
        '#title' => 'Gender',
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
        '#type' => 'date',
        '#title' => 'Birthday',
        '#required' => TRUE,
        '#default_value' => empty($user->profiles->byear)?"":array('year' => $user->profiles->byear, 'month' => $user->profiles->bmonth, 'day'=>$user->profiles->bday),
    );
    $form['picture'] = array(
        '#type'  => 'file',
        '#title' => 'Picture',
        '#description' => t('If no change, keep this empty.'),
        '#suffix' => empty($user->profiles->photo)?"":"<img src='{$user->profiles->photo}' height='80px'/><br/>",  
    );
    $form['picture-url'] = array(
        '#type' => 'hidden',
        '#value' => empty($user->profiles->photo)?"":$user->profiles->photo,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Save',
    );
    
    return $form;
}
/**
 * Handel submit for user profile form
 */
function project_user_profiles_form_submit($form,&$form_state){
    $uid = $form_state['values']['uid'];
    $firstname = $form_state['values']['firstname'];
    $lastname = $form_state['values']['lastname'];
    $phone = $form_state['values']['phone'];
    $birthdayArray = $form_state['values']['birthday'];
    $birthday = $birthdayArray['year'].'-'.$birthdayArray['month'].'-'.$birthdayArray['day'];
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