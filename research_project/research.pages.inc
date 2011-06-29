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
    $form['send-mail'] = array(
        '#type' => 'checkbox',
        '#title' => 'Send an email to inform researcher.',
    );
    $form['submit'] = array(
        '#value' => '<input id="invite_new_researcher_button" type="button" value="Invite" onclick="invite_new_researcher('.$pid.')" disabled="disabled"  />',
    );
    
    return $form;
}


/**
 * Menu callback for researchpage/researcher/window/%/edit/%
 */
function edit_researcher_window($rid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    //get researcher info
    $sql = "SELECT r.id id, r.uid uid, r.pid pid, r.introduction introduction, r.weight weight, u.name name, u.mail mail "
          ."FROM {project_researchers} AS r, {users} AS u "
          ."WHERE r.id=%d AND r.uid=u.uid";
    
    $researcher = db_fetch_object(db_query($sql,$rid));
    if($researcher){
        //check if the researcher is the owner of the project
        //TODO: now only the onwer of the project can access to this window
        global $user;
        if($user->uid==$researcher->uid){
            $researcher->is_owner = TRUE;
        }else{
            $researcher->is_owner = FALSE;
        }
        
        $content = drupal_get_form("edit_researcher_window_form", $researcher);
    }else{
        $content = "<p>Can't find researcher information</p>";
    }
    print $content;
}

/**
 * Form for edit researcher window
 */
function edit_researcher_window_form(&$form_state,$researcher){
    $form['user-info'] = array(
        '#value' => "<p>Username: {$researcher->name}</p><p>Email: {$researcher->mail}</p>",  
    );
    $form['introduction'] = array(
        '#type' => 'textfield',
        '#title' => t('Position Introduction'),
        '#description' => t('A short introduction for this researcher in your project. The content will be shown on project page'),
        '#default_value' => $researcher->introduction,
        '#required' => TRUE,
    );
    $form['weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#default_value' => $researcher->weight,  
    );
    $form['buttons'] = array(
        '#value' => "<input type='button' value='Save' onclick='save_edit_researcher_change({$researcher->pid},{$researcher->id})' />".($researcher->is_owner?"":" <input type='button' value='Remove' onclick='remove_researcher_from_project({$researcher->pid},{$researcher->id})' />"),
    );
    return $form;
}

/**
 * Menu callback for researchpage/researcher/window/%/save/%
 */
function ajax_save_researcher_window_change($rid){
    
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $introduction = $_POST['intro'];
    $weight = (int) $_POST['weight'];
    
    if(empty($introduction)){
        $status = "0";
        $message = "Introduction should not be empty.";
    }else{
        $update = "UPDATE project_researchers SET introduction='%s', weight=%d WHERE id=%d";
        db_query($update, $introduction, $weight, $rid);
        $status = "1";
    }
    
    
    printf('{"status":%d,"message":"%s"}',$status,$message);
}

/**
 * Menu callback for researchpage/researcher/window/%/delete/%
 */
