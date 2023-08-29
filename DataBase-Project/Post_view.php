<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host, $dbid, $dbpass, $dbname);

//get으로 넘어온 값이 있는지 확인
if (array_key_exists("UID", $_GET)) {
    $UID = $_GET["PID"];
    //mysql에선 int string 잘 비교해서 갖다주었지만,
    //php 에선 int인 작성자와 stirng인 get UID 를 비교 못한다. 
    $UID_C = intval($_GET['UID']);
}

//get으로 넘어온 값이 있는지 확인
if (array_key_exists("PID", $_GET)) {
    $PID = $_GET["PID"];
    $query = "SELECT * FROM Post WHERE Post.PID = $PID";
    $result = mysqli_query($conn, $query);
    $Post = mysqli_fetch_assoc($result);
}

//get으로 넘어온 값이 있는지 확인
if (array_key_exists("CID", $_GET)) {
    $CID = $_GET["CID"];
    // Retrieve the '글' attribute from the Comment table
    $query = "SELECT 글 FROM Comment WHERE CID = $CID";
    $result = mysqli_query($conn, $query);
    $comment = mysqli_fetch_assoc($result);
    $thePID = $comment['글'];
    
    // Find the Post where PID equals thePID
    $query = "SELECT * FROM Post WHERE PID = $thePID";
    $result = mysqli_query($conn, $query);
    $Post = mysqli_fetch_assoc($result);
}

$worked = $_GET['worked'] ?? '';

?>
    <div class="container fullwidth">

    <h3>악질 유저 활동 정보 상세 보기</h3>
    <h4>해당 유저의 활동은 빨간색으로 표시됩니다.</h4><br><br><hr style="height:5px; background: #000;">
    <!-- Display Post information -->
  
    <?php
    $isUserPost = $Post['작성자'] == $UID_C;
    $postColor = $isUserPost ? 'red' : 'black';
    ?>
    
    <h4> <p style="color: <?= $postColor ?>">게시물 제목: <?= $Post['글제목']?></p></h4>
    <span style="color: <?= $postColor ?>">게시물 고유번호: <?= $Post['PID']?></span>
    <p style="color: <?= $postColor ?>">게시물 내용: <?= $Post['글내용']?></p>
    <hr>   

    <!-- Display Comment information -->
    <?
    $query = "SELECT * FROM Comment WHERE 글 = " . $Post['PID'];
    $result = mysqli_query($conn, $query);
    

while ($comment = mysqli_fetch_assoc($result)) {
    // Check if the Comment is from the User
    $isUserComment = $comment['작성자'] == $UID_C;
    
    // Apply different color to User-related comments
    $commentColor = $isUserComment ? 'red' : 'black';
    
    // Display each Comment with appropriate styling
    echo '<div style="color: ' . $commentColor . '">';

    echo "<p>->댓글 내용: " . $comment['댓글내용'] . "</p>";
    // Display more Comment attributes if needed
    
    // Display Recomment information for each Comment
    $query = "SELECT * FROM Recomment WHERE 댓글 = " . $comment['CID'];
    $recommentResult = mysqli_query($conn, $query);
    while ($recomment = mysqli_fetch_assoc($recommentResult)) {
        // Check if the Recomment is from the User
        $isUserRecomment = $recomment['작성자'] == $UID_C;
        
        // Apply different color to User-related recomments
        $recommentColor = $isUserRecomment ? 'red' : 'black';
        
        // Display each Recomment with appropriate styling
        echo '<div style="color: ' . $recommentColor . '">';

        echo "<p>--->대댓글 내용: " . $recomment['대댓글내용'] . "</p>";
        // Display more Recomment attributes if needed
        echo '</div>';
    }
    
    echo '</div>';
}
    ?>


    <br><br>
        <p align="center">
        	<a href="bad_view.php?UID=<?= $UID_C ?>&worked=<?=$worked?>" class="button primary large">Go Back</a>
        </p>
    </div>
<? include "footer.php" ?>