<?php
// $Id$
/**
 * @file
 * admin page callbacks for research project management module.
 */

/**
 * Menu callback for admin/content/researchtools
 */
function list_research_tools_callback(){
    //get all research tools
    $sql = "SELECT id, name, description, logo from {research_tool} ORDER BY created";
    $res = db_query($sql);
    $header = array(
        t("Logo"),  t("Tool Name"), t("Description"), t("Operations")  
    );
    while($tool=db_fetch_object($res)){
        if(empty($tool->logo)){
            $logo = "&lt;No logo&gt; ";
        }else{
            $logo = "<img src='{$tool->logo}' height='40px' alt='{$tool->name}'/>";
        }
        $view_link = "research/tools/{$tool->id}";
        $edit_link = "admin/content/researchtools/edit/{$tool->id}";
        $del_link = "admin/content/researchtools/delete/{$tool->id}";
        $short_des = substr($tool->description,0,40);
        $rows[] = array(
            array('data'=>$logo),  
            array('data'=>l($tool->name,$view_link)),
            array('data'=>l($short_des,$view_link)),
            array('data'=>l("Edit",$edit_link)." | ".l("Delete",$del_link)),
        );
    }
    $content= theme('table', $header, $rows);
    return $content;
}

/**
 * Menu callback for admin/content/researchtools/add
 */
function add_research_tools_callback(){
    return drupal_get_form('_research_tool_info');
}
/**
 * form for research tool info
 */
function _research_tool_info(&$form_state,$tool=NULL){
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['id'] = array(
        '#type' => 'hidden',
        '#default_value' => isset($tool->id)?$tool->id:0,
    );
    $form['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Tool Name'),
        '#required' => TRUE,
        '#default_value' => isset($tool->name)?$tool->name:"",
    );
    $form['description'] = array(
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#required' => TRUE,
        '#default_value' => isset($tool->description)?$tool->description:"",
    );
    $form['guidance'] = array(
        '#type' => 'textarea',
        '#title' => t('Use of guidance'),
       /** '#description' => t('Please input guidance of how to use this tool.'),*/
        '#required' => TRUE,
        '#default_value' => isset($tool->guidance)?$tool->guidance:"",
    );
    $form['download'] = array(
        '#type' => 'textfield',
        '#title' => t('Data view link'),
       /** '#description' => t('The link to download the tool, if it has.'),*/
        '#default_value' => isset($tool->download)?$tool->download:"",
    );
    $form['logo'] = array(
        '#type'  => 'file',
        '#title' => t('Tool logo'),
        '#description' => t('Upload a image file as the logo of the tool. If no change, keep this empty'),
        '#prefix' => empty($tool->logo)?"":"<img src='{$tool->logo}' height='80px'/>",
    );
    $form['logo-url'] = array(
        '#type'  => 'hidden',
        '#default_value' => isset($tool->logo)?$tool->logo:"",
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}

/**
 * Handel submit for research tool info
 */
function _research_tool_info_submit($form, &$form_state){
    $id = $form_state['values']['id'];
    $name = $form_state['values']['name'];
    $description = $form_state['values']['description'];
    $guidance = $form_state['values']['guidance'];
    $download = $form_state['values']['download'];
    
    //Upload image
    $validators = array(
        'file_validate_is_image' => array(),
    );
    $file = file_save_upload('logo', $validators);
    if($file!=0){
        $info = image_get_info($file->filepath);
        //create tools_logo folder if not existed
        $folder = "tools_logo";
        $path = file_create_path($folder);
        file_check_directory($path, FILE_CREATE_DIRECTORY);
        
        $destination = $folder.'/'.time().'.'. $info['extension'];
        if(file_copy($file, $destination, FILE_EXISTS_REPLACE)){
	    $logo = url($file->filepath);
            $logo = str_replace("?q=","",$logo);
        }else{
            //keep old logo
            $logo = $form_state['values']['logo-url'];
        }
    }else{
        //no change in logo
        $logo = $form_state['values']['logo-url'];
    }
    if($id==0){
        
        //Insert into database
        $insert = "INSERT INTO {research_tool}"
                 ."(name,description,guidance, download, logo, created, changed) "
                 ."VALUES('%s','%s','%s','%s','%s',now(),now())";
        db_query($insert,$name,$description,$guidance,$download,$logo);
        drupal_goto("admin/content/researchtools");
    }else{
        //Edit
        $update = "UPDATE {research_tool} "
                 ."SET name='%s', description='%s', guidance='%s', download='%s', logo='%s', changed=now() "
                 ."WHERE id=%d";
        db_query($update,$name,$description,$guidance,$download,$logo,$id);
        drupal_goto("research/tools/$id");
    }
}

