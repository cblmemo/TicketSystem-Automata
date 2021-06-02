<?php
$curUsername = $_POST['curUsername'];
$command = "query_order -u $curUsername";

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_connect($socket, '127.0.0.1', 10240);
socket_write($socket, strlen($command) . $command);
$returnContent = socket_read($socket, 20480);
socket_close($socket);

$result = substr($returnContent, 6, substr($returnContent, 0, 6));
echo $result;