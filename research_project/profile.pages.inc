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
    //get user profiles
    load_user_profiles($person);
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
        '#default_value' => empty($user->profiles->birthday)?"":array('year' => 2007, 'month' => 2, 'day'=>20),
    );
    $form['picture'] = array(
        '#type'  => 'file',
        '#title' => 'Picture',
        '#description' => t('If no change, keep this empty.'),
        '#suffix' => empty($user->profiles->photo)?"":"<img src='{}' height='80px'/><br/>",  
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
    $photo = $form_state['values']['picture-url'];
    $firstname = $form_state['values']['firstname'];
    $lastname = $form_state['values']['lastname'];
    $phone = $form_state['values']['phone'];
    $birthdayArray = $form_state['values']['birthday'];
    $birthday = $birthdayArray['year'].'-'.$birthdayArray['month'].'-'.$birthdayArray['day'];
    $gender = $form_state['values']['gender'];
    
    
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