/**
 * Menu callback for admin/content/researchtools/edit/%
 */
function edit_research_tools_callback($tool_id){
    //get tool info
    $sql = "SELECT id, name, description, guidance, download, logo from {research_tool} WHERE id=%d";
    if($tool = db_fetch_object(db_query($sql,$tool_id))){
        return drupal_get_form('_research_tool_info',$tool);
    }else{
        $content=t("<p><font color='red'>No tool info can be found.</font></p>");
        return $content;
    }
}

/**
 * Menu callback for admin/content/researchtools/delete/%
 */
function delete_research_tools_callback($tool_id){
    $sql = "SELECT id, name from {research_tool} WHERE id=%d";
    if($tool = db_fetch_object(db_query($sql,$tool_id))){
        $content = "<strong>You are deleting research tool <strong>{$tool->name}</strong>. ";
        $content.= drupal_get_form('_delete_research_tool_info',$tool);
        return $content;
    }else{
        $content=t("<p><font color='red'>No tool info can be found.</font></p>");
        return $content;
    }
}

function _delete_research_tool_info(&$form_state,$tool){
    $cancel_link = "admin/content/researchtools/";
    $form['id'] = array(
        '#type' => 'hidden',
        '#value' => $tool->id,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Delete'),
        '#suffix' => l(t("Or cancel?"),$cancel_link),
    );
    return $form;
}

function _delete_research_tool_info_submit($form, &$form_state){
    $id = $form_state['values']['id'];
    //delete from database
    $sql = "DELETE FROM {research_tool} WHERE id=%d";
    db_query($sql,$id);
    drupal_goto("admin/content/researchtools/");
}

/**
 * Menu callback for admin/settings/sms
 */
function sms_settings_callback(){
    return drupal_get_form("sms_settings_info_form");
}

/**
 * Form for sms settings info
 */
