﻿﻿<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host, $dbid, $dbpass, $dbname);
//get으로 넘어온 값이 있는지 확인
if (array_key_exists("MID", $_GET)) {
    $MID = $_GET["MID"];
    $query = "SELECT Manager.*, Board.* FROM Manager LEFT JOIN Board ON Manager.MID = Board.담당관리자 WHERE Manager.MID = $MID";
    $result = mysqli_query($conn, $query);
    $Manager = mysqli_fetch_assoc($result);
    if (!$Manager) {
        msg("관리자가 존재하지 않습니다.");
    }
}
?>
    <div class="container fullwidth">

        <h3>관리자 정보 상세 보기</h3>

        <p>
            <label for="MID">관리자 고유번호</label>
            <input readonly type="text" id="MID" name="MID" value="<?= $Manager['MID'] ?>"/>
        </p>
             <p>
            <label for="manager_id">관리자 아이디</label>
            <input readonly type="text" id="manager_id" name="manager_id" value="<?= $Manager['아이디'] ?>"/>
        </p>
        <p>
            <label for="manager_nickname">관리자 닉네임</label>
            <input readonly type="text" id="manager_nickname" name="manager_nickname" value="<?= $Manager['닉네임'] ?>"/>
        </p>
        <p>
            <label for="manager_name">관리자 이름</label>
            <input readonly type="text" id="manager_name" name="manager_name" value="<?= $Manager['이름'] ?>"/>
        </p>
        <p>
            <label for="manager_phone">관리자 전화번호</label>
            <input readonly type="text" id="manager_phone" name="manager_phone" value="<?= $Manager['전화번호'] ?>"/>
        </p>
        <p>
        <label for="managing_board">관리중인 게시판</label>
        <?php
        	$query = "SELECT * FROM Board WHERE 담당관리자 = $MID";
        	$result = mysqli_query($conn, $query);
        
        	if (mysqli_num_rows($result) > 0) {
        		while ($row = mysqli_fetch_assoc($result)) {
                	echo "<input readonly type='text' value='{$row['게시판이름']}'/><br>";
            	}
        	} else {
            	echo "<br>"."현재 관리 중인 게시판이 없습니다.";
        	}
        ?>
    </p>



        <br><br>
        <p align="center">
        	<a href="manager_list.php" class="button primary large">Go Back</a>
        </p>
    </div>
<? include "footer.php" ?>