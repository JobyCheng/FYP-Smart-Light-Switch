$(function () { //shorthand document.ready function
console.log("access_point page loaded")

// Replace the original post function
  $('#AP_passwd_form').on('submit', function (e) {
        e.preventDefault();  //prevent form from submitting
        var info = $("#AP_passwd_form :input").serializeArray();
        for (item of info) {
          if (item.name != "passwd") { continue; }
          if (!(item.value.length >= 8)) { $(AP_respond).html("Not engought digit"); return; }
          if (!(item.value.length < 64)) { $(AP_respond).html("To much digit"); return; }
        }
    
        $.post(
          "/AP_passwd",
          info,
          function(data, status){
            console.log(data)
            $(AP_respond).html(data);
          });
      });
})
