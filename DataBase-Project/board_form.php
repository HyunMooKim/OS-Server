<?
include "header.php";
include "config.php";    //데이터베이스 연결 설정파일
include "util.php";      //유틸 함수

$conn = dbconnect($host, $dbid, $dbpass, $dbname);
$mode = "입력";
$action = "board_insert.php";

if (array_key_exists("BID", $_GET)) {
    $BID = $_GET["BID"];
    $query =  "select * from Board where BID = $BID";
    $result = mysqli_query($conn, $query);
    $Board = mysqli_fetch_array($result);
    if(!$Board) {
        msg("게시판이 존재하지 않습니다.");
    }
    $mode = "수정";
    $action = "board_modify.php";
}

$query = "select * from Manager";
$result = mysqli_query($conn, $query);
while($row = mysqli_fetch_array($result)) {
    $Manager[$row['MID']] = $row['닉네임'];
}
?>
    <div class="container">
        <form name="board_form" action="<?=$action?>" method="post" class="fullwidth">
            <input type="hidden" name="BID" value="<?=$Board['BID']?>"/>
            <h3>게시판 정보 <?=$mode?></h3>

            <p>
                <label for="board_name">게시판 이름</label>
                <input type="text" placeholder="게시판 이름 입력" id="board_name" name="board_name" value="<?=$Board['게시판이름']?>"/>
            </p>
            <p>
                <label for="board_desc">게시판 설명</label>
                <textarea placeholder="게시판 설명 입력" id="board_desc" name="board_desc" rows="10"><?=$Board['설명']?></textarea>
            </p>
            <p>
                <label for="board_manager">담당 관리자</label>
                <select name="board_manager" id="board_manager">
                    <option value="-1">선택해 주십시오.</option>
                    <option value="">없음</option>
                    <?
                        foreach($Manager as $MID => $닉네임) {
                            if($MID == $Board['담당관리자']){
                                echo "<option value='{$MID}' selected>{$닉네임}</option>";
                            } else {
                                echo "<option value='{$MID}'>{$닉네임}</option>";
                            }
                        }
                    ?>
                </select>
            </p>

            <p align="center"><button class="button primary large" onclick="javascript:return validate();"><?=$mode?></button></p>

           <script>
    		function validate() {

        		var boardName = document.getElementById("board_name").value;
        		if (boardName.trim().length == 0) {
            		alert("게시판 이름을 입력해 주십시오");
            		return false;
        		    }
        
        		if (boardName.length > 20) {
            		alert("게시판 이름은 20자 이내로 입력해 주십시오");
            		return false;
        		    }
        		    
         		if (document.getElementById("board_manager").value == "-1") {
        			alert("게시판 관리자를 선택해 주십시오");
        			return false;
        	     	}
       
        		return true;
    	        }
         </script>

        </form>
    </div>
<? include("footer.php") ?>