function ajax_delete_researcher_from_project($rid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $rid = (int)$rid;
    if($rid>0){
        $sql = "DELETE FROM {project_researchers} WHERE id=%d";
        if(db_query($sql,$rid)){
            $status = 1;
            $message = "Successed to remove researcher from project";
        }else{
            $status = 0;
            $message = "SQL error!";
        }
        
    }else{
        $status = 0;
        $message = "rid should be an index!";
    }
    
    printf('{"status":%d,"message":"%s"}',$status,$message);
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
    $weight = (int)$_POST['weight'];
    $send_mail = $_POST['send'];
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
                             ."(uid,pid,created,changed,introduction,weight) "
                             ."VALUES(%d,%d,now(),now(),'%s',%d)";
                db_query($insert_sql,$user->uid,$pid,$intro,$weight);
                
                $status = 1;
                $message = "Researcher has been added.";
                
                //send inform mail if required
                if($send_mail){
                    //get project name
                    $project = db_fetch_object(db_query("SELECT n.title title, p.nid nid FROM {research_project_page} AS p, {node} AS n WHERE p.id=%d AND p.nid=n.nid",$pid));
                    $subject_tmp = variable_get("project_invite_researcher_mail_subject","@project invite you to be a staff");
                    $subject = t($subject_tmp,array('@project'=>$project->title));
                    //TODO set it in variable_get
                    $body_tmp = "<p>Hi,</p>"
                           ."<p>@project has invited you to work for their project. The project page is @link.</p>"
                           ."<p>You can use your @site account to login.</p>"
                           ."<p>Kind regards,</p>"
                           ."<p>@site</p>";
                    $body = t($body_tmp,array(
                                            '@project'=>$project->title,
                                            '@link'=>"http://".$_SERVER['HTTP_HOST'].url('node/'.$project->nid),
                                            '@site'=>T3RC,
                                        )
                            );
                    $result=project_send_http_mail($email,$subject,$body);
                    if($result['tag']=='error'){
                        $mail_status = "Failed"; 
                    }else if($result['tag']=='status'){
                        $mail_status = "Successed"; 
                    }
                    watchdog(
                        "rproject $pid",
                        "Send invitation mail to reseacher @mail. @status",
                        array('@mail'=>$email, '@status'=>$mail_status),
                        WATCHDOG_INFO
                    );                    
                }
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
    $back_link = l("<< Back to project","research/project/$pid");
    $content.= "<p>$back_link</p>";
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
    /*$form['message'] = array(
        '#type' => 'textarea',
        '#title' => t('Invitating message'),
        '#description' => t('This message will be sent with the invitation content'),
        '#required' => TRUE,
        '#default_value' => generate_invitation_template('email',$pid),
    );*/
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
    //$message = $form_state['values']['message'];
    
    //get project info
    $sql = "SELECT n.nid nid, title from {node} AS n, {research_project_page} AS r "
          ."WHERE r.id=%d AND r.nid=n.nid";
    if($project=db_fetch_object(db_query($sql,$pid))){
	$name = $project->title;
    }
    $message_tmp = generate_invitation_template($way,$pid);
    if($way=='email'){
        $mail_list = splite_mail_list($lists,',');
        foreach($mail_list as $mail){
            //find account, if null create it.
            $user = get_user_by_mail($mail,$name);
            //check if is new user
            if($user->new_user){
                $account = "Your login account is your email address: $mail, and default password is {$user->pass}. Please change it as soon as possible.";
            }else{
                $account = "You already has an account in T3RC, your username is {$user->name}";
            }
            $message = t($message_tmp, array('@account_info'=>$account));
            
            //check if user has already linked with project
            $check = "SELECT COUNT(id) FROM {project_persons} WHERE pid=%d AND uid=%d";
            if(db_result(db_query($check,$pid,$user->uid))==0){
                //bind user with project
                $insert = "INSERT INTO {project_persons}"
                     ."(uid,pid,status,created,changed) "
                     ."VALUES(%d,%d,0,now(),now())";
                db_query($insert,$user->uid,$pid);
            }            
            //send email
            $subject =  t('@project invites you to join their research',array("@project"=>$name));
            $body = t($message);
            if($result=project_send_http_mail($mail,$subject,$body)){
                drupal_set_message($result['message'],$result['tag']);
                $user_link = l($user->name, "user/prifles/".$user->uid);
                if($result['tag']=='error'){
                   $mail_status = "Failed"; 
                }else if($result['tag']=='status'){
                   $mail_status = "Successed"; 
                }
                watchdog("rproject_$pid","Send invitation mail to @user_link. @status",array('@user_link'=>$user_link, '@status'=>$mail_status),WATCHDOG_INFO);
            }
        }
    }else if($way='sms'){
        $phone_list = splite_phone_list($lists,',');
        foreach($phone_list as $phone){
            //find account, if null create it.
            $user = get_user_by_phone($phone,$name);            
    
            //check if is new user
            if($user->new_user){
                $account = "Your login account is your phone number: $phone, and default password is {$user->pass}. Please change it as soon as possible.";
            }else{
                $account = "You already has an account in T3RC, your username is {$user->name}";
            }
            $message = t($message_tmp, array('@account_info'=>$account));
            
            //check if user has already linked with project
            $check = "SELECT COUNT(id) FROM {project_persons} WHERE pid=%d AND uid=%d";
            if(db_result(db_query($check,$pid,$user->uid))==0){
                //bind user with project
                $insert = "INSERT INTO {project_persons}"
                     ."(uid,pid,status,created,changed) "
                     ."VALUES(%d,%d,0,now(),now())";
                db_query($insert,$user->uid,$pid);
            }
            
            //send sms
            send_sms($phone,$message);
        }
        
    }
}

/**
 * Menu callback for researchpage/researcher/refresh/%
 */
