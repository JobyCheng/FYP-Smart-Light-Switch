function ResponsiveNav() {
  $("#navbar").toggleClass("responsive");
}

function openTab() {
  var tab_id = location.hash.replace("#", "");

  $(".active").removeClass("active");

  $("#" + tab_id).addClass("active");

  $("#tabContent").load(tab_id+"/index.html");

  if ($("#navbar.responsive").length == 1) { $(".navbar .icon").click(); }
}

window.onhashchange = openTab;

$(function () { //shorthand document.ready function
  if (location.hash == "") {
    location.hash = "#home"
  } else {
    openTab();
  }
})