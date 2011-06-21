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
function edit_tools(id){
    //empty dialog content
    $("#edit-dialog-content").html("");
    
    var $dialog = $("#edit-dialog-div");
    $dialog.dialog( "option", "title", 'Select research tools');
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
    $dialog.dialog( "option", "title", 'Invite new researhers');
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
    if(intro){
        var url = Drupal.settings.research_project.research_invite_new+id;
        $.post(url,{"email":email,"introduction":intro},function(data){
            var result = jQuery.parseJSON(data);
            if(result.status){
                if(resutl.status==1){
                    $("#edit-dialog-div").dialog('close');
                }else{
                    alert(result.message);
                }               
            }else{
                alert("Unknow error happened.");
            }
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
