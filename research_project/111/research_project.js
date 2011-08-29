//this js required jquery.js and jquery-window plugin

$(function(){
    $("#edit-dialog-div").dialog({
        autoOpen: false,
        minWidth: 520,
        modal: true
    });    
});

/**
 * Button onclick function
 * Open select tools dialog
 * @param
 *  id: number, the id of project
 */
function test(){
	alert("fdfew");
}


/**
 * Button onclick function
 * Open select tools dialog
 * @param
 *  id: number, the id of project
 */
function edit_tools(id){
    //empty dialog content
    $("#edit-dialog-content").html("");
    
    var $dialog = $("#edit-dialog-div");
    $dialog.dialog( "option", "title", ' ');
    $dialog.dialog('open');
    $("#onloading-div").css('display','block');
    
    var url = Drupal.settings.research_project.tool_window+id;
    $.get(url,function(data){
        $("#onloading-div").css('display','none');
        $("#edit-dialog-content").css('display','block');
        $("#edit-dialog-content").html(data);
    });  
}

/**
 * Button onclick function
 * Link selected tools with the project
 * @param
 *  id: number, the id of project
 */
function save_selected_tools(id){
    var check_list = "";
    $('input[name^="tool_checkbox_"]:checked').each(function(){
        check_list+= $(this).val()+",";        
    });
    
    var url = Drupal.settings.research_project.tool_changed+id;
    $.post(url,{"checkeds":check_list},function(data){
        var result = jQuery.parseJSON(data);
        if(result.status==1){
            $("#edit-dialog-div").dialog('close');
            var refresh_url = Drupal.settings.research_project.tool_refresh+id;
            $('#tools-list').load(refresh_url);
            //alert(data);
        }else{
            alert("Failed to save changes");
        }
    });
}

/**
 * Button onclick function
 * Show add new researcher window
 * @param
 *  id: number, the id of project
 */
function show_new_researcher_window(id){    
    var $dialog = $("#edit-dialog-div");
    $dialog.dialog( "option", "title", ' ');
    $dialog.dialog('open');
    $("#onloading-div").css('display','none');
    var url = Drupal.settings.research_project.research_window+id
    $("#edit-dialog-content").load(url);
}

/**
 * Button onclick function
 */
function invite_new_researcher(id){
    //check forms
    var email = $('#edit-email').val();
    var intro = $.trim($('#edit-introduction').val());
    var weight = $('#edit-weight').val();
    var send = $('#edit-send-mail').attr('checked');
    if(intro){
        if(send){
            var send_mail = 1;
        }else{
            send_mail = 0;
        }
        var url = Drupal.settings.research_project.research_invite_new+id;
        $.post(url,{"email":email,"introduction":intro,"weight":weight,"send":send_mail},function(data){
            /*var result = jQuery.parseJSON(data);
            if(result.status){
                if(result.status==1){
                    $("#edit-dialog-div").dialog('close');
                    var refresh = Drupal.settings.research_project.research_refresh+id;
                    $("#researchers-list").load(refresh);
                }else{
                    alert(result.message);
                }               
            }else{
                alert("Unknow error happened.");
            }*/
            $("#edit-dialog-div").dialog('close');
            var refresh = Drupal.settings.research_project.research_refresh+id;
            $("#researchers-list").load(refresh);
        });
    }else{
        alert("Please input a introduction for your researcher.");
    }
}

/**
 * Handle when email address changed
 */
function check_researcher_email_address(id){
    var email = $('#edit-email').val();
    var url = Drupal.settings.research_project.research_email_check+id;
    $.post(url,{"email":email},function(data){
        var result = jQuery.parseJSON(data);
        if(result.message){
            if(result.status==1){
                $("#check-research-email-result-div").css('color','blue');
                $("#invite_new_researcher_button").removeAttr('disabled');
            }else{
                $("#invite_new_researcher_button").attr('disabled','disabled');
                $("#check-research-email-result-div").css('color','red');
            }
            $("#check-research-email-result-div").html(result.message);
            
        }        
    });
}

/**
 * Link onclick function
 * @param
 *   pid
 *     int, index of project
 * @param
 *   rid
 *     int, index of researcher in the project
 */
function open_edit_researcher_window(pid,rid){
    var $dialog = $("#edit-dialog-div");
    $dialog.dialog( "option", "title", 'Edit researher');
    $dialog.dialog('open');
    $("#onloading-div").css('display','none');
    var url = Drupal.settings.research_project.research_window+pid+"/edit/"+rid;
    $("#edit-dialog-content").load(url);
}

