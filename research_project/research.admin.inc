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
        "Logo",  "Tool Name", "Description", "Operations"  
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
        '#title' => 'Tool name',
        '#required' => TRUE,
        '#default_value' => isset($tool->name)?$tool->name:"",
    );
    $form['description'] = array(
        '#type' => 'textarea',
        '#title' => 'Description',
        '#required' => TRUE,
        '#default_value' => isset($tool->description)?$tool->description:"",
    );
    $form['guidance'] = array(
        '#type' => 'textarea',
        '#title' => 'Use of guidance',
        '#description' => t('Please input guidance of how to use this tool.'),
        '#required' => TRUE,
        '#default_value' => isset($tool->guidance)?$tool->guidance:"",
    );
    $form['download'] = array(
        '#type' => 'textfield',
        '#title' => 'Download Link',
        '#description' => t('The link to download the tool, if it has.'),
        '#default_value' => isset($tool->download)?$tool->download:"",
    );
    $form['logo'] = array(
        '#type'  => 'file',
        '#title' => 'Tool logo',
        '#description' => t('Upload a image file as the logo of the tool. If no change, keep this empty.'),
        '#prefix' => empty($tool->logo)?"":"<img src='{$tool->logo}' height='80px'/>",
    );
    $form['logo-url'] = array(
        '#type'  => 'hidden',
        '#default_value' => isset($tool->logo)?$tool->logo:"",
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Save',
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
        $content="<p><font color='red'>No tool info can be found.</font></p>";
        return $content;
    }
}

/**
 * Menu callback for admin/content/researchtools/delete/%
 */
function delete_research_tools_callback($tool_id){
    $sql = "SELECT id, name from {research_tool} WHERE id=%d";
    if($tool = db_fetch_object(db_query($sql,$tool_id))){
        $content = "You are deleting research tool <strong>{$tool->name}</strong>. ";
        $content.= drupal_get_form('_delete_research_tool_info',$tool);
        return $content;
    }else{
        $content="<p><font color='red'>No tool info can be found.</font></p>";
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
        '#value' => 'Delete',
        '#suffix' => l(" Or cancel?",$cancel_link),
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