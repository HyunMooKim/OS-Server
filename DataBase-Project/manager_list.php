<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수
?>
<div class="container">
    <?
    $conn = dbconnect($host, $dbid, $dbpass, $dbname);
    $query = "select * from Manager";
    $result = mysqli_query($conn, $query);
    if (!$result) {
         die('Query Error : ' . mysqli_error());
    }
    //비번확인을 위해 최고 관리자 비번 가져옴
    $password = mysqli_query($conn, "select 비밀번호 from Manager where MID = 1");
    $password_row = mysqli_fetch_array($password);
    $manager_password = $password_row['비밀번호'];
    ?>
     <p align='right'> 삭제 시 비밀번호 입력: 0070 <input type='password' id='password' name='password'></p>
    <table class="table table-striped table-bordered">
        <tr>
            <th>No.</th>
            <th>닉네임</th>
            <th>이름</th>
            <th>전화번호</th>
            <th>기능</th>
        </tr>
        <?
        $row_index = 1;
        while ($row = mysqli_fetch_array($result)) {
            echo "<tr>";
            echo "<td>{$row_index}</td>";
            echo "<td><a href='manager_view.php?MID={$row['MID']}'>{$row['닉네임']}</a></td>";
            echo "<td>{$row['이름']}</td>";
            echo "<td>{$row['전화번호']}</td>";
            if ($row_index != 1){
            echo "<td width='17%'>
                <a href='manager_form.php?MID={$row['MID']}'><button class='button primary small'>수정</button></a>
                <button onclick='javascript:deleteConfirm({$row['MID']})' class='button danger small'>삭제</button>
                </td>";
            }
            else{
            	   echo "<td>최고관리자!</td>";
            }
            echo "</tr>";
            $row_index++;
        }
        ?>
    </table>
    
    <script>
        function deleteConfirm(MID) {
            var password = document.getElementById('password').value;
            if (password == '<?=$manager_password?>') { // Replace 'YOUR_PASSWORD' with the actual password from the Manager table
                if (confirm("정말 삭제하시겠습니까?") == true) { // 확인
                    window.location = "manager_delete.php?MID=" + MID;
                } else { // 취소
                    return;
                }
            } else {
                alert("비밀번호가 일치하지 않습니다.");
            }
        }
    </script>
   
</div>
<? include("footer.php") ?>