/**
 * Button onclick function
 * @param
 *   pid
 *     int, index of project
 * @param
 *   rid
 *     int, index of researcher in the project
 */
function save_edit_researcher_change(pid,rid){
    var intro = $('#edit-introduction').val();
    var weight = $('#edit-weight').val();
    var url = Drupal.settings.research_project.research_window+pid+"/save/"+rid;
    $.post(url,{"intro":intro,"weight":weight},function(data){
        var result = jQuery.parseJSON(data);
        if(result.status==1){
            $("#edit-dialog-div").dialog('close');
            var refresh = Drupal.settings.research_project.research_refresh+pid;
            $("#researchers-list").load(refresh);
        }else{
            alert("Failed to save changes");
        }        
    });
    
}

/**
 * Button onclick function
 * @param
 *   pid
 *     int, index of project
 * @param
 *   rid
 *     int, index of researcher in the project
 */
function remove_researcher_from_project(pid,rid){
    if(confirm("Are you real want to remove this researcher from project?")){
        var url = Drupal.settings.research_project.research_window+pid+"/delete/"+rid;
        $.get(url,function(data){
            var result = jQuery.parseJSON(data);
            if(result.status==1){
                $("#edit-dialog-div").dialog('close');
                var refresh = Drupal.settings.research_project.research_refresh+pid;
                $("#researchers-list").load(refresh);
            }else{
                alert("Failed to remove researcher from project.");
            }        
        });
    }
}
/**
 * Button onclick function
 * @param
 *   pid
 *     int, index of project
 */
function display_add_new_report_window(pid){
    var $dialog = $("#edit-dialog-div");
    $dialog.dialog( "option", "title", ' ');
    $dialog.dialog('open');
    $("#onloading-div").css('display','none');
    var url = Drupal.settings.research_project.report_window+pid;
    $("#edit-dialog-content").load(url);
    
}
/**
 * Button onclick function
 * @param
 *   pid
 *     int, index of project
 */
function add_new_report_to_project(pid){
    var nid = $("#edit-report").val();
    var intro =$.trim($("#edit-introduction").val());
    var weight = $("#edit-weight").val();
    
    if(intro==''){
        alert("Please put something for introduction!");
        return;
    }
    var url = Drupal.settings.research_project.report_bind+pid;
    $.post(url,{"nid":nid,"intro":intro,"weight":weight},function(data){
        var result = jQuery.parseJSON(data);
        if(result&&result.status==1){
            $("#edit-dialog-div").dialog('close');
            var refresh = Drupal.settings.research_project.report_refresh+pid;
            $("#reports-list").load(refresh);
        }else{
            alert("Failed to bind report with project.");
        }
    });
}
/**
 * Button onclick function
 * @param
 *   pid
 *     int, index of project
 * @param
 *   did
 *     int, index of document in the project
 */
function display_edit_report_window(pid,did){
    var $dialog = $("#edit-dialog-div");
    $dialog.dialog( "option", "title", 'Edit document');
    $dialog.dialog('open');
    $("#onloading-div").css('display','none');
    var url = Drupal.settings.research_project.report_window+pid+'/edit/'+did;
    $("#edit-dialog-content").load(url);
}

/**
 * Button onclick function
 */
function save_report_change_in_project(){
    var pid = $('#edit-pid').val();
    var nid = $('#edit-nid').val();
    var intro = $('#edit-introduction').val();
    var weight = $('#edit-weight').val();
    
    var url = Drupal.settings.research_project.report_bind+pid;
    $.post(url,{"nid":nid,"intro":intro,"weight":weight},function(data){
        var result = jQuery.parseJSON(data);
        if(result&&result.status==1){
            $("#edit-dialog-div").dialog('close');
            var refresh = Drupal.settings.research_project.report_refresh+pid;
            $("#reports-list").load(refresh);
        }else{
            alert("Failed to bind report with project.");
        }
    });
}

/**
 * Button onclick function
 * @param
 *   pid
 *     int, index of project
 * @param
 *   did
 *     int, index of document in the project
 */
function remove_report_from_project(pid,did){
    if(confirm("Are you real want to remove this document from project?")){
        var url = Drupal.settings.research_project.report_window+pid+"/delete/"+did;
        $.get(url,function(data){
            var result = jQuery.parseJSON(data);
            if(result&&result.status==1){
                $("#edit-dialog-div").dialog('close');
                var refresh = Drupal.settings.research_project.report_refresh+pid;
                $("#reports-list").load(refresh);
            }else{
                alert("Failed to remove document from project.");
            }        
        });
    }
}