function ajax_refresh_researcher_list($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    
    $module_path = str_replace("?q=","",url(drupal_get_path('module','research_project')));    
    //get researcher list
    $sql = "SELECT r.id id, r.uid uid, r.introduction introduction, u.name name, p.photo photo, p.firstname firstname, p.lastname lastname "
          ."FROM {project_researchers} AS r, {users} AS u ,{person_profiles} AS p "
          ."WHERE r.pid=%d AND r.uid=u.uid AND p.uid=r.uid "
          ."ORDER by r.weight ";
    $res = db_query($sql,$pid);
    while($researcher=db_fetch_array($res)){
        $content.= "<li>"
                  ."<img class='staff-photo' src='".(empty($researcher['photo'])?"$module_path/images/user_default.gif":$researcher['photo'])."' alt=''/>"
                  ."<span class='staff-name'><a href='{$researcher['uid']}' target='_blank'>{$researcher['name']}</a></span>"
                  ."<span class='staff-intro'>({$researcher['introduction']})</span>"
                  ."<span class='edit-text'>[<a href='javascript:open_edit_researcher_window($pid,{$researcher['id']})'>Edit</a>]</span>"
                  ."</li>";    
    }
    print $content;
}

/**
 * Menu callback for research/project/%
 */
function redirect_project_node_page($pid){
    $sql = "SELECT nid FROM {research_project_page} WHERE id=%d";
    $nid = db_result(db_query($sql,$pid));
    drupal_goto("node/$nid");
}

/**
 * Menu callback for researchpage/participants/list/%
 */
function list_participants_page($pid){
    $sql = "SELECT p.id id, p.uid uid, u.access access, pf.firstname firstname, pf.lastname lastname, pf.gender gender "
          ."FROM {project_persons} AS p, {users} AS u, {person_profiles} AS pf "
          ."WHERE p.pid=%d AND p.uid=u.uid AND p.uid=pf.uid ORDER BY p.created";
    $res = db_query($sql,$pid);
    
    $tool_bar = "<div id='project-tool-bar'>"
                ."<input type='button' value='+Invite new' onclick='javascript:window.location.href=".'"'.url("researchpage/participants/page/$pid").'"'."'/>"
                ."<input type='button' value='Send message' onclick='javascript:window.location.href=".'"'.url("researchpage/participants/message/send/$pid").'"'."'/>"
                ."<input type='button' value='View sending history' onclick='javascript:window.location.href=".'"'.url("researchpage/participants/message/history/$pid").'"'."'/>"
               ."</div>";
    
    $header = array(
        "NO.",  "Name", "Gender", "Last activity"
    );
    
    //check if current user has permissiont to edit participants list
    $edit_access = _current_user_access_edit_research_page($pid);
    if($edit_access){
        $header[] = "Operation";
    }
    
    $id = 1;
    while($person=db_fetch_object($res)){
        if(empty($person->firstname)&&empty($person->lastname)){
            $name = "Anonymous";
        }else{
            $name = $person->firstname." ".$person->lastname;
        }
        $user_link = l($name,"user/profiles/{$person->uid}");
        $row = array(            
            array('data'=>$id),
            array('data'=>$user_link),
            array('data'=>t($person->gender)),
            array('data'=>t(format_interval(time() - $person->access))),
        );
        if($edit_access){
            $remove_link = l("[Remove]", "researchpage/participants/page/$pid/remove/{$person->id}");
            $row[] = $remove_link;
        }
        $rows[] = $row;
        $id++;
    }
    $back_link = l("<< Back to project","research/project/$pid");
    $content = "<p>$back_link</p>";
    $content.= $tool_bar;
    $content.= theme('table', $header, $rows);
    $content.= "<p>$back_link</p>";
    return $content;
}

/**
 * Menu callback for researchpage/report/window/%
 */
function add_project_report_window($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $content = drupal_get_form("new_report_info_form",$pid);
    print $content;
}

/**
 * Form for new report info window
 */
function new_report_info_form(&$form_state,$pid){
    //get reports list in this project
    $reports_sql = "SELECT nid FROM {project_documents} WHERE pid=%d";
    $res1 = db_query($reports_sql,$pid);
    $reports_list = array();
    while($nid=db_result($res1)){
        $reports_list[] = $nid;
    }
    
    //get current users node list
    global $user;
    $node_sql = "SELECT nid, title FROM {node} WHERE uid=%d AND type<>'researchproject' ";
    $res2 = db_query($node_sql,$user->uid);
    $node_list = array();
    while($node=db_fetch_object($res2)){
        if(in_array($node->nid,$reports_list)){
            continue;
        }
        $node_list[''.$node->nid] = $node->title;
    }
    
    $form['pid'] = array(
        '#type' => 'hidden',
        '#value' => $pid,
    );
    $form['report'] = array(
        '#type' => 'select',
        '#title' => t('Select artical'),
        '#options' => $node_list,
        '#description' => l('Create new content','node/add', array('attributes' => array('target'=>'_blank'))),
        '#required' => TRUE,    
    );
    $form['introduction'] = array(
        '#type' => 'textarea',
        '#title' => t('Short introduction'),
        '#description' => t('Introduction should less than 255 chars'),
        '#required' => TRUE,
    );
    $form['weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#description' => t('Change the weight to order reports'),
    );
    $form['submit'] = array(
        '#value' => "<input type='button' value='Add' onclick='add_new_report_to_project($pid)'/>",
    );
    return $form;
}


