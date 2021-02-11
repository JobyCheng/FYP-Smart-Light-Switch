function ResponsiveNav() {
  $("#navbar").toggleClass("responsive");
}

function openTab() {
  var tab_id = location.hash.replace("#", "");

  $(".active").removeClass("active");
  $(".tabContent").css("display", "none");

  $("#" + tab_id + "-tab").addClass("active");
  $("#" + tab_id).css("display", "block");

  if ($("#navbar.responsive").length == 1) { $(".navbar .icon").click(); }
}


window.onhashchange = openTab;

$(function () { //shorthand document.ready function
  if (location.hash == "") {
    location.hash = "#home"
  } else {
    openTab();
  }

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