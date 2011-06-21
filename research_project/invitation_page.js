/**
 * Handle when select invitation way changed
 */
function invite_way_onchange(){
    var way = $('input[name="invite-way"]:checked').val();
    if(way=='email'){
        var message = $('#edit-email-message').val();
    }else if(way=='sms'){
        var message = $('#edit-sms-message').val();
    }
    $('#edit-message').val(message);
}