/**
 * Menu callback for researchpage/report/bind/%
 */
function ajax_bind_report_to_project($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $nid = (int)$_POST['nid'];
    $introduction = $_POST['intro'];
    $weight = (int) $_POST['weight'];
    
    if($nid<=0||empty($introduction)){
        $status = 0;
        $message = "Invaldate parameters";
    }else{
        //check if node already in project document table
        $check = "SELECT id FROM {project_documents} WHERE pid=%d AND nid=%d";
        $id = db_result(db_query($check,$pid,$nid));
        if($id){
            //Update
            $update = "UPDATE {project_documents} SET introduction='%s', weight=%d WHERE id=%d";
            if(db_query($update,$introduction,$weight,$id)){
                $status = 1;
                $message = "Successed to update changes.";
            }else{
                $status = 0;
                $message = "SQL error.";
            }
        }else{
            //Insert new
            $insert = "INSERT INTO {project_documents}(nid,pid,introduction,weight) VALUES(%d,%d,'%s',%d)";
            if(db_query($insert,$nid,$pid,$introduction,$weight)){
                $status = 1;
                $message = "Successed to bind new report with project.";
            }else{
                $status = 0;
                $message = "SQL error.";
            }
        }
    }
    printf('{"status":%d,"message":"%s"}',$status,$message);
    
}

/**
 * Menu callback for researchpage/report/window/%/edit/%
 */
function edit_report_in_project_window($did){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    //get report info
    $get = "SELECT d.id id, d.nid nid, d.pid pid, d.introduction introduction, d.weight weight, n.title "
          ."FROM {project_documents} AS d, {node} AS n WHERE d.id=%d AND n.nid=d.nid";
    $res = db_query($get,$did);
    if($report=db_fetch_object($res)){
        $content = "<h3>Title: {$report->title}</h3>";
        $content.= drupal_get_form("edit_report_info_form",$report);
    }else{
        $content = "<h2>Can not find the report information.</h2>";
    }
    print $content;
}
/**
 * Form for edit report info window
 */
function edit_report_info_form(&$form_state,$report){
    $form['pid'] = array(
        '#type' => 'hidden',
        '#value' => $report->pid,
    );
    $form['nid'] = array(
        '#type' => 'hidden',
        '#value' => $report->nid,
    );
    $form['introduction'] = array(
        '#type' => 'textarea',
        '#title' => t('Short introduction'),
        '#description' => t('Introduction should less than 255 chars'),
        '#required' => TRUE,
        '#default_value' => $report->introduction,
    );
    $form['weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#description' => t('Change the weight to order reports'),
        '#default_value' => $report->weight,
    );
    $form['submit'] = array(
        '#value' => "<input type='button' value='Save' onclick='save_report_change_in_project()'/> <input type='button' value='Remove' onclick='remove_report_from_project({$report->pid},{$report->id})'/>",
    );
    
    return $form;
}

/**
 * Menu callback for researchpage/report/window/%/delete/%
 */
function ajax_delete_report_from_project($did){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $delete = "DELETE FROM {project_documents} WHERE id=%d";
    if(db_query($delete,$did)){
        $status = 1;
        $message = "Successed to remove report form project.";
    }else{
        $status = 0;
        $message = "SQL error";
    }
    
    printf('{"status":%d,"message":"%s"}',$status,$message);
}

/**
 * Menu callback for researchpage/reports/refresh/%
 */
function refresh_reports_list_in_project($pid){
    drupal_set_header('Content-Type: text/plain; charset: utf-8');
    $get_documents_sql = "SELECT d.id id, n.nid nid, n.title title, d.introduction introduction FROM {project_documents} AS d, {node} AS n WHERE pid=%d AND d.nid=n.nid ORDER by d.weight";
    $res = db_query($get_documents_sql,$pid);
    
    $content = "<ul>";
    while($document = db_fetch_array($res)){
        $content.= "<li>"
                  ."<a href='".url("node/{$document['nid']}")."' target='_blank'>{$document['title']}</a> "
                  ."<span class='edit-text'><a href='javascript:display_edit_report_window($pid,{$document['id']})'>[Edit]</a></span>"
                  ."<p>{$document['introduction']}</p>"
                  ."</li>";
    }
    $content.= "</ul>";
    print $content;
}

