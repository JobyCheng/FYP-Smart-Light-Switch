window.addEventListener('DOMContentLoaded', (event) => {
  init();
});

function ResponsiveNav() {
  if (document.getElementById("navbar").className === "navbar") {
    document.getElementById("navbar").classList.add("responsive");
  } else {
    document.getElementById("navbar").classList.remove("responsive");
  }
}

function openTab(){
  var i;
  var tab_id = location.hash;
  tab_id = tab_id.replace("#","");

  var tabs = document.getElementsByClassName("tab");
  for (item of tabs) {
    item.classList.remove("active");
  }

  var tabcontents = document.getElementsByClassName("tabContent");
  for (item of tabcontents) {
    item.style.display = "none";
  }
  document.getElementById(tab_id+"-tab").classList.add("active");
  document.getElementById(tab_id).style.display = "block";

  if (!(document.getElementById("navbar").className === "navbar")){
    ResponsiveNav();
  }
}

function checkPass(formID)
{
  var form = document.getElementById(formID);
  if (form == null){console.log("No such form");return;}

  form.getElementsByTagName("input")
  for (item of inputs) {
    if (item.name=="passwd"){
      if (item.value.length < 8){
        document.getElementById("AP_respond").innerHTML = "Password is less than 8 digit"
      }
    }
  }
}

function post(formID)
{
  var form = document.getElementById(formID);
  if (form == null){console.log("No such form");return;}

  // (A) GET FORM DATA
  var data = new FormData();
  var inputs = form.getElementsByTagName("input");
  for (item of inputs) {
    if (item.type=="text"){
      data.append(item.name, item.value);
    }
  }

  // (B) AJAX
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/"+formID);
  // What to do when server responds
  xhr.onload = function(){ console.log(this.response); };
  xhr.send(data);
}

window.onhashchange = openTab;

function init(){
  if(location.hash==""){
    location.hash="#home"
  }else{
    openTab();
  }
}