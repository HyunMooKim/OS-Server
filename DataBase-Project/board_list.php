<?
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host,$dbid,$dbpass,$dbname);

mysqli_query($conn, "set autocommit = 0");
mysqli_query($conn, "set session transaction isolation level serializable"); 
mysqli_query($conn, "begin");

$board_name = $_POST['board_name'];
$board_desc = $_POST['board_desc'];
$board_manager = $_POST['board_manager'];

// Check if the board name is already taken
$check_query = mysqli_query($conn, "SELECT * FROM Board WHERE 게시판이름 = '$board_name'");
if (mysqli_num_rows($check_query) > 0) {
    msg("이미 사용 중인 게시판 이름입니다. 다른 이름을 선택해주세요.");
    exit(); // Stop execution if the board name is taken
}

if ( empty($board_manager) == true ){
	$result = mysqli_query($conn, "INSERT INTO Board (게시판이름, 설명, 담당관리자) VALUES ('$board_name', '$board_desc', NULL)");
}
else{
	$query =  "select * from Manager where MID = $board_manager";
	$temp = mysqli_query($conn, $query);
	$row = mysqli_fetch_array($temp);
	$result = mysqli_query($conn, "INSERT INTO Board (게시판이름, 설명, 담당관리자) VALUES ('$board_name', '$board_desc', '{$row['MID']}')");
  	
}


if(!$result)
{
	//if Manger was deleted while inserting 
	mysqli_query($conn, "rollback");
    msg('게시판 생성에 실패했습니다. 다시 시도하여 주십시오 Query Error : '.mysqli_error($conn));
}
else
{
	
    //inserted ID
    $BID = mysqli_insert_id($conn);  
	$query = "SELECT * FROM Manager WHERE MID = $board_manager";
	$result = mysqli_query($conn, $query);
	$board_manager_name = mysqli_fetch_assoc($result);
	$board_desc = substr($_POST['board_desc'], 0, 30);
	
    $message = "성공적으로 입력되었습니다.\\n";
    $message .= "고유 아이디: $BID\\n";
    $message .= "게시판이름: $board_name\\n";
    $message .= "설명: $board_desc\\n";
    $message .= "관리자: {$board_manager_name['이름']}";
    s_msg($message);
    echo "<script>location.replace('board_list.php');</script>";
    mysqli_query($conn, "commit");
}

mysqli_close($connect);
?>