/**
 * Menu callback for researchpage/participants/page/%/remove/%
 */
function remove_person_from_project_callback($ppid){
    //get remove person info
    $sql = "SELECT pp.id id, pp.pid pid, u.name name, u.mail mail FROM {project_persons} AS pp, {users} AS u WHERE pp.id=%d AND pp.uid=u.uid";
    $person = db_fetch_object(db_query($sql,$ppid));
    if($person){
        $content = "<h3><font color='red'>You are removing {$person->name} from this project.</font></h3>";
        $content.= drupal_get_form('remove_person_from_project_form',$person);
        return $content;
    }else{
        return "<p>No such person can be found from project.</p>";
    }
}
/**
 * Form for remove person from project
 */
function remove_person_from_project_form(&$form_state,$person){
    $cancel_link = "researchpage/participants/list/".$person->pid;
    $form['pid'] = array(
        '#type' => 'hidden',
        '#value' => $person->pid,
    );
    $form['ppid'] = array(
        '#type' => 'hidden',
        '#value' => $person->id,
    );
    $form['sumbit'] = array(
        '#type' => 'submit',
        '#value' => 'Remove',
        '#suffix' => l(" Or cancel?",$cancel_link),
    );
    return $form;
}
/**
 * Handle submit action for remove person from project form
 */
function remove_person_from_project_form_submit($form, &$form_state){
    $pid = $form_state['values']['pid'];
    $id = $form_state['values']['ppid'];
    
    $remove = "DELETE FROM {project_persons} WHERE id=%d";
    if(db_query($remove,$id)){
        drupal_goto("researchpage/participants/list/$pid");
    }else{
        drupal_set_message("Failed to remove person from project.", "error");
    }
}
/**
 * Menu callback for researchpage/participants/message/send/%
 */
function send_message_to_participants_page($pid){
    $content = drupal_get_form('message_sending_to_participants_form',$pid);
    return $content;
}
/**
 * Form for message sending
 */
function message_sending_to_participants_form(&$form_state,$pid){
    $option = array(
        'email' => 'By email',
        'sms' => 'By SMS',
    );
    $form['pid'] = array(
        '#type' => 'hidden',
        '#value' => $pid,
    );
    $form['send-way'] = array(
        "#type" => 'radios',
        "#title" => t('Send way'),
        "#description" => t('The way to send message'),
        '#options' => $option,
        '#default_value' => 'email',
        '#required' => TRUE,
    );
    $form['subject'] = array(
        '#type' => 'textfield',
        '#title' => t('Subject'),
        '#description' => t('If you are sending SMS, just left subject empty'),
    );
    $form['message'] = array(
        "#type" => 'textarea',
        "#title" => 'Message content',
        "#description" => 'If you are sending SMS, please keep it concise.',
        '#required' => TRUE,
    );
    $form['submit'] = array(
        "#type" => 'submit',
        '#value' => 'Send'
    );
    return $form;
}
/**
 * Handle validation for form message sending
 */
function message_sending_to_participants_form_validate($form, &$form_state){
    $way = $form_state['values']['send-way'];
    $message = $form_state['values']['message'];
    if($way=='sms'){
        $message = strip_tags($message);
        if(strlen($message)>460){
            form_set_error('message','The message is too long.');
        }
    }else if($way=='email'){
        $subject = trim(strip_tags($form_state['values']['subject']));
        if(strlen($subject)==0){
            form_set_error('subject','Plesase input a subject for email.');
        }
    }
}
/**
 * Handle submit for form message sending
 */
function message_sending_to_participants_form_submit($form, &$form_state){
    $pid = $form_state['values']['pid'];
    $way = $form_state['values']['send-way'];
    $message = $form_state['values']['message'];
    
    if($way=='email'){
        //get all user email
        $mails_sql = "SELECT u.mail mail FROM {users} AS u, {project_persons} AS p WHERE p.pid=%d AND p.uid = u.uid";
        $res = db_query($mails_sql,$pid);
        
        while($record=db_fetch_object($res)){
            $mail = $record->mail;
            if(empty($mail)){
                continue;
            }
            $mail_list[] = $mail;            
            $to_mails.= $mail.', '; 
        }
        $subject = trim(strip_tags($form_state['values']['subject']));
        $result = project_send_http_mail($mail_list,$subject,$message);
        if($result['tag'] == 'status'){
            drupal_set_message("Successed to send mail to $to_mails","status");
        }else{
            drupal_set_message($result['message'],"error");
        }
        
    }else if($way=='sms'){
        
    }
}
/**
 * Menu callback for researchpage/participants/message/history/%
 */
function list_message_sending_records_page($pid){
    
}