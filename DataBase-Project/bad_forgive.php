<?php

include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host,$dbid,$dbpass,$dbname);
mysqli_query($conn, "set autocommit = 0");
mysqli_query($conn, "set session transaction isolation level serializable"); 
mysqli_query($conn, "begin");


// Retrieve the UID from the POST data
$UID = $_POST['UID'];

// Update the User table
$query = "UPDATE User SET 신고 = 0 WHERE UID = $UID";
$result = mysqli_query($conn, $query);

if ($result == false ) {
    //if User is deleted while inserting 
	mysqli_query($conn, "rollback");
    msg('유저가 이미 도망 갔습니다.');
}


// Update the Post table if it exists and has matching rows
if (mysqli_query($conn, "SELECT 1 FROM Post WHERE 작성자 = $UID LIMIT 1")) {
    $query = "UPDATE Post SET 글신고 = 0 WHERE 작성자 = $UID";
    mysqli_query($conn, $query);
}
else{
	mysqli_query($conn, "rollback");
    msg('더 이상 존재하지 않는 활동이 포함된 듯 합니다. ');
}



// Update the Comment table if it exists and has matching rows
if (mysqli_query($conn, "SELECT 1 FROM Comment WHERE 작성자 = $UID LIMIT 1")) {
    $query = "UPDATE Comment SET 댓글신고 = 0 WHERE 작성자 = $UID";
    mysqli_query($conn, $query);
}
else{
	mysqli_query($conn, "rollback");
    msg('더 이상 존재하지 않는 활동이 포함된 듯 합니다. ');
}



// Update the Recomment table if it exists and has matching rows
if (mysqli_query($conn, "SELECT 1 FROM Recomment WHERE 작성자 = $UID LIMIT 1")) {
    $query = "UPDATE Recomment SET 대댓글신고 = 0 WHERE 작성자 = $UID";
    mysqli_query($conn, $query);
}
else{
	mysqli_query($conn, "rollback");
    msg('더 이상 존재하지 않는 활동이 포함된 듯 합니다. ');
}



//if all worked well
mysqli_query($conn, "commit");
mysqli_close($connect);

// Redirect back to the bad_view.php page
header("Location: bad_view.php?UID=$UID&worked=1");
exit();
?>