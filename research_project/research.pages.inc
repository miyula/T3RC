<?php
// $Id$
/**
 * @file
 * page callbacks for research pages.
 */

/**
 * Menu callback for researchpage/tools/window/%
 */
function select_research_tools_window($pid){    
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    
    //get tools list in this research
    $tools_sql = "SELECT tid, pid FROM {project_tools} WHERE pid=%d ORDER BY id";
    $res1 = db_query($tools_sql,$pid);
    $selected_tool_list = array();
    while($tool=db_fetch_object($res1)){
        $selected_tool_list[$tool->tid] = $tool->pid ;
    }
    
    //create form
    $content.= "<h3>Select the tool you want to use:</h3>";
    $content.= '<form name="research-tools-form">';
    $link_options = array(
        'attributes' => array('target'=>'_blank'),  
    );
    //get research tools list
    $all_tools_sql = "SELECT id, name FROM {research_tool} ORDER BY created";
    $res2 = db_query($all_tools_sql);
    while($tool=db_fetch_object($res2)){
        $view_link = l("(Learn it)","research/tools/{$tool->id}",$link_options);
        if(array_key_exists($tool->id,$selected_tool_list)){
            $content.="<p><input type='checkbox' name='tool_checkbox_{$tool->id}' value='{$tool->id}' checked='checked' >{$tool->name} $view_link</p>";
        }else{
            $content.="<p><input type='checkbox' name='tool_checkbox_{$tool->id}' value='{$tool->id}'>{$tool->name} $view_link</p>";
        }
        
    }
    $content.="<p><input type='button' value='Save' onclick='save_selected_tools($pid)'></p>";
    $content.='</form>';
    
    //release memory
    unset($selected_tool_list);
    
    print $content;    
}






/**
 * Menu callback for researchpage/tools/changed/%
 */
function ajax_save_researchpage_tool_changed($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    
    try{
        //get checked tools id from post
        $checked_list = $_POST['checkeds'];
        $checked = array();
        if(!empty($checked_list)){
            $token = strtok($checked_list, ",");
            while($token!==FALSE&&is_numeric($token)){
                $checked[] = $token;            
                $token = strtok(",");
            }
        }
    
        //get tools list from table
        $list_sql = "SELECT id, tid, pid FROM {project_tools} WHERE pid=%d";
        $tool_list = array();
        $res = db_query($list_sql,$pid);
        while($tool = db_fetch_object($res)){
            $tool_list[$tool->tid] = $tool->id;
            //check if tool not in checked
            if(in_array($tool->tid,$checked)==FALSE){
                //delete from {project tools}
                $delete = "DELETE FROM {project_tools} WHERE id=%d";
                db_query($delete,$tool->id);
            }
        }
    
        //check if checked not in tool_list
        foreach($checked as $check){
            if(array_key_exists($check,$tool_list)==FALSE){
                //add into project list
                $insert = "INSERT INTO {project_tools}(tid,pid) VALUES(%d,%d)";
                db_query($insert,$check,$pid);
            }
        }
        $status = 1;
    }catch(Exception $e){
        $status = 0;
    }
    //print_r($checked);
    //print_r($tool_list);
    printf('{"status":%d}',$status);
    
}

/**
 * Menu callback researchpage/tools/refresh/%
 */
function ajax_refresh_tool_list($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $tool_sql = "SELECT t.id id, t.name name, t.logo logo "
               ."FROM {project_tools} AS p, {research_tool} AS t "
               ."WHERE p.pid=%d AND t.id=p.tid";
    $res1 = db_query($tool_sql,$pid);
    $content = "";
    while($tool = db_fetch_array($res1)){
        if(!empty($tool['logo'])){
            $image = "<img src='{$tool['logo']}' height='16px'/>";
        }else{
            $image = "";
        }
	$content.= "<li>{$tool['name']} -- "
                  ."<a href='".url("research/tools/{$tool['id']}")."' target='_blank'>"
                  ."$image (Learn more)"
                  ."</a></li>";
    }
    print $content;
}


/**
 * Menu callback for researchpage/researcher/window/%
 */
function add_researcher_window($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $content = drupal_get_form("invite_new_researcher_form",$pid);
    print $content;    
}
/**
 * Invite new researcher form
 */
function invite_new_researcher_form(&$form_state,$pid=0){
    $form['project'] = array(
        '#type' => 'hidden',
        '#value' => $pid,
    );
    $form['email'] = array(
        '#type' => 'textfield',
        '#title' => t('Email'),
        '#description' => t("Invited researchers email address"),
        '#required' => TRUE,
        '#attributes' => array('onblur'=>"check_researcher_email_address($pid)"),
        '#suffix' => '<div id="check-research-email-result-div" class="form-check-result"></div>'
    );
    $form['introduction'] = array(
        '#type' => 'textfield',
        '#title' => t('Position Introduction'),
        '#description' => t('A short introduction for this researcher in your project. The content will be shown on project page'),
        '#required' => TRUE,
    );
    $form['weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#default_value' => 0,  
    );
    $form['submit'] = array(
        '#value' => '<input id="invite_new_researcher_button" type="button" value="Invite" onclick="invite_new_researcher('.$pid.')" disabled="disabled"  />',
    );
    
    return $form;
}

/**
 * Menu callback for researchpage/researcher/checkemail/%
 */
