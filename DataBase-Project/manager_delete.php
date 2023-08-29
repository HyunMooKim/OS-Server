﻿<?
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host,$dbid,$dbpass,$dbname);

$MID = $_GET['MID'];

//Before delete Manager, you should update 담당관리자 in Table Board to NULL!
$updateQuery = "UPDATE Board SET 담당관리자 = NULL WHERE 담당관리자 = $MID";
$updateResult = mysqli_query($conn, $updateQuery);

$ret = mysqli_query($conn, "delete from Manager where MID = $MID");

if(!$ret)
{
	msg('Query Error : '.mysqli_error($conn));
}
else
{
	s_msg ('성공적으로 삭제 되었습니다');
	echo "<meta http-equiv='refresh' content='0;url=manager_list.php'>";
}	

?>
