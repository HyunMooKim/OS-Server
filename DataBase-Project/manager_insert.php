﻿<?
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host,$dbid,$dbpass,$dbname);


$manager_id = $_POST['manager_id'];
$manager_password = $_POST['manager_password'];
$manager_nickname = $_POST['manager_nickname'];
$manager_name = $_POST['manager_name'];
$manager_phone = $_POST['manager_phone'];

// Check if the nickname is already taken
$check_query = mysqli_query($conn, "SELECT * FROM Manager WHERE 닉네임 = '$manager_nickname'");
if (mysqli_num_rows($check_query) > 0) {
    msg("이미 사용 중인 닉네임입니다. 다른 닉네임을 선택해주세요.");
    exit(); // Stop execution if the nickname is taken
}






$result = mysqli_query($conn, "INSERT INTO Manager (아이디, 비밀번호, 닉네임, 이름, 전화번호) VALUES ('$manager_id', '$manager_password','$manager_nickname', '$manager_name', '$manager_phone')");
//inserted ID
$MID = mysqli_insert_id($conn);  

if(!$result)
{
    msg('Query Error : '.mysqli_error($conn));
}
else
{
    $message = "성공적으로 입력되었습니다.\\n";
    $message .= "고유 아이디: $MID\\n";
    $message .= "관리자 아이디: $manager_id\\n";
    $message .= "닉네임: $manager_nickname\\n";
    $message .= "이름: $manager_name\\n";
    $message .= "전화번호: $manager_phone\\n";
    s_msg($message);
    echo "<script>location.replace('manager_list.php');</script>";
}



myspli_close($connect);
?>

