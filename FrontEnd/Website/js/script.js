/*
 *  全局脚本
 */

// function sleep(msTime = 500) { (function () { setTimeout(function () { }, msTime) }); return; }
var sleep = function (time) {
    var startTime = new Date().getTime() + parseInt(time, 10);
    while (new Date().getTime() < startTime) { }
};

function setCookie() {
    if (arguments.length == 2)
        document.cookie = arguments[0] + "=" + arguments[1] + ";";
    else if (arguments.length == 3) {
        var cname = arguments[0];
        var cvalue = arguments[1];
        var exdays = arguments[2];
        var d = new Date();
        d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000));
        var expires = "expires=" + d.toGMTString();
        document.cookie = cname + "=" + cvalue + "; " + expires;
    }
}

function getCookie(cname) {
    var name = cname + "=";
    var ca = document.cookie.split(';');
    for (var i = 0; i < ca.length; i++) {
        var c = ca[i].trim();
        if (c.indexOf(name) == 0)
            return c.substring(name.length, c.length);
    }
    return "";
}

function delCookie(name) {
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var value = getCookie(name);
    if (value != null)
        document.cookie = name + "=" + value + ";expires=" + exp.toUTCString();
}

function logout() {
    var xmlhttp = new XMLHttpRequest();
    var username = getCookie("username");
    if (username != "") {
        delCookie("username");

        xmlhttp.open("POST", "php/logout.php", true);
        xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xmlhttp.onreadystatechange = function () {
            if (xmlhttp.readyState == 4) {
                if (xmlhttp.status == 200) {
                    var returnMsg = xmlhttp.responseText;
                    if (returnMsg != "0") alert("Logout failed!");
                } else alert("Meet unexpected error during logout :(");
                window.location.href = "index.html";
            }
        }
        xmlhttp.send("username=" + username);
    } else {
        alert("User login status error!");
        window.location.href = "index.html";
    }
}

function setUserRealName(realNameLabel) {
    var xmlhttp = new XMLHttpRequest();
    var username = safeGetUsername();
    if (username != "") {
        xmlhttp.open("POST", "php/search.php", true);
        xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xmlhttp.onreadystatechange = function () {
            if (xmlhttp.readyState == 4) {
                if (xmlhttp.status == 200) {
                    var returnMsg = xmlhttp.responseText;
                    if (returnMsg == "-1") alert("Get user name failed!");
                    else realNameLabel.textContent = returnMsg.split(' ')[1];
                } else {
                    alert("Meet unexpected error during querying user name :(");
                    window.location.href = "index.html";
                }
            }
        }
        xmlhttp.send("curUsername=" + username + "&username=" + username);
    }
}

function safeGetUsername() {
    var usernameCookie = getCookie("username");
    if (usernameCookie == "") {
        alert("You haven't logged in!");
        window.location.href = "index.html";
    }
    return usernameCookie;
}