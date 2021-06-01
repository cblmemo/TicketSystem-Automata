$(document).ready(function () {
    $('#loginFrame').ajaxForm(function (phpReturnText) {
        alert("Hear!" + phpReturnText);
        /*if (phpReturnText == "0") {
            $('#loginStatus').text("Success");
        } else {
            $('#loginStatus').text("Fail");
        }*/
    });
});