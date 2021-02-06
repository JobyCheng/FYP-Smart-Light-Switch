function main(){
    console.log("script loaded");
}

scanlock = false;

function scan(){
    if (scanlock) {console.log("please wait for a few second.");return;}
    scanlock = true;
    console.log("locked")
    setTimeout(function(){}, 3000);

    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() {
        var target = document.getElementById("result");

        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            //removing all child first
            while (target.hasChildNodes()) {
                target.removeChild(target.childNodes[0]);
            }
            //adding the child
            for (result of myObj){
                console.log(result);
                var temp = document.createElement("div");
                temp.innerHTML = result.ssid;
                target.appendChild(temp);
            }
        }

        console.log("unlocked");
        scanlock = false;
    };

    xmlhttp.open("GET", "/scan", true);
    xmlhttp.send();
}

function sendText()
{
    // (A) GET FORM DATA
    var data = new FormData();
    data.append("fname", document.getElementById("fname").value);
    data.append("lname", document.getElementById("lname").value);
   
    // (B) AJAX
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/postText");
    // What to do when server responds
    xhr.onload = function(){ console.log(this.response); };
    xhr.send(data);
}

console.clear();
main();