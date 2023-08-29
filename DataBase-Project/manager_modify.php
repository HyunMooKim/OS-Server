﻿<?
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host,$dbid,$dbpass,$dbname);

$MID = $_POST['MID'];
$manager_id = $_POST['manager_id'];
$manager_password = $_POST['manager_password'];
$manager_nickname = $_POST['manager_nickname'];
$manager_name = $_POST['manager_name'];
$manager_phone = $_POST['manager_phone'];


$result = mysqli_query($conn, "UPDATE Manager SET 아이디 = '$manager_id', 비밀번호 = '$manager_password', 닉네임 = '$manager_nickname', 이름 = '$manager_name',전화번호 ='$manager_phone' WHERE MID = $MID ");



if(!$result)
{
    msg('Query Error : '.mysqli_error($conn));
}
else
{
    s_msg ('성공적으로 수정 되었습니다');
    echo "<script>location.replace('manager_list.php');</script>";
}

?>



