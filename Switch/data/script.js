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
      $("#ssid-list").empty();
      var content = ""
      for (item of data){
        if (item.ssid){
        content += "<option value='"+item.ssid+"'>"+item.ssid+"</option>";
        }
      }
      $("#ssid-list").html(content);
      $("#ssid-list").change();
      ssidlock = false;
    }
	});
}
// SSID end

// Multi-Switch
function doCalibration(item){
  $.get("http://"+$(item).parents("tr")[0].dataset.ip+"/calibration");
}

function editLabel(item){
  var td = $(item).parents("td")
  td.html('<input name="label" type="text" value='+td.find("a").html()+'></input>')
  td.find("input[name='label']").change(function(){
      var label = $(this).val();
      var tr = $(this).parents("tr");
      $.get("http://"+tr[0].dataset.ip+"/setLabel","label="+label)
      td.html("<a>"+label+'</a><button style="float: right;" onclick="editLabel(this)">edit label</button>');
      $("#client option[value='"+tr.prop("id")+"']").html(label);
    }
  )
}

function getClient(){
  $.get("/getClient",function(data,status){
    for (var item of data){
      // On Schedule page
      $("#client").append("<option value='"+item.id+"' data-ip='"+item.address+"'>"+item.label+"</option>");
      $("#client").change();
      $("#switchList").append('\
      <tr id='+item.id+' data-ip='+item.address+'>\
      <td><a>'+item.label+'</a><button style="float: right;" onclick="editLabel(this)">edit label</button></td>\
      <td><button onclick="doCalibration(this)">calibration</button></td>\
      <td>\
        <label class="switch">\
          <input type="checkbox">\
          <span class="slider round"></span>\
        </label>\
      </td>\
      </tr>'
    );
    setInterval(getSwitchStatus,3000);
    var target = $("#"+item.id+" input[type='checkbox']")
    target.change(function(){
      $.get("http://"+$(this).parents("tr")[0].dataset.ip+"/"+($(this).prop("checked")?"on":"off"))
    });
    }
  });
}

function getSwitchStatus(){
  var list = $("#switchList tr");
  for(var item of list){
    var target = $(item).find("input[type='checkbox']")
    $.get("http://"+item.dataset.ip+"/status",function(data,status){target.prop("checked",status)})
  }  
}
// End Multi-Switch

function addNewSchedule(){
  // [0] is hour, [1] is minute, [2] is action
  var form = $("#new-schedule");
  var hour = form.find("select[name='hour']").val();
  var minute = form.find("select[name='minute']").val();
  var action = form.find("select[name='action']").val();
  insetTable(minute,hour,action);
}

function insetTable(minute,hour,action){
  var cron = '0 '+minute+' '+hour+' * * * '+action
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
  var id = $("#client").val()
  var url = $("#client option[value='"+id+"']")[0].dataset.ip
  $.post("http://"+url+"/setSchedule",data);
}

window.onhashchange = openTab; 

//shorthand document.ready function
$(function () {
  if (location.hash == "") {
    location.hash = "#home"
  } else {
    openTab();
  }
  getClient();
  updateWifiStauts();
  setTimeout(updateSSIDlist,500);

  // Add option to Time
  var hour = $("#new-schedule select[name='hour']");
  var minutes = $("#new-schedule select[name='minute']")
  for(var i=0;i<24;i++){hour.append("<option value='"+i+"'>"+i+"</option>")}
  for(var i=0;i<60;i++){minutes.append("<option value='"+i+"'>"+i+"</option>")}

  $("#client").change(function(){
    $("#schedule-table tbody").empty();
    var id = $("#client").val()
    var url = $("#client option[value='"+id+"']")[0].dataset.ip
    $.get("http://"+url+"/getSchedule",function(data,status){
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