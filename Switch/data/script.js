// Responsive Webpage
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
// End Responsive Webpage

// Wifi
wifilock = false;
function updateWifiStauts(){
	if (wifilock) {console.log("Please wait for a few second.");return;}
    wifilock = true;
	$.get("/wifiStauts",function(data,status){
		$("#wifiStatus").html(data)
		wifilock = false;
	});
}
// End Wifi

// SSID
ssidlock = false;
function updateSSIDlist(){
	if (ssidlock) {console.log("Please wait for a few second.");return;}
  ssidlock = true;
  getSSID();
}

function getSSID(){
	$.get("/SSIDlist",function(data,status,xhr){
    if(xhr.status == 202){setTimeout(getSSID,500);}
    if(xhr.status == 200){
      //console.log(data);
      $("#SSID").empty();
      var content = "<optgroup label='Search Result'>"
      for (item of data){
        if (item.ssid){
        content += "<option value='"+item.ssid+"'>"+item.ssid+"</option>";
        }
      }
      content += "<optgroup label='---------'><option value='hidden'>Hidden Network</option>"
      $("#SSID").html(content);
      $("#SSID").change();
      ssidlock = false;
    }
	});
}
// SSID end

// Multi-Switch
function addClient(data){
  for (var item of data){
    $("#switchList").append('\
      <tr>\
      <td>'+item.identifier+'</td>\
      <td>\
        <label class="switch">\
          <input type="checkbox" id='+item.id+'>\
          <span class="slider round"></span>\
        </label>\
      </td>\
      </tr>'
    );

    var target = $("#"+item.id)
    target.prop("checked",item.status)

    target.change(function(){
      $.get("http://"+target.prop("id")+".local/"+(target.prop("checked")?"on":"off"))
    }
    );

    $("#client").empty();
    content = '';
    for (item of data){
      content += "<option value='"+item.id+"'>"+item.identifier+"</option>";
    }
  $("#client").html(content);
  $("#client").change();
  }
}

function getClientList(){
  $.get("/getClient",function(data,status){
    addClient(data);
  });
}
// End Multi-Switch

function addNewSchedule(){
  // [0] is hour, [1] is minute, [2] is action
  form = $("#new-schedule").serializeArray()
  insetTable(form[1].value,form[0].value,form[2].value)
}

function insetTable(minute,hour,action){
  cron = '0 '+minute+' '+hour+' * * * '+action
  if (matchTable(cron)){return;}
  $("#schedule-table tbody").append(
    "<tr data-cron='"+cron+"'>\
      <th>"+hour+":"+(minute<10?"0":"")+minute+"</th>\
      <th>"+action+"</th>\
      <th><button onclick='removeTable(this)'>X</button></th>\
    </tr>"
  )
}

function removeTable(item){
  $(item).parents('tr').remove()
}

function matchTable(cron){
  table = $("#schedule-table tbody tr")
  for(item of table){
    splitCron = cron.split(' ');
    splitItem = item.dataset.cron.split(' ');
    if ((splitCron[1]==splitItem[1])&&(splitCron[2]==splitItem[2])){
      return true;
    };
  }
  return false
}

function applyTable(){
  var entry = $("#schedule-table tbody tr");
  var data = [];
  var num = 0;
  for(var item of entry){
    temp = {
      name:num.toString(10),
      value:item.dataset.cron
    }
    data[num++]=temp;
  }
  $.post("/setSchedule",data);
}

window.onhashchange = openTab; 


//shorthand document.ready function
$(function () {
  if (location.hash == "") {
    location.hash = "#home"
  } else {
    openTab();
  }
  getClientList();
  updateWifiStauts();
  updateSSIDlist();

  // Add option to Time
  for(var i=0;i<24;i++){
    $("#hour").append(
      "<option value='"+i+"'>"+i+"</option>"
    )
  }
  for(var i=0;i<60;i++){
    $("#minute").append(
      "<option value='"+i+"'>"+i+"</option>"
    )
  }

  $("#SSID").change(function(){
    if ($("#Wifi_form select[name=SSID]").val() == "hidden"){
      $("#hidden_input").css("display","block");
    }else{
      $("#hidden_input").css("display","none");
    }
  })

  $("#client").change(function(){
    $("#schedule-table tbody").empty();
    $.get("http://"+$("#client-selector #client").val()+".local/getSchedule",function(data,status){
      for(item of data){
        splitItem = item.value.split(' ');
        insetTable(splitItem[1],splitItem[2],splitItem[6]);
      }
    })
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