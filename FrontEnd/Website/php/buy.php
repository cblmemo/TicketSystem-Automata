<?php
$curUsername = $_POST['curUsername'];
$trainId = $_POST['trainId'];
$date = $_POST['date'];
$departure = $_POST['departure'];
$arrival = $_POST['arrival'];
$buyQuantity = $_POST['buyQuantity'];
$queue = $_POST['queue'];
$command = "buy_ticket -u $curUsername -i $trainId -d $date -f $departure -t $arrival -n $buyQuantity -q $queue";

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_connect($socket, '127.0.0.1', 10240);
socket_write($socket, strlen($command) . $command);
$returnContent = socket_read($socket, 20480);
socket_close($socket);

$result = substr($returnContent, 6, substr($returnContent, 0, 6));
echo $result;