function check_invited_researcher_email($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $email = $_POST['email'];
    if(valid_email_address($email)!=1){
        $status = 0;
        $message = "Please give a valid email address";
    }else{
        //find account
        $account_sql = "SELECT uid, name FROM {users} WHERE mail='%s'";
        if($user = db_fetch_object(db_query($account_sql,$email))){
            //check if this user has already been invited
            $invited_check_sql = "SELECT id FROM {project_researchers} WHERE uid=%d AND pid=%d";
            if(db_result(db_query($invited_check_sql,$user->uid,$pid))){
                $status = 0;
                $message = "This researcher has already been invited into this project";
            }else{
                $status = 1;
                $message = "You are inviting this person: {$user->name}.";
            }
        }else{
            $status = 0;
            $message = "Can not find this user";
        }
    }

    printf('{"status":%d, "message":"%s"}',$status,$message);    
}

/**
 * Menu callback for researchpage/researcher/invitenew/%
 */
function ajax_invite_new_researcher($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    
    $email = $_POST['email'];
    $intro = $_POST['introduction'];
    
    //check email address and introduction
    if(valid_email_address($email)!=1||empty($intro)){
        $status = 0;
        $message = "Invalid input.";
    }else{
        //find account
        $account_sql = "SELECT uid, name FROM {users} WHERE mail='%s'";
        if($user = db_fetch_object(db_query($account_sql,$email))){
            //check if this user has already been invited
            $invited_check_sql = "SELECT id FROM {project_researchers} WHERE uid=%d AND pid=%d";
            if(db_result(db_query($invited_check_sql,$user->uid,$pid))){
                $status = 0;
                $message = "This researcher has already been invited into the project.";
            }else{
                $insert_sql = "INSERT INTO {project_researchers}"
                             ."(uid,pid,created,changed,introduction) "
                             ."VALUES(%d,%d,now(),now(),'%s')";
                db_query($insert_sql,$user->uid,$pid,$intro);
                $status = 1;
                $message = "Researcher has been added.";
            }
        }else{
            $status = 0;
            $message = "Can not find this user.";
        }
    }
    
    printf('{"status":%d,"message":"%s"}',$status,$message);
}

/**
 * Menu callback for researchpage/participants/page/%
 */
function invite_participants_page($pid){
    $path = drupal_get_path('module', 'research_project');
    drupal_add_js($path.'/invitation_page.js');
    $content = drupal_get_form('invite_participants_form',$pid);
    return $content;
}
/**
 * Invite participants form
 */
function invite_participants_form(&$form_state,$pid=0){
    $form['project'] = array(
        '#type' => 'hidden',
        '#value' => $pid,
    );
    $inform_option = array(
        'email' => 'By email',
        'sms' => 'By SMS',
    );
    $form['invite-way'] = array(
        "#type" => 'radios',
        "#title" => t('Way to inform'),
        "#description" => t('The way to inform persons about invitations'),
        '#options' => $inform_option,
        '#default_value' => 'email',
        '#attributes' => array('onchange'=>'invite_way_onchange()'),
    );
    $form['invitation-list'] = array(
        '#type' => 'textarea',
        '#title' => t('Invitation List'),
        '#description' => t('Please split each item by ",". e.g. +358449893823,+35832423235,+358233549509'),
        '#required' => TRUE,
    );
    $form['message'] = array(
        '#type' => 'textarea',
        '#title' => t('Invitating message'),
        '#description' => t('This message will be sent with the invitation content'),
        '#required' => TRUE,
        '#default_value' => generate_invitation_template('email',$pid),
    );
    $form['email-message'] = array(
        '#type' => 'hidden',
        '#value' => generate_invitation_template('email',$pid),
    );
    $form['sms-message'] = array(
        '#type' => 'hidden',
        '#value' => generate_invitation_template('sms',$pid),
    );
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Send invitation',
    );
    
    return $form;
}
/**
 * Handle validate form when sumbit
 */
function invite_participants_form_validate($form, &$form_state){
    
}
/**
 * Handle submit operations
 */
function invite_participants_form_submit($form, &$form_state){
    $pid = $form_state['values']['project'];
    $way = $form_state['values']['invite-way'];
    $lists = $form_state['values']['invitation-list'];
    $message = $form_state['values']['message'];
    
    //get project info
    $sql = "SELECT n.nid nid, title from {node} AS n, {research_project_page} AS r "
          ."WHERE r.id=%d AND r.nid=n.nid";
    if($project=db_fetch_object(db_query($sql,$pid))){
	$name = $project->title;
    }
    
    if($way=='email'){
        $mail_list = splite_mail_list($lists,',');
        foreach($mail_list as $mail){
            //find account, if null create it.
            $user = get_user_by_mail($mail,$name);
            //check if user has already linked with project
            $check = "SELECT COUNT(id) FROM {project_persons} WHERE pid=%d AND uid=%d";
            if(db_result(db_query($check,$pid,$user->uid))>0){
                continue;
            }
            //bind user with project
            $insert = "INSERT INTO {project_persons}"
                     ."(uid,pid,status,created,changed) "
                     ."VALUES(%d,%d,0,now(),now())";
            db_query($insert,$user->uid,$pid);
            //send email
            $subject =  t('@project invites you to join their research',array("@project"=>$name));
            $body = t($message);
            side_send_http_mail($mail,$subject,$body);
        }
    }
    drupal_goto("node/{$project->nid}");
}

?>