<?php
$curUsername = $_POST['curUsername'];
$realName = $_POST['realName'];
$email = $_POST['email'];
$password = $_POST['password'];
if ($realName != null) $realName = " -n " . $realName;
if ($email != null) $email = " -m " . $email;
if ($password != null) $password = " -p " . $password;


$command = "modify_profile -c $curUsername -u $curUsername$realName$email$password";

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_connect($socket, '127.0.0.1', 10240);
socket_write($socket, strlen($command) . $command);
$returnContent = socket_read($socket, 20480);
socket_close($socket);

$result = substr($returnContent, 6, substr($returnContent, 0, 6));
echo $result;