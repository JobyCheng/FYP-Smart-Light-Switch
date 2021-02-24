function ResponsiveNav() {
  $("#navbar").toggleClass("responsive");
}

function openTab() {
  var tab_id = location.hash.replace("#", "");

  $(".active").removeClass("active");
  $("#" + tab_id).addClass("active");

  $(".content").css("display","none")
  $("#"+tab_id+"-content").css("display","block")

  if ($("#navbar.responsive").length == 1) { $(".navbar .icon").click(); }
}

wifilock = false;
function updateWifiStauts(){
	if (wifilock) {console.log("Please wait for a few second.");return;}
    wifilock = true;
	$.get("/wifiStauts",function(data,status){
		$("#wifiStatus").html(data)
		wifilock = false;
	});
}

ssidlock = false;
function updateSSIDlist(){
	if (ssidlock) {console.log("Please wait for a few second.");return;}
    ssidlock = true;
	  $.get("/SSIDlist",function(data,status){
      //console.log(data);
      $("#SSID").empty();
      var content = "<optgroup label='Search Result'>"
      for (item of data){
        content += "<option value='"+item.ssid+"'>"+item.ssid+"</option>";
      }
      content += "<optgroup label='---------'><option value='hidden'>Hidden Network</option>"
      $("#SSID").html(content);
      ssidlock = false;
	});
}

function addClient(data){
  for (item of data){
  $("#switchList").append('\
    <tr>\
    <td>'+item.identifier+'</td>\
    <td>\
      <label class="switch">\
        <input type="checkbox" id='+item.id+'>\
        <span class="slider round"></span>\
      </label>\
    </td>\
    </tr>\
    ');

    var target = $("#"+item.id)
    target.prop("checked",item.status)

    target.change(function(){
      //console.log(target.prop("checked"))
      $.get("http://"+target.prop("id")+".local/"+(target.prop("checked")?"on":"off"))
    });
  }
}

function getClientList(){
  $.get("/getClient",function(data,status){
    //console.log(data);
    addClient(data);
  });
}

window.onhashchange = openTab;


//shorthand document.ready function
//like "setup()" in arduino
$(function () {
  if (location.hash == "") {
    location.hash = "#home"
  } else {
    openTab();
  }
  getClientList();
  updateWifiStauts();
  updateSSIDlist();

  // Replace the original post function
  $('#AP_passwd_form').on('submit', function (e) {
    e.preventDefault();  //prevent form from submitting
    var form = $("#AP_passwd_form");
    var passwd = form.find("input[name=passwd]")[0];
    var confirm = form.find("input[name=confirm]")[0];
    if ((passwd.value.length < 8)) { console.log("password too short"); $(AP_respond).html("Not engought digit."); return; }
    if ((passwd.value.length > 63)) { console.log("password too long"); $(AP_respond).html("To much digit."); return; }
    if ((passwd.value != confirm.value)) {console.log("password not equal"); $(AP_respond).html("The two entry doesn't match."); return; }
    
    $.post(
      "/AP_passwd",
      form.serializeArray(),
      function(data, status){
        $(AP_respond).html(data);
      });
  });

  $("#SSID").change(function(){
    if ($("#Wifi_form select[name=SSID]").serializeArray()[0].value == "hidden"){
      $("#hidden_input").css("display","block");
    }else{
      $("#hidden_input").css("display","none");
    }
  })

  $('#Wifi_form').on('submit', function (e) {
    e.preventDefault();  //prevent form from submitting
    $.post(
      "/wifi_setting",
      $("#Wifi_form").serializeArray(),
      function(data, status){
        $(Wifi_respond).html(data);
      });
  });
  
})