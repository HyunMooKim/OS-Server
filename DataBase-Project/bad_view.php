<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host, $dbid, $dbpass, $dbname);
//get으로 넘어온 값이 있는지 확인
if (array_key_exists("UID", $_GET)) {
    $UID = $_GET["UID"];
    $query = "
    SELECT * FROM User
	where UID = $UID
    ";
    $result_U = mysqli_query($conn, $query);
    $User = mysqli_fetch_assoc($result_U);
    $query = "
    SELECT * FROM Post
	where 작성자 = $UID
    ";
    $result_P = mysqli_query($conn, $query);
    $query = "
    SELECT * FROM Comment
	where 작성자 = $UID
    ";
    $result_C = mysqli_query($conn, $query);
    $query = "
    SELECT * FROM Recomment
	where 작성자 = $UID
    ";
    $result_R = mysqli_query($conn, $query);
    if (!$User) {
        msg("유저 정보가 존재하지 않습니다.");
    }
}

$worked = $_GET['worked'] ?? '';

// Check if the "worked" parameter exists in the GET request
if ($worked) {
    $hideButtons = true; // Set a flag to hide the buttons
} else {
    $hideButtons = false; // Set a flag to show the buttons
}
?>
    <div class="container fullwidth">

        <h3>유저 상세 정보 보기 </h3>
		<p>
            <label for="product_id">고유 번호</label>
            <input readonly type="text" id="board_name" name="product_id" value="<?=$User['UID']?>"/>
        </p>
        <p>
            <label for="product_id">닉네임</label>
            <input readonly type="text" id="board_name" name="product_id" value="<?= $User['닉네임'] ?>"/>
        </p>
        <p>
            <label for="product_id">아이디</label>
            <input readonly type="text" id="board_name" name="product_id" value="<?= $User['아이디'] ?>"/>
        </p>

        <p>
            <label for="manufacturer_id">누적 신고수</label>
            <input readonly type="text" id="manufacturer_id" name="manufacturer_id" value="<?= $User['신고'] ?>"/>
        </p>

    <p>
    <label for="manufacturer_id">유저활동</label>
    <table class="table table-striped table-bordered">
        <tr>
            <th>종류</th>
            <th>제목</th>
            <th>내용</th>
            <th>신고수</th>
            <th>날짜</th>
        </tr>
        <?php
        // Post
        if (mysqli_num_rows($result_P) > 0) {
            while ($row = mysqli_fetch_array($result_P)) {
                echo "<tr>";
                echo "<td>Post</td>";
                echo "<td>{$row['글제목']}</td>";
                echo "<td><a href='Post_view.php?PID={$row['PID']}&UID=$UID&worked=$worked'>{$row['글내용']}</a></td>";
                echo "<td>{$row['글신고']}</td>";
                echo "<td>{$row['글날짜']}</td>";
                echo "</tr>";
            }
        }
        // Comment
        if (mysqli_num_rows($result_C) > 0) {
            while ($row = mysqli_fetch_array($result_C)) {
                echo "<tr>";
                echo "<td>Comment</td>";
                echo "<td>댓글</td>";
                echo "<td><a href='Post_view.php?PID={$row['글']}&UID=$UID&worked=$worked'>{$row['댓글내용']}</td>";
                echo "<td>{$row['댓글신고']}</td>";
                echo "<td>{$row['댓글날짜']}</td>";
                echo "</tr>";
            }
        }
        // Recomment
        if (mysqli_num_rows($result_R) > 0) {
            while ($row = mysqli_fetch_array($result_R)) {
                echo "<tr>";
                echo "<td>Recomment</td>";
                echo "<td>대댓글</td>";
                echo "<td><a href='Post_view.php?CID={$row['댓글']}&UID=$UID&worked=$worked'>{$row['대댓글내용']}</td>";
                echo "<td>{$row['대댓글신고']}</td>";
                echo "<td>{$row['대댓글날짜']}</td>";
                echo "</tr>";
            }
        }
        ?>
    </table>
</p>
        <br>
        
        
    <div style="display: flex; justify-content: space-between;">
        <a style="text-align: right;" href="bad_list.php" class="button primary large">Go Back</a>
<?php if (!$hideButtons): ?>
<form method="post" action="bad_punish.php" onsubmit="return confirm('활동 내용을 모두 지우고 신고수를 초기화하시겠습니까?');">
    <input type="hidden" name="UID" value="<?= $UID ?>">
    <button type="submit" class="button primary large" style="background-color: red;">철퇴</button>
</form>

<form method="post" action="bad_forgive.php" onsubmit="return confirm('활동 내용을 그대로 두고 신고수를 초기화하시겠습니까?');">
    <input type="hidden" name="UID" value="<?= $UID ?>">
    <button type="submit" class="button primary large">용서</button>
</form>
<?php endif; ?>

    </div>


</div>
    <br><br><br>


<? include "footer.php" ?>