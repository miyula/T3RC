$(document).ready(function(){
    $('.tab-window-link').each(function(){
       $(this).bind('click',function(){
           
            //alert($(this).attr('href'));
            $.window({
                //showModal: true,
                //modalOpacity: 0.5,
                url: $(this).attr('href'),
                width: $(window).width()-200,
                height: $(window).height()-100
            });
            return false;
       });
    });
});