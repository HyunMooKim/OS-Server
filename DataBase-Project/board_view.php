﻿﻿<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host, $dbid, $dbpass, $dbname);
//get으로 넘어온 값이 있는지 확인
if (array_key_exists("BID", $_GET)) {
    $BID = $_GET["BID"];
    $query = "select * from Board left join Manager on 담당관리자 = MID where Board.BID = $BID";
    $result = mysqli_query($conn, $query);
    $Board = mysqli_fetch_assoc($result);
    if (!$Board) {
        msg("게시판이 존재하지 않습니다.");
    }
}
?>
    <div class="container fullwidth">

        <h3>게시판 정보 상세 보기</h3>

        <p>
            <label for="board_id">게시판 고유번호</label>
            <input readonly type="text" id="board_name" name="board_id" value="<?= $Board['BID'] ?>"/>
        </p>
        <p>
            <label for="board_name">게시판 이름</label>
            <input readonly type="text" id="board_name" name="board_name" value="<?= $Board['게시판이름'] ?>"/>
        </p>

        <p>
            <label for="manufacturer_id">게시판 설명</label>
            <input readonly type="text" id="manufacturer_id" name="manufacturer_id" value="<?= $Board['설명'] ?>"/>
        </p>

        <p>
            <label for="manufacturer_name">게시판 관리자</label>
            <input readonly type="text" id="manufacturer_name" name="manufacturer_name" value="<?= $Board['닉네임'] ?>"/>
        </p>
        <br><br>
        <p align="center">
        	<a href="board_list.php" class="button primary large">Go Back</a>
        </p>
    </div>
<? include "footer.php" ?>