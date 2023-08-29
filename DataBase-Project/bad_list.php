<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수
?>
<div class="container">
    <?
    $conn = dbconnect($host, $dbid, $dbpass, $dbname);
    // bring me Thanos
    $query = "
    SELECT * FROM User where 신고 >= 10 
    ";
    $result = mysqli_query($conn, $query);
    if (!$result) {
         die('Query Error : ' . mysqli_error());
    }

    ?>
   
    <table class="table table-striped table-bordered">
        <tr>
            <th>No.</th>
            <th>악질 유저 닉네임</th>
            <th>유저 아이디</th>
            <th>누적 신고수</th>
            <th>기능</th>

        </tr>
        <?
        $row_index = 1;
        while ($row = mysqli_fetch_array($result)) {  
            echo "<tr>";
            echo "<td>{$row_index}</td>";
            echo "<td>{$row['닉네임']}</td>";
            echo "<td>{$row['아이디']}</td>";
            echo "<td>{$row['신고']}</td>";
            echo "<td width='17%'>
                <a href='bad_view.php?UID={$row['UID']}'><button class='button primary small'>나쁜짓확인</button></a>
   
                </td>";
            echo "</tr>";
            $row_index++;
        }
        ?>
    </table>
    
 
   
</div>
<? include("footer.php") ?>