function sms_settings_info_form(){
    $form['username'] = array(
        '#title' => t('Username'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => variable_get('sms_server_username', ''),
    );
    $form['password'] = array(
        '#title' => t('Password'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => variable_get('sms_server_password', ''),
    );
    $form['server-address'] = array(
        '#title' => t('Server Address'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => variable_get('sms_server_address', ''),
    );
    $form['send-test-message'] = array(
        '#title' => t('send test message to'),
        '#type' => 'textfield',
        '#required' => FALSE,
        '#description' => t('Input a phone number'),
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}
/**
 * Handle submit for  sms settings info form
 */
function sms_settings_info_form_submit($form, &$form_state){
    $username = $form_state['values']['username'];
    $password = $form_state['values']['password'];
    $server = $form_state['values']['server-address'];
    $message_to = $form_state['values']['send-test-message'];
    
    variable_set('sms_server_username', $username);
    variable_set('sms_server_password', $password);
    variable_set('sms_server_address', $server);
    
    drupal_set_message("Save SMS settings changes successfully","status");
    
    if(!empty($message_to)){
        send_sms($message_to,"This is a test message.");
    }
    
}

/**
 * Menu callback for admin/settings/researchproject
 */
function researchproject_settings_callback(){
    return drupal_get_form('message_template_form');
}
/**
 * Menu callback for admin/settings/researchproject/termofuse
 */
function termofuse_settings_callback(){
    return drupal_get_form('termofuse_template_form');
}
/**
 * Form for message template settings
 */
function message_template_form(){
    $form['inform_researcher'] = array(
        '#type' => 'textarea',
        '#title' => t('Email template for inform researcher'),
        '#default_value' => variable_get('resarch_project_inform_researcher_tpl',''),
        '#required' => TRUE,
    );
    $form['invite_participant_mail_subject'] = array(
        '#type' => 'textfield',
        '#title' => t('Email subject template for invite new participant'),
        '#default_value' => variable_get('resarch_project_invite_mail_subject_tpl',''),
        /**'#description' => t('This is mainly use for sending account info.'),*/
        '#required' => TRUE,
    );
    $form['invite_participant_new_mail'] = array(
        '#type' => 'textarea',
        '#title' => t('Email template for invite new participant with new created account'),
        '#default_value' => variable_get('resarch_project_invite_mail_new_tpl',''),
        '#description' => t('This mail is sended when the participant is new for the site.'),
        '#required' => TRUE,
    );
    $form['invite_participant_mail'] = array(
        '#type' => 'textarea',
        '#title' => t('Email template for invite new participant'),
        '#default_value' => variable_get('resarch_project_invite_mail_tpl',''),
        '#description' => t('This is mainly use for sending account info.'),
        '#required' => TRUE,
    );
    $form['invite_participant_sms'] = array(
        '#type' => 'textarea',
        '#title' => t('SMS template for invite new participant'),
        '#default_value' => variable_get('resarch_project_invite_SMS_tpl',''),
        '#description' => t('This is mainly use for sending account info. Please keep it as short as possible. Less than 160 chars is suggestion'),
        '#required' => TRUE,
    );
    $form['invite_participant_new_sms'] = array(
        '#type' => 'textarea',
        '#title' => t('SMS template for invite new participant with new created account'),
        '#default_value' => variable_get('resarch_project_invite_SMS_new_tpl',''),
        '#description' => t('This is sended when the participant is new for the site. Please keep it as short as possible. Less than 160 chars is suggestion'),
        '#required' => TRUE,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
    
}

/**
 * Handle submit for message template settings form
 */
function message_template_form_submit($form, &$form_state){
    $inform = $form_state['values']['inform_researcher'];
    $invite_mail_subject = $form_state['values']['invite_participant_mail_subject'];
    $invite_mail_new = $form_state['values']['invite_participant_new_mail'];
    $invite_mail = $form_state['values']['invite_participant_mail'];
    $invite_sms_new = $form_state['values']['invite_participant_new_sms'];
    $invite_sms = strip_tags($form_state['values']['invite_participant_sms']);
    
    variable_set('resarch_project_inform_researcher_tpl', $inform);
    variable_set('resarch_project_invite_mail_subject_tpl',$invite_mail_subject);
    variable_set('resarch_project_invite_mail_new_tpl',$invite_mail_new);
    variable_set('resarch_project_invite_SMS_new_tpl',$invite_sms_new);
    variable_set('resarch_project_invite_mail_tpl', $invite_mail);
    variable_set('resarch_project_invite_SMS_tpl', $invite_sms);
    
    
}

/**
 * Form for term of use template settings
 */
function termofuse_template_form(){
    $form['inform_researcher'] = array(
        '#type' => 'textarea',
        '#title' => t('Term of use template for projects'),
        '#default_value' => variable_get('termofuse_project_inform_researcher_tpl',''),
        '#required' => TRUE,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
    
}

/**
 * Handle submit for termofuse template settings form
 */
function termofuse_template_form_submit($form, &$form_state){
    $inform = $form_state['values']['inform_researcher'];
    variable_set('termofuse_project_inform_researcher_tpl', $inform);
    
}