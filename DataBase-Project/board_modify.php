﻿<?
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host,$dbid,$dbpass,$dbname);

mysqli_query($conn, "set autocommit = 0");
mysqli_query($conn, "set session transaction isolation level serializable"); 
mysqli_query($conn, "begin");


$BID = $_POST['BID'];
$board_name = $_POST['board_name'];
$board_desc = $_POST['board_desc'];
$board_manager = $_POST['board_manager'];

if ( empty($board_manager) == true ){
	$result = mysqli_query($conn, "UPDATE Board SET 게시판이름 = '$board_name', 설명 = '$board_desc', 담당관리자 = NULL WHERE BID = $BID");
}
else{
	$result = mysqli_query($conn, "UPDATE Board SET 게시판이름 = '$board_name', 설명 = '$board_desc', 담당관리자 = $board_manager WHERE BID = $BID");
}


if(!$result)
{
	//if Manger was deleted while modifying 
	mysqli_query($conn, "rollback");
    msg('게시판 수정에 실패했습니다. 다시 시도해주세요. Query Error : '.mysqli_error($conn));
}
else
{
    s_msg ('성공적으로 수정 되었습니다');
    echo "<script>location.replace('board_list.php');</script>";
    mysqli_query($conn, "commit");
}
myspli_close($connect);